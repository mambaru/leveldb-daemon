#pragma once

namespace common{ namespace multiserver { namespace hlp {

template<typename T> void run_select(T &mux) {
	try {
		mux.select(50);
	} catch(std::exception& e) {
		log_write ("std::exception:\n" << e.what());
	} catch(...) {
		log_write ("unhandled exception");
	}
}


template<typename MAP> void fill_map(MAP &map, ap::empty_type, int ) {
}
template<typename MAP, typename L> void fill_map(MAP &map, L l, int v = 0) {
	typedef typename L::left_type current_type;
	current_type obj;
	map[obj.name()] = v;
	fill_map(map, typename L::right_type(),v+1);
}

template<typename method_class,typename T, bool = ajr::has_invoke_request<method_class>::result && ajr::has_invoke_response<method_class>::result>
struct invoke_requester {
	static void execute(T&t, method_class &mc, int id, std::string& params) {
		typedef ajr::invoker<method_class> invoker;
		typedef typename invoker::invoke_request invoke_request;
		typedef typename invoker::invoke_response invoke_response;
		typedef typename invoke_request::serializer request_serializer;
		typedef typename invoke_response::serializer response_serializer;

		typename invoke_request::target req;
		typename invoker::invoke_response_type res;

		try {
			request_serializer()(req,params.c_str(),params.c_str()+params.size());
		} catch (...) {
			t.get_aspect().template get<ajr::_invalid_json_>()(t, params.c_str(),params.size());
			return;
		}
		if (mc.request(t, req, id, res)) {
			invoker(mc).response(t, res, id);
		}
	};
};
template<typename method_class,typename T>
struct invoke_requester<method_class,T,false> {
	static void execute(T&t, method_class &m, int id, std::string& params) {
	}
};


template<typename method_class,typename T, bool = ajr::has_invoke_notify<method_class>::result>
struct invoke_notifier {
	static void execute(T&t, method_class &mc, std::string& params) {
		typedef ajr::notifier<method_class> invoker;
		typedef typename invoker::invoke_notify invoke_request;
		typedef typename invoke_request::serializer request_serializer;

		typename invoke_request::target req;

		try {
			request_serializer()(req,params.c_str(),params.c_str()+params.size());
		} catch (...) {
			t.get_aspect().template get<ajr::_invalid_json_>()(t, params.c_str(),params.size());
			return;
		}
		mc.notify(t, req);
	};
};
template<typename method_class,typename T>
struct invoke_notifier<method_class,T,false> {
	static void execute(T&t, method_class &m, std::string& params) {
	}
};


template<typename method_class,typename T, bool = ajr::has_call_request<method_class>::result && ajr::has_call_response<method_class>::result>
struct invoke_caller {
	template<typename M>
	static void execute(T&t, M &m, int id, std::string& result) {
		typedef ajr::caller<method_class> caller;
		typedef typename caller::call_response call_response;
		typedef typename call_response::serializer response_serializer;

		typename caller::call_response_type res;

		try {
			response_serializer()(res,result.c_str(),result.c_str()+result.size());
		} catch (...) {
			t.get_aspect().template get<ajr::_invalid_json_>()(t, result.c_str(),result.size());
			return;
		}
		//caller &c = static_cast<caller&>(m.get_class());
		caller &c = (caller&)m.get_class();
		if (c.remove_id(id)) {
			method_class &mc = m.get_class();
	    	mc.response(t, res, id);
		} else {
	    	//t.get_aspect().template get<_invalid_id_>()(t,result.c_str(),result.c_str()+result.size());
	    }
	}
};
template<typename method_class,typename T>
struct invoke_caller<method_class,T,false> {
	static void execute(T&t, method_class &m, int id, std::string& result) {
	}
};


template<typename T> static void run_method_request(int num, T& t, ap::empty_type , json_rpc_req* req) {
	//error!!!
	t.get_aspect().template get<ajr::_unknown_method_>()(t, req->params.c_str(), req->params.size() );
}
template<typename T, typename L> static void run_method_request(int num, T& t, L , json_rpc_req* req) {
	if (num) { run_method_request(num-1, t, typename L::right_type(), req); return; }
	typedef typename L::left_type::advice_class::method_class current_type;
	current_type& current = t.get_aspect().template get<typename L::left_type>().get_class();
	if (req->id>=0) {
		invoke_requester<current_type, T>::execute(t, current, req->id, req->params);
	} else {
		invoke_notifier<current_type, T>::execute(t, current, req->params);
	}
}
template<typename T> static void run_method_call(T& t, ap::empty_type , json_rpc_req* req) {
	t.get_aspect().template get<ajr::_lost_result_>()(t, req->params.c_str(), req->params.size() );
}
template<typename T, typename L> static void run_method_call(T& t, L , json_rpc_req* req) {
	typedef typename L::left_type current_type;
	current_type& current = t.get_aspect().template get<typename L::left_type>();
	if (current.has_id(req->id)) {
		std::cerr << "invoke_caller is not supported\n";
		//invoke_caller<typename current_type::advice_class::method_class, T>::execute(t, current, req->id, req->result);
	} else {
		run_method_call(t, typename L::right_type(), req);
	}
}


}}}
