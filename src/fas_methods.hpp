#pragma once

#include <fas/adv/ad_mutex.hpp>
#include <fas/filter/basic_filter.hpp>
namespace af = ::fas::filter;
namespace ad = ::fas::adv;

#include "common/fas_statistics.hpp"

namespace methods {

template<typename T, typename X>
struct method_advice_tag : aa::advice< typename aa::tag_list_n< aa::tag< X >, aa::gtag< ajr::_gmethod_ > >::type, ajr::method<T> > {};

template<typename T>
struct method_advice : aa::advice< aa::tag_list_n< aa::gtag< ajr::_gmethod_ > >::type, ajr::method<T> > {};

template<typename M>
struct method_replicated : M {
	template<typename T> bool request(T& t, const typename M::invoke_request::target& cmd,int id, typename M::invoke_response::target &result) {
		bool res = M::request(t,cmd,id,result);
		return res;
		//send_to_replication<M::invoke_request>(M::name(),cmd);
	}

};

struct rep_status {
	const char *name() const { return "rep_status"; }
	typedef aj::raw_value<std::string> invoke_request;
	typedef aj::raw_value<std::string> invoke_response;

	template<typename T> bool request(T& t, const std::string& cmd,int id, std::string &result) {
		std::stringstream sstr;

		sstr << "{";
		bool first = true;
		for (auto it=replication.repClients.begin();it!=replication.repClients.end();it++) {
			if (!first) sstr<< ",";
			sstr << '"' << it->first << '"' << ":";
			it->second->write_status(sstr);
			first = false;
		}
		sstr << "}";
		result = sstr.str();
		return true;
	}
};

struct rep_setstate {
	const char *name() const { return "rep_setstate"; }
	typedef request::rep_setstate_json invoke_request;
	typedef aj::value<bool> invoke_response;
	typedef invoke_request call_request;
	typedef invoke_response call_response;

	std::string clid;
	template<typename T> void clear(T&) { clid.clear(); }

	template<typename T> bool request(T& t, const request::rep_setstate& cmd,int id, bool &result) {
		clid = cmd.id;
		replication.add_client(cmd.id, cmd.state, t);

		return true;
	}
	template<typename T> bool response(T& t, bool result,int id) {
		return true;
	}

};
struct rep_setstate_advice : aa::advice<
    aa::tag_list_n<
    	aa::tag<_rep_setstate_>
        ,aa::gtag < ad::_cleaner_ >
        ,aa::gtag< ajr::_gmethod_ >
    >::type,
    ajr::method<rep_setstate>
> {};

struct get {
	const char *name() const { return "get"; }
	typedef vallist_json invoke_request;
	typedef valmap2str_json invoke_response;

	template<typename T> bool request(T& t, const vallist& cmd,int id, valmap2str &result) {
		result.reserve(cmd.size());
		for (auto it=cmd.begin();it!=cmd.end();it++) {
			std::string r;
			if (getDb()->Get(leveldb::ReadOptions(),*it,&r).ok()) {
				push_pair(result,*it,r);
			}
		}
		return true;
	}
};

struct get_range {
	const char *name() const { return "get_range"; }
	typedef request::get_range_json invoke_request;
	typedef valmap2str_json invoke_response;

	template<typename T> bool request(T& t, const request::get_range& cmd,int id, valmap2str &result) {
		if (cmd.limit<0) return false;
		if (cmd.skip<0) return false;
		if (cmd.limit + cmd.skip > 11000) return false;

		result.reserve(cmd.limit);

		leveldb::ReadOptions ro;
		ro.fill_cache = false;
		leveldb::Iterator* it = getDb()->NewIterator(ro);
		if (cmd.from.length()) {
			it->Seek(cmd.from);
		} else {
			it->SeekToFirst();
		}
		leveldb::Slice slice_to = cmd.to;
		int total_limit = cmd.limit + cmd.skip;
		for (
			int count = 0;
			it->Valid()
				&& (slice_to.empty() || it->key().compare(slice_to)<=0)
				&& count<total_limit;
			count++, it->Next()
		) {
			if (count>=cmd.skip) push_pair(result, it->key().ToString(), it->value().ToString());
		}
		delete it;
		return true;
	}
};

/**
 * вызывается на слейве репликации.
 * установить новые значения + удалить старые значения, которые были раньше а теперь нет
 */
struct set_range {
	const char *name() const { return "set_range"; }
	typedef valmap2str_json invoke_request;
	typedef aj::value<bool> invoke_response;
	typedef aj::raw_value<std::string> call_request;
	typedef invoke_response call_response;

	template<typename T> bool request(T& t, const valmap2str& cmd,int id, bool &result) {
		bool b = true;
		if (cmd.empty()) return true;

		//leveldb::Iterator* dbit = getDb()->NewIterator(leveldb::ReadOptions());
		//delete dbit;
		//dbit->Seek(cmd.front().first);
		for (auto it=cmd.begin();it!=cmd.end();it++) {
			/*while (dbit->Valid() && dbit->key().compare(it->first)<0) {
				getDb()->Delete(leveldb::WriteOptions(),dbit->key());
				dbit->Next();
			}*/
			if (!getDb()->Put(leveldb::WriteOptions(),it->first,it->second).ok()) b = false;
		}
		result = b;
		replication.set_range(cmd);
		return true;
	}
	template<typename T> bool response(T& t, const bool result,int id) {
		std::string clid = t.get_aspect().template get<_rep_setstate_>().clid;
		if (!replication.repClients.count(clid)) return false;
		replication.repClients[clid]->on_confirm(t);
		return true;
	}
};


struct set {
	const char *name() const { return "set"; }
	typedef valmap2str_json invoke_request;
	typedef aj::value<bool> invoke_response;
	typedef aj::raw_value<std::string> call_request;
	typedef invoke_response call_response;

	template<typename T> bool request(T& t, const valmap2str& cmd,int id, bool &result) {
		bool b = true;
		for (auto it=cmd.begin();it!=cmd.end();it++) {
			if (!getDb()->Put(leveldb::WriteOptions(),it->first,it->second).ok()) b = false;
		}
		result = b;
		replication.set(cmd);
		return true;
	}
	template<typename T> bool response(T& t, const bool result,int id) {
		std::string clid = t.get_aspect().template get<_rep_setstate_>().clid;
		if (!replication.repClients.count(clid)) return false;
		replication.repClients[clid]->on_confirm(t);
		return true;
	}
};

struct del {
	const char *name() const { return "del"; }
	typedef vallist_json invoke_request;
	typedef valmap2bool_json invoke_response;
	typedef aj::raw_value<std::string> call_request;
	typedef invoke_response call_response;

	template<typename T> bool request(T& t, const vallist& cmd,int id, valmap2bool &result) {
		result.reserve(cmd.size());
		for (auto it=cmd.begin();it!=cmd.end();it++) {
			std::string r;
			push_pair(result, *it, getDb()->Delete(leveldb::WriteOptions(),*it).ok());
		}
		replication.del(cmd);
		return true;
	}
	template<typename T> bool response(T& t, const valmap2bool& result,int id) {
		std::string clid = t.get_aspect().template get<_rep_setstate_>().clid;
		if (!replication.repClients.count(clid)) return false;
		replication.repClients[clid]->on_confirm(t);
		return true;
	}
};

struct add {
	const char *name() const { return "add"; }
	typedef valmap2str_json invoke_request;
	typedef valmap2bool_json invoke_response;

	template<typename T> bool request(T& t, const valmap2str& cmd,int id, valmap2bool &result) {
		valmap2str repl;
		result.reserve(cmd.size());
		for (auto it=cmd.begin();it!=cmd.end();it++) {
			std::string r;

			std::lock_guard<longspinlock> lck(getLockmanager()->get(it->first));
//			auto lck = std::lock_guard<longspinlock>(getLockmanager()->get(it->first));
			bool val = false;
			if (!getDb()->Get(leveldb::ReadOptions(),it->first,&r).ok()) {
				if (getDb()->Put(leveldb::WriteOptions(),it->first,it->second).ok()) {
					push_pair(result,it->first,true);
					push_pair(repl,it->first,it->second);
					val = true;
				}
			}
			if (!val) push_pair(result,it->first,false);
		}
		replication.set(repl);
		return true;
	}
};

struct inc {
	const char *name() const { return "inc"; }
	typedef valmap2int_json invoke_request;
	typedef valmap2raw_json invoke_response;

	template<typename T> bool request(T& t, const valmap2int& cmd,int id, valmap2raw &result) {
		result.reserve(cmd.size());
		typename aj::value<long long int>::serializer serializer;

		for (auto it=cmd.begin();it!=cmd.end();it++) {
			std::string r;

			std::lock_guard<longspinlock> lck(getLockmanager()->get(it->first));
//			auto lck = std::lock_guard<longspinlock>(getLockmanager()->get(it->first));
			if (getDb()->Get(leveldb::ReadOptions(),it->first,&r).ok()) {
				long long int val;
				serializer(val, r.begin(), r.end());
				val += it->second;
				r.clear();
				serializer(val,std::back_inserter(r));
				if (getDb()->Put(leveldb::WriteOptions(),it->first,r).ok()) {
					push_pair(result,it->first, r);
				}
			}
		}
		replication.set(result);
		return true;
	}

};
struct inc_add {
	const char *name() const { return "inc_add"; }
	typedef request::inc_add_json invoke_request;
	typedef valmap2raw_json invoke_response;

	template<typename T> bool request(T& t, const request::inc_add& cmd,int id, valmap2raw &result) {
		result.reserve(cmd.inc.size());
		typename aj::value<long long int>::serializer serializer;

		for (auto it=cmd.inc.begin();it!=cmd.inc.end();it++) {
			std::string r;

			std::lock_guard<longspinlock> lck(getLockmanager()->get(it->first));
			long long int val;
			if (getDb()->Get(leveldb::ReadOptions(),it->first,&r).ok()) {
				try {
					serializer(val, r.begin(), r.end());
				} catch (std::exception &e) {
					val = 0;
				}
			} else if (cmd.def.count(it->first)){
				val = cmd.def.at(it->first);
			} else {
				val = 0;
			}
			val += it->second;
			r.clear();
			serializer(val,std::back_inserter(r));
			if (getDb()->Put(leveldb::WriteOptions(),it->first,r).ok()) {
				push_pair(result, it->first, r);
			}
		}
		replication.set(result);
		return true;
	}
};

struct update_packed {
	const char *name() const { return "update_packed"; }
	typedef request::update_packed_json invoke_request;
	typedef aj::raw_value<std::string> invoke_response;

	template<typename T> bool request(T& t, const request::update_packed& cmd,int id, std::string &result) {


		typename aj::value<long long int>::serializer serializer_i;
		typename valmap2str_map_json::serializer serializer_m;

		valmap2str_map data;
		std::lock_guard<longspinlock> lck(getLockmanager()->get(cmd.key));
		{
			std::string tmp;
			if (getDb()->Get(leveldb::ReadOptions(),cmd.key,&tmp).ok()) {
				try {
					serializer_m(data, tmp.begin(), tmp.end());
				} catch (std::exception &e) {
					data = cmd.def;
				}
			} else {
				data = cmd.def;
			}
		}
		for (auto it=cmd.set.begin();it!=cmd.set.end();it++) {
			data[it->first] = it->second;
		}
		for (auto it=cmd.inc.begin();it!=cmd.inc.end();it++) {
			std::string &sval = data[it->first];
			long long int val = 0;
			try {
					if (sval.length()) serializer_i(val, sval.begin(), sval.end());
			} catch (std::exception &e) {}
			val += it->second;
			sval.clear();
			serializer_i(val, std::back_inserter(sval));
		}

		result.clear();
		serializer_m(data, std::back_inserter(result));
		if (getDb()->Put(leveldb::WriteOptions(),cmd.key,result).ok()) {
			valmap2raw tmp;
			push_pair(tmp, cmd.key,result);
			replication.set(tmp);
		} else {
			result = "null";
		}

		return true;
	}
};

struct mutex_advice : aa::advice< af::_mutex_, ad::ad_mutex< ::fas::system::thread::mutex> > { };

typedef ap::type_list_v<
	method_advice<get>
	,method_advice<get_range>
	,method_advice_tag<set,_set_>
	,method_advice_tag<set,_set_range_>
	,method_advice<inc>
	,method_advice<inc_add>
	,method_advice<add>
	,method_advice_tag<del,_del_>
	,method_advice<rep_status>
	,method_advice<update_packed>
	,rep_setstate_advice
	, mutex_advice
	, ::fas_stats::get_statistics_advice< ::fas_stats::get_statistics_helper_null >
	, ::fas_stats::ad_span_reg_mtx_advice
>::type method_list;

typedef aa::aspect<method_list> method_aspect;
typedef aa::aspect_merge< common::aspect::rn_json_aspect, method_aspect>::type json_aspect;




//struct on_connect_advice: aa::advice< aa::tag< ai::_on_client_connect_ >, on_connect_progress > {};
//struct on_connected_advice: aa::advice< aa::tag< ai::_on_client_connected_ >, on_connected > {};

struct on_connect_error {
	template<typename T> void operator()( T& t, const fas::system::inet::socket_error& err) {
		t.close();
	}
};
struct on_connection_close {
	template<typename T> void operator()( T& t) {
		t.close();
	}
};
struct on_connect_error_advice: aa::advice< aa::tag< ai::_on_connect_error_ >, on_connect_error > {};
struct on_connection_close_advice: aa::advice< aa::tag< ai::_on_connection_close_ >, on_connection_close > {};

typedef ap::type_list_v<
	method_advice_tag<set,_set_>
	,method_advice_tag<set,_set_range_>
	,method_advice_tag<del,_del_>
	,rep_setstate_advice
	,on_connection_close_advice
	,on_connect_error_advice
>::type method_list_client;
typedef aa::aspect<method_list_client> method_aspect_client;
typedef aa::aspect_merge< common::aspect::rn_json_aspect, method_aspect_client>::type json_aspect_client;

}
