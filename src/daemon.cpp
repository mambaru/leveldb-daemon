#define LOG_VERBOSITY 0

#include <boost/program_options.hpp>
namespace po = ::boost::program_options;
#include <iostream>
#include <fstream>
#include <string>
#include <mutex>
#include <list>
#include <thread>

#include <fas/mux/best_mux.hpp>
#include <fas/mux/imux.hpp>
#include <fas/inet/server.hpp>
#include <fas/inet/client.hpp>

#include <fas/adv/json_rpc3/method.hpp>

#include "common/params.h"
#include "common/lock_atomic.hpp"
#include <fas/mux/epoller.hpp>

#include "common/misc.hpp"

namespace as = ::fas::system;
namespace asi = ::fas::system::inet;
namespace af = ::fas::filter;
namespace ai = ::fas::inet;
namespace am = ::fas::mux;
namespace aa = ::fas::aop;
namespace ap = ::fas::pattern;
namespace ad = ::fas::adv;
namespace ajr = ::fas::adv::json_rpc3;
namespace aj = ::fas::json;

#include <string>
#include <unordered_map>
#include <vector>
#include <deque>

#include <leveldb/db.h>
#include <leveldb/filter_policy.h>
#include <leveldb/cache.h>

#include <tbb/concurrent_queue.h>
#include "common/udp.hpp"
#include "fas_queries.hpp"

#include "db.hpp"
#include "replication.hpp"

#include "fas_methods.hpp"


template<typename T> void run_select(T &mux) {
	try {
		mux.select(50);
	} catch(std::exception& e) {
		log_write ("std::exception:\n" << e.what());
	} catch(...) {
		log_write ("unhandled exception");
	}
}

struct client_tcp: public ai::mux_client< methods::json_aspect_client > {
};
#include <fas/inet/mt_server.hpp>
#include "common/multiserver.hpp"

struct rserver_tcp: public ai::server< methods::json_aspect > {

	template<typename T>
	void initobj(T &t, ::fas_stats::common_stat_mtx *stat) {
		t.get<ad::_span_reg_>().stat_object(stat);
		t.get<fas_stats::_get_statistics_>().stat_count = 1;
		t.get<fas_stats::_get_statistics_>().stat = stat;
	}

	void init(::fas_stats::common_stat_mtx *stat) {
		this->set_nonblock(true);
		initobj(this->get_prototype().get_aspect(), stat);
	}
};
struct server_tcp: public common::multiserver::multiserver< methods::json_aspect > {
	void init(int threads) {
		this->set_nonblock(true);
		int n0 = threads>=8?2:1;
		int n1 = std::max(1,(threads-n0)/2);
		int n2 = std::max(1,threads-n0-n1);
		this->set_max_threads(n0);
		this->method2thread["set"] = 1;
		this->method2thread["set_range"] = 1;
		this->method2thread["inc_add"] = 1;
		this->method2thread["inc"] = 1;
		this->method2thread["update_packed"] = 1;
		this->thread_count = {n2,n1};
	}
};


/*
struct server_tcp: public ai::server< methods::json_aspect > {
	template<typename T>
	void initobj(T &t, ::fas_stats::common_stat_mtx *stat) {
		t.get<ad::_span_reg_>().stat_object(stat);
		t.get<fas_stats::_get_statistics_>().stat_count = 1;
		t.get<fas_stats::_get_statistics_>().stat = stat;
	}

	void init(::fas_stats::common_stat_mtx *stat) {
		this->set_nonblock(true);
		initobj(get_prototype().get_aspect(), stat);
	}
};
*/
struct server_udp: public ::common::server_udp< ai::mux_connection_base, methods::json_aspect > {
	template<typename T>
	void initobj(T &t, ::fas_stats::common_stat_mtx *stat) {
		t.get<ad::_span_reg_>().stat_object(stat);
		t.get<fas_stats::_get_statistics_>().stat_count = 1;
		t.get<fas_stats::_get_statistics_>().stat = stat;
	}
	void init(::fas_stats::common_stat_mtx *stat) {
		this->set_nonblock(true);
		initobj(get_aspect(), stat);
	}
};

struct ReplicationConfig {
    std::string id;
    std::string host;
    in_port_t port;

    void addToOptions(po::options_description &options, std::string section, std::string prefix) {
        po::options_description desc(section);
        desc.add_options()
			((prefix+"id").c_str(), po::value<std::string>(&id)->default_value(""),"Replication id")
			((prefix+"host").c_str(), po::value<std::string>(&host)->default_value("127.0.0.1"),"Replication master host")
            ((prefix+"port").c_str(), po::value<in_port_t>(&port)->default_value(22501), "Replication master port")
        ;
        options.add(desc);
    }

};


int main (int argc, char **argv) {

	::fas_stats::common_stat_mtx stat;

	int port,rport;
	std::string config_file,user,log_file,db_dir;
	bool daemonize;
	int thread_num_tcp,thread_num_udp;
	int memory;
	int buffer;

	po::options_description options;
	po::options_description gopt("Generic options");
	gopt.add_options()
		("help,h", "show help message" )
		("config,C", po::value<std::string>(&config_file),"load configuration file" )
		("log_file", po::value<std::string>(&log_file),"log file" )
		("db", po::value<std::string>(&db_dir)->default_value("testdb"),"database directory name" )
		("port", po::value< int >(&port)->default_value(22500), "port for general use" )
		("rport", po::value< int >(&rport)->default_value(22501), "port for replication slaves to connect" )
		("daemonize,d", po::value< bool >(&daemonize)->default_value(false) ,"run as a daemon")
		("user", po::value< std::string>(&user)->default_value("") )
		("threads_tcp", po::value< int >(&thread_num_tcp)->default_value(1), "tcp threads count" )
		("threads_udp", po::value< int >(&thread_num_udp)->default_value(1), "udp threads count" )
		("memory", po::value< int >(&memory)->default_value(40*1024), "in megabytes" )
		("buffer", po::value< int >(&buffer)->default_value(512), "in megabytes" )
	;
	options.add(gopt);

	ReplicationConfig repl;
	repl.addToOptions(options, "Replication configuration","replication.");

    po::variables_map vm;
    try {
        po::store(po::command_line_parser(argc, argv).options(options).run(), vm);
        po::notify(vm);
        if (config_file.length()) {
            std::ifstream ifs(config_file.c_str());
            po::store(parse_config_file(ifs, options), vm);
            po::notify(vm);
        }
    } catch(std::exception &e) {
        std::cout << e.what() << std::endl;
        return 1;
    }

    if (vm.count("help")) {
        std::cout << options << std::endl;
        return 0;
    }

    std::cout << vm << std::endl;;

    if (log_file.length()) {
		common::redirect_output(log_file);
        std::cout << vm << std::endl;
    }
    if (user.length()) common::change_user(user);

    if (daemonize) {
        common::daemonize(log_file.length()?true:false,log_file.length()?true:false);
    }

    signal(SIGPIPE,  SIG_IGN);
    signal(SIGPOLL,  SIG_IGN);


	db_global = create_db( db_dir,buffer, memory);

	std::vector<std::unique_ptr<std::thread> > threads;
	is_running = 1;

	threads.push_back(std::unique_ptr<std::thread>(new std::thread([&stat,thread_num_tcp,port,&repl]{
		client_tcp client;
		server_tcp json_tcp_srv;
		fas::mux::best_mux mux;
		json_tcp_srv.set_mux(&mux);
		json_tcp_srv.init(thread_num_tcp);
		json_tcp_srv.start( port );

		auto client_connect = [&mux,repl,&client]() {
			client.close();
			client.set_mux(&mux);
			client.connect(repl.host,repl.port);
			client.set_nonblock(true);
			fas::system::inet::keepalive(client.get_socket(),true);
			client.get_aspect().get< methods::_rep_setstate_>().request(client,request::rep_setstate(repl.id,"continue"));
		};
		if (repl.id.length()) {
			try {  client_connect();} catch (std::exception &ex) { std::cerr << ex.what() << std::endl; } catch (...){}
		}

		while (is_running) {
			run_select(mux);
			if (repl.id.length() && !client.get_status()) {
				try {
					client_connect();
				} catch(...) { }
			}
		}
		json_tcp_srv.stop();
	})));

	server_udp json_udp_srv;
	fas::mux::best_mux mux;
	json_udp_srv.set_mux(&mux);
	json_udp_srv.init( &stat);
	json_udp_srv.start( port );

	rserver_tcp json_tcp_srv;
	json_tcp_srv.set_mux(&mux);
	json_tcp_srv.init(&stat);
	json_tcp_srv.start( rport );

	for (int i=0;i<thread_num_udp-1;i++) {
		threads.push_back(std::unique_ptr<std::thread>(new std::thread([&json_udp_srv,&stat]{
			server_udp udp;
			fas::mux::best_mux mux;
			udp.set_mux(&mux);
			udp.init( &stat);
			udp.start_on_same_socket( json_udp_srv );

			while (is_running) run_select(mux);
		})));
	}

	while (is_running) run_select(mux);

	for (int i=0;i<(int)threads.size();i++) threads[i]->join();
}

