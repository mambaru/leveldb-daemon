#pragma once
#include <fas/inet/mt_server.hpp>

namespace common{ namespace multiserver {

struct json_rpc_req;

struct connection_interface
{
	virtual void write(const char *data, size_t sz)=0;
	virtual bool get_status() const = 0;
	virtual bool release(bool lck = false) = 0;
	virtual void delete_request(json_rpc_req*) = 0;
};

struct json_rpc_req {
	char jsonrpc[8];
	std::string method;
	long long int id;
	std::string params;
	std::string result;
	spinlock mtx;
//	long long int clid;

	connection_interface* client;
  
	void clean(bool locked = true)
  {
    //обязательно сделать self unlock; вызывать только из бэк-тредов
		if (client)
      client->delete_request(this);
    
		client = 0;
    
		if (locked)
      mtx.unlock();
    
	}
	
	json_rpc_req(const json_rpc_req&other)
    : method(other.method)
    , id(other.id)
    , params(other.params)
    , result(other.result)
    , client(0)
  {
		memcpy(&jsonrpc[0],&other.jsonrpc[0],8);
	}
	
	void operator=(const json_rpc_req&other)
  {
		method = other.method;
		id=other.id;
		params=other.params;
		result=other.result;
		memcpy(&jsonrpc[0],&other.jsonrpc[0],8);
	}
	
	json_rpc_req()
    : id(-1)
  {}
  
	~json_rpc_req()
  {
	}
	
};

}}

#include "common/multiserver_hlp.hpp"

#ifndef multiserver_NAME
#define multiserver_NAME(X) struct n_##X { const char* operator()() const{ return #X;} };
#endif

namespace common{ namespace multiserver {

multiserver_NAME(id)
multiserver_NAME(jsonrpc)
multiserver_NAME(method)
multiserver_NAME(params)
multiserver_NAME(result)

typedef aj::object<
	json_rpc_req,
	ap::type_list_v<
		aj::member<n_jsonrpc, json_rpc_req, char[8], &json_rpc_req::jsonrpc>
		,aj::member<n_method, json_rpc_req, std::string, &json_rpc_req::method>
		,aj::member<n_id, json_rpc_req, long long int, &json_rpc_req::id>
		,aj::member<n_params, json_rpc_req, std::string, &json_rpc_req::params, aj::raw_value<std::string> >
		,aj::member<n_result, json_rpc_req, std::string, &json_rpc_req::result, aj::raw_value<std::string> >
	>::type
> json_rpc_req_json;

struct multiserver_iface {
	virtual void push_queue(json_rpc_req *req) = 0;
};

struct ad_invoke_first
{
	std::set<json_rpc_req*> requests;
	spinlock mtx;
	multiserver_iface* server;

	ad_invoke_first()
    : server(0)
  {}
  
	ad_invoke_first(const ad_invoke_first &other)
    : server(other.server)
  {}

	void set_server(multiserver_iface *s)
  {
		this->server = s;
	}

	bool delete_request(json_rpc_req *req)
  {
		std::lock_guard<spinlock> lck(mtx);
    
		if (requests.count(req))
    {
			requests.erase(req);
			//req->clean(false);
			return true;
		}
		else
    {
			std::cout << "error: delete_request failed\n";
			return false;
		}
	}

	template<typename T>
	void clear(T &t)
  {
    //очищает не все requests, а только те, которые можно спокойно очистить
		for ( int i=0; i<3; i++)
    {
			std::lock_guard<spinlock> lck(mtx);
			std::set<json_rpc_req*> rnew;
      
			for (auto it = requests.rbegin(); it!=requests.rend(); it++ )
      {
				json_rpc_req *req = (*it);
        
				if (req->mtx.try_lock())
        {
					req->client = 0;
					req->mtx.unlock();
				}
				else
        {
					//req->client = 0;
					rnew.insert(req);
				}
			}
			requests.swap(rnew);
      
			if ( requests.empty() )
        return;
		}
		
		{
			std::lock_guard<spinlock> lck(mtx);
			std::cout << "clear: warning: " << requests.size() << " reqs left\n";
		}
	}

	template<typename T>
	void operator() (T& t, const char* d, size_t s)
  {
		if (!server)
    {
			std::cerr << "cannot find server\n";
			return;
		}
		
		json_rpc_req *req = new json_rpc_req();
		req->client = &t;
    
		
		try
		{
			json_rpc_req_json::serializer srlzr;
			srlzr(*req, d, d+s);
      
      {
        std::lock_guard<spinlock> lck(mtx);
        requests.insert(req);
      }

			server->push_queue(req);
		}
		catch (...)
    {
			delete req;
			t.get_aspect().template get<ajr::_invalid_json_>()(t, d,s);
      std::cout << "ad_invoke_first: catch(...): " << std::string(d, d+s) << std::endl;
		}
	}

	template<typename T>
	void error(T& t, int id, const char* d, size_t s)
  {
		t.get_aspect().template get<af::_writer_>()(t, d, s );
	}

};

struct invoke_advice: aa::advice<
	aa::tag_list_n<
		aa::gtag<ad::_cleaner_>,
		aa::tag<ajr::_input_>,
		aa::tag<ajr::_invoke_>
	>::type,
	ad_invoke_first
> {};


template<typename A, typename F>
struct multiserver_mux_connection :
  ::ai::mux_connection_base<A,F>, public connection_interface
{
	typedef ::ai::mux_connection_base<A,F> super;

	virtual void write(const char *data, size_t sz)
  {
		//на самом деле что-то писать
		if (super::get_status())
      super::write( data, sz );
	}
	
	virtual bool get_status() const
	{
		return super::get_status();
	}
	
	virtual void delete_request(json_rpc_req *req)
  {
		this->get_aspect().template get<ajr::_input_>().delete_request(req);
	}
};

struct ad_writer {	//на второй половине, в бэкграунде
	connection_interface *client;
	template<typename T>
	void operator()(T& t, const char* d, size_t s) {
		if (client) client->write(d,s);
	}
	void set_client(connection_interface *client) {
		this->client = client;
	}
	bool get_status() const { return client?client->get_status():false; }
	bool release(bool lock) { return client?client->release(lock):true; }
};


template<typename MLA>
struct multiserver
  : public multiserver_iface
{
//	typedef typename ap::select<ajr::_gmethod_, MLA>::type ML;

	typedef ::fas::inet::server<
		::aa::aspect_merge< ::common::aspect::rn_json_aspect, ::aa::aspect< ap::type_list_v<invoke_advice, ai::mt_mutex_advice>::type > >::type
		,::ai::mt_server_aspect
		,false
		,::ai::IPv4
		,::ai::TCP
		,ai::mux_connection<aa::aspect<>, multiserver_mux_connection>
	> fserver;

	typedef typename aa::aspect_merge <
		MLA,
		aa::aspect< ap::type_list_v <
			aa::advice <
				aa::tag<af::_writer_>,
				ad_writer
			>, common::aspect::rn_writer_advice
		>::type >
	>::type invokeMLA;

	typedef typename MLA::template select<ajr::_gmethod_>::type methods;

	struct MLA_class : aa::aspect_class<invokeMLA> {
		bool get_status() const {
			return this->get_aspect().template get<af::_writer_>().get_status();
		}
		virtual bool release(bool lock = false) {
			return this->get_aspect().template get<af::_writer_>().release(lock);
		}

	};
	fserver srvmain;	//основной сервер
	int srvmain_threads;//кол-во основных потоков

	std::map<std::string,int> method2thread;//метод => №пула потоков
	std::vector<int> thread_count;//количество потоков-обработчиков, для каждого пула
	std::vector<tbb::strict_ppl::concurrent_queue<json_rpc_req*> > queues;//очереди - для каждого пула

	std::vector<std::thread*> threads;//spawned threads
	volatile int is_running;

	std::map<std::string,int> method2num;//json methods; autogenerated

	multiserver()
    : srvmain_threads(1)
  {
		//typedef typename ap::select<ajr::_gmethod_, MLA>::type mt;
		hlp::fill_map(method2num, methods());
		is_running = 0;
		srvmain.get_prototype().get_aspect().template get<ajr::_input_>().set_server(this);
	}

	fserver::connection_type& get_prototype() { return srvmain.get_prototype();}
	
	void set_nonblock(bool val) { this->srvmain.set_nonblock(val); }
	
	template<typename T> void set_mux(T* t) { srvmain.set_mux(t); }
	void start(unsigned short port) {
		srvmain.start( port );
		assert(srvmain_threads>0);
		assert(thread_count.size()>0);
		spawn_threads();
	}
	void set_max_threads(int threads) {
		srvmain_threads = threads;
	}

	int get_thread_number(std::string &m)
  {
		auto it = method2thread.find(m);
		if (it == method2thread.end() )
      return 0;
		return it->second;
	}
	
	int get_method_number(std::string &m) {
		auto it = method2num.find(m);
		if (it == method2num.end()) return -1;
		return it->second;
	}

	void push_queue(json_rpc_req *req)
  {
		int num = get_thread_number(req->method);
		//std::cout << "req: " << req->method << "\n";
		queues[num].push(req);
	}

	template<typename T>
	void run_select(T& mux) {
		while (this->is_running) hlp::run_select(mux);
	}

	void stop() {
		this->is_running = 0;
		for (auto it=threads.begin();it!=threads.end();it++) {
			(*it)->join();
			delete (*it);
		}
	}

private:

	void threadproc(int num)
  {
		tbb::strict_ppl::concurrent_queue<json_rpc_req*> &queue = queues[num];
		int time = 1;
		json_rpc_req* req;
		MLA_class ml;
    
		//::fas_stats::common_stat_mtx *stat = srvmain.get_prototype().get_aspect().get<fas_stats::_get_statistics_>().stat;
		//ml.get_aspect().get<ad::_span_reg_>().stat_object(stat);
		//ml.get_aspect().get<fas_stats::_get_statistics_>().stat_count = 1;
		//ml.get_aspect().get<fas_stats::_get_statistics_>().stat = stat;
		while (is_running)
    {
			if (!queue.try_pop(req))
      {
				usleep(time);
				if (time<1000)
          time = time*2+10;
				continue;
			}
			
			time = std::max(1,time/3);

			if (req->client)
      {
				req->mtx.lock();
        
				if (req->client)
        {
					ml.get_aspect().template get<af::_writer_>().set_client( req->client );
          
					if (req->result.length()==0)
          {
						int method_id = req->method.length() ? get_method_number(req->method) : -1;
						hlp::run_method_request(method_id, ml, methods(), req);
					}
					else
          {
						hlp::run_method_call(ml, methods(), req);
					}
					
					req->clean(true);
				}
				else
          req->mtx.unlock();
			}
			delete req;
		}
	}

	void spawn_threads() {
		is_running = 1;
		for (int i=1;i<srvmain_threads;i++) threads.push_back(new std::thread([this]{
			fserver srv;
			fas::mux::best_mux mux;
			srv.set_mux(&mux);
			srv.set_nonblock( true );
			srv.start_on_same_socket( this->srvmain );
			srv.get_prototype().get_aspect().template get<ajr::_input_>().set_server(this);

			while (this->is_running) hlp::run_select(mux);
		}));
		queues.resize(thread_count.size());
		for (int num=0;num<(int)thread_count.size();num++) {
			for (int i=0;i<thread_count[num];i++) threads.push_back(new std::thread([this,num]{
				this->threadproc(num);
			}));
		}
	}

};

}}
