/*
 * misc.hpp
 *
 *  Created on: Jul 7, 2011
 *      Author: shepik
 */

#ifndef DAEMON_MISC_HPP_
#define DAEMON_MISC_HPP_

#include "common/mymysql.hpp"
#include <boost/program_options.hpp>
namespace po = ::boost::program_options;
#include <netinet/in.h>
namespace db = ::mamba::gateway::db;

namespace misc {
struct mysql_options {
	std::string host;
	in_port_t port;
	std::string user;
	std::string pass;
	std::string name;

	void addToOptions(po::options_description &options, std::string section, std::string prefix, std::string default_host, std::string default_name, bool use_root = false) {
		po::options_description desc(section);
		desc.add_options()
			((prefix+"host").c_str(), po::value<std::string>(&host)->default_value(default_host),"Mysql host")
			((prefix+"port").c_str(), po::value<in_port_t>(&port)->default_value(3306), "Mysql port")
			((prefix+"user").c_str(), po::value<std::string>(&user)->default_value(use_root?"root":"monamour2"),"Mysql user")
			((prefix+"pass").c_str(), po::value<std::string>(&pass)->default_value(use_root?"":"monamourchik"),"Mysql pass")
			((prefix+"name").c_str(), po::value<std::string>(&name)->default_value(default_name),"Mysql database name")
		;
		options.add(desc);
	}
	db::mysql_connection *connect() const {
		db::mysql_connection *conn = new db::mysql_connection();
		conn->connect(host,user,pass,name,port);
		return conn;
	}
	void connect(db::mysql_connection &conn) const {
		conn.connect(host,user,pass,name,port);
	}
};

struct json_options {
	std::string host;
	in_port_t port;
	int timeout;

	void addToOptions(po::options_description &options, std::string section, std::string prefix, std::string default_host, int default_port, bool add_timeout, int default_timeout = 0) {
		po::options_description desc(section);
		desc.add_options()
			((prefix+"host").c_str(), po::value<std::string>(&host)->default_value(default_host),"Host")
			((prefix+"port").c_str(), po::value<in_port_t>(&port)->default_value(default_port), "Port")
		;
		if (add_timeout) {
			desc.add_options() ((prefix+"timeout").c_str(), po::value<int>(&timeout)->default_value(default_timeout),"Timeout") ;
		}
		options.add(desc);
	}

};

}


#endif /* MISC_HPP_ */
