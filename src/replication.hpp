#define REPLICATION_DEBUG 0

#pragma once

#include <list>
#include <memory>

enum client_state { not_ready, syncing, sync_finishing, running};
struct Client_t {
	std::string id;
	client_state state;
	std::list<std::string> current_keys;
	std::list< std::pair<std::string,std::string> > queue;
	int queue_pipelined;
	spinlock mtx;
	static const int pipeline_size = 100;

	void write_status(std::stringstream &sstr) {
		std::lock_guard<spinlock> lck(mtx);
		sstr << "{\"id\":\"" << id << "\",\"state\":\"";
		switch (state) {
		case not_ready:
			sstr<<"not_ready";
			break;
		case syncing:
			sstr<<"syncing";
			break;
		case sync_finishing:
			sstr<<"sync_finishing";
			break;
		case running:
			sstr<<"running";
			break;
		}
		sstr << "\",\"queue\":" << queue.size();
		sstr << ",\"current\":";
		std::string str;
		aj::value<std::string>::serializer s;
		s(current(),std::back_inserter(str));
		sstr << str << "}";
	}

	std::string current() const {
		return current_keys.empty()?"" : current_keys.back();
	}

	void enqueue(std::pair<std::string,std::string> &buf) {
		std::lock_guard<spinlock> lck(mtx);
		queue.push_back(buf);
	}


	template<typename T>
	void send(const std::string &cmd, const typename T::target &x){
		//todo: serialize set(valmap2str)
		typename T::serializer s;
		std::string buf;

		s(x,std::back_inserter(buf));
		if (REPLICATION_DEBUG>0) {
			std::cout << buf << "\n";
			if (buf=="{}") gdb_break();
		}
		std::pair<std::string,std::string> p(cmd,buf);
		enqueue(p);
	}
/*	void send(const vallist&x){
		//todo: serialize del(valmap2str)
		typename vallist_json::serializer s;
		std::string buf;
		s(x,std::back_inserter(buf));
		if (REPLICATION_DEBUG>0) {
			std::cout << buf << "\n";
		}
		enqueue(buf);
	}*/

	template<typename connection>
	void on_confirm(connection &conn) {	//если клиент подтверждает
		std::lock_guard<spinlock> lck(mtx);
		if (state == syncing || state == sync_finishing) {
			//std::cout << current_keys.size() << "sz \n";
			if (current_keys.empty()) throw std::logic_error("fail");
			std::string last = current_keys.back();
			current_keys.pop_front();
			if (state == syncing) {
				_push_next_portion(conn, last);
			} else if (state == sync_finishing && current_keys.empty()) {
				_sync_queue(conn);
			}
		} else if (state == running) {
			if (queue.empty()) throw std::logic_error("fail: empty queue but something returned");
			queue_pipelined--;
			queue.pop_front();
			//в очереди есть ещё записи и мы можем их выслать
			_push_queue_items(conn);
		}
	}

	std::vector<std::unique_ptr<std::thread>> schthread;

	template<typename connection>
	void _schedule_queue(connection &conn) {
		//добавляет в очередь задач (задачу на проверку очереди у клиента/соединения
		//schthread.push_back(std::unique_ptr<std::thread>(new std::thread([&conn,this]{
		std::thread thr = std::thread([&conn,this]{
			usleep(1000);
			if (!conn.get_status()) return;
			std::lock_guard<spinlock> lck(mtx);
			_push_queue_items(conn);
		});
		thr.detach();
		//})));
	}

	/**
	 * следующая порция данных для первичного заполнения
	 */
	template<typename connection>
	std::string _push_next_portion(connection &conn, std::string from) {
		//for (auto it = begin();it!=end();it++) {
		valmap2str data;

		leveldb::Iterator* it = getDb()->NewIterator(leveldb::ReadOptions());
		if (from.length()) {
			it->Seek(from);
			it->Next();
		} else {
			it->SeekToFirst();
		}
		for ( int count = 0; it->Valid() && count<1000; count++, it->Next() ) {
			push_pair(data, it->key().ToString(), it->value().ToString());
		}

		typename valmap2str_json::serializer s;
		std::string buf;
		s(data,std::back_inserter(buf));
		if (REPLICATION_DEBUG>1) {
			std::cout << "next portion: " << buf << "\n";
		}
		conn.get_aspect().template get< methods::_set_>().request(conn,buf);

		std::string key;
		if (data.size()>0) {
			key = data.back().first;
		} else {
			key = from;
			state = sync_finishing;
		}
		current_keys.push_back(key);
		return key;
	}

	/**
	 * заполняет очередь отправки
	 */
	template<typename connection>
	bool _push_queue_items(connection &conn) {
		if (queue.empty()) {
			if (queue_pipelined!=0) throw std::logic_error("fail: Y R U PIPELINED?");
			_schedule_queue(conn);
			return false;
		}
		if ((int)queue.size() <= queue_pipelined) return false;
		if (queue_pipelined >= pipeline_size) return false;

		auto it = queue.begin();
		for (int i=0;i<queue_pipelined;i++) it ++; //it+= queue_pipelined;
		for (;queue_pipelined<pipeline_size&&it!=queue.end();queue_pipelined++,it++) {

			std::pair<std::string,std::string> &data = *it;
			if (REPLICATION_DEBUG>0) {
				std::cout << "sending: " << data.first << ": " << data.second << "\n\n";
			}

			if (data.first == "del") {
				conn.get_aspect().template get< methods::_del_>().request(conn,data.second);
			} else if (data.first == "set") {
				conn.get_aspect().template get< methods::_set_>().request(conn,data.second);
			} else if (data.first == "set_range") {
				conn.get_aspect().template get< methods::_set_range_>().request(conn,data.second);
			} else throw new std::logic_error(data.first);
			//send_queue_item(conn, it); TODO
		}
		return true;
	}

	/**
	 * делает запросы, отправляет данные первичного заполнения
	 */
	template<typename connection>
	void sync_initial(connection &conn) {
		std::lock_guard<spinlock> lck(mtx);
		std::string key = "";
		for (int i=0;i<pipeline_size;i++) key = _push_next_portion(conn,key);
		state = syncing;
	}

	/**
	 * первичное заполнение окончено, отправляет апдейт-очередь
	 */
	template<typename connection>
	void _sync_queue(connection &conn) {
		state = running;
		queue_pipelined = 0;
		_push_queue_items(conn);
/*
		std::lock_guard<spinlock> lck(mtx);
		if (queue.empty()) return;
		std::string str = queue.front();
		if (t.send(str)) {
			//TODO: сделать pop только при получении подтверждения
			queue.pop_front();
		} else if (queue.size()>100000) {
			//TODO: файловая буферизация и ещё одно состояние
			state = not_ready;
			t.disconnect();
			return;
		}*/
	}

	template<typename connection>
	void sync_queue(connection &conn) {
		std::lock_guard<spinlock> lck(mtx);
		_sync_queue(conn);
	}
};

struct Replication {
	std::unordered_map<std::string, Client_t*> repClients;

	template<typename connection>
	void add_client(std::string id, std::string state, connection &conn) {
		if (repClients.count(id)) {
			if (state == "stop") {
				delete repClients[id];
				repClients.erase(id);
				return;
			} else if (state == "start") {
				delete repClients[id];
				repClients.erase(id);
				//не выходим - добавляем заново
			}
		} else {
			//пользователя ещё нет. если режим - авто, то пусть перекачает всё заново
			if (state == "auto") state = "start";
		}
		if (state == "stop") return;

		if (!repClients.count(id)) {
			Client_t *cl = new Client_t();
			cl->id = id;
			repClients[id] = cl;
		}
		if (state == "start") {
			repClients[id]->state = not_ready;
			repClients[id]->sync_initial(conn);
		} else /*if (state == auto or continue)*/ {
			repClients[id]->state = running;
			repClients[id]->sync_queue(conn);
		}
	}


	template<typename T, typename CB>
	void _send_to_clients(const std::string str, const typename T::target &data, const CB &callback) {
		if (!data.size()) return;
		for (auto itc = repClients.begin();itc!=repClients.end();itc++) {
			auto &client = itc->second;

			if (client->state == running) {
				client->send<T>(str, data);
			} else if (client->state == syncing) {
				typename T::target partial;
				callback(partial, data, client->current());
				if (partial.size()>0) client->send<T>(str, partial);
			}
		}
	}



	void set(const valmap2str &data) {
		_send_to_clients<valmap2str_json>("set", data, [](valmap2str &partial, const valmap2str &data, const std::string &key){
			for (auto it = data.begin();it!=data.end();it++) if (it->first < key) {
				push_pair(partial, it->first, it->second);
			}
		});
	}
	void set_range(const valmap2str &data) {
		_send_to_clients<valmap2str_json>("set_range", data, [](valmap2str &partial, const valmap2str &data, const std::string &key){
			for (auto it = data.begin();it!=data.end();it++) if (it->first < key) {
				push_pair(partial, it->first, it->second);
			}
		});
	}
	void del(const vallist &data) {
		_send_to_clients<vallist_json>("del", data, [](vallist &partial, const vallist &data, const std::string &key){
			for (auto it = data.begin();it!=data.end();it++) if (*it< key) {
				partial.push_back(*it);
			}
		});
	}


};
Replication replication;
