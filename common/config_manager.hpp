#pragma once

#include <boost/program_options.hpp>
namespace po = ::boost::program_options;

#include "common/params.h"
#include "common/lock_atomic.hpp"
#include <mutex>
#include <list>

struct IConfig {
	void addToOptions(po::options_description &options, std::string section, std::string prefix);	//just abstract
};

template<typename T>
class ConfigHandle {
private:
	T* ptr;
	std::mutex *mtx;
public:
	ConfigHandle() : ptr(0), mtx(0){}
	ConfigHandle(T *val, std::mutex *mtx) : ptr(val), mtx(mtx){
	}
	const T get() {
		std::lock_guard<std::mutex> lck(*mtx);
		return *ptr;
	}
	T* getUnsafe() {
		return ptr;
	}
	std::mutex& getMutex() {
		return *mtx;
	}
	const T* operator->() {
		return ptr;
	}
};

class ConfigManager {
protected:
	std::vector<IConfig*> items;
	std::vector<std::mutex*> mtxs;
public:

	po::options_description &options;
	typedef char charT;
	typedef po::basic_parsed_options<charT> parsed_options;
	std::list<parsed_options> argslist;

public:
	ConfigManager(po::options_description &options) : options(options) {
		this->options.add_options()
			("help,h", "show help message" )
			("config,C", po::value<std::string>()->default_value(""),"load configuration file" )
		;
	}

	int initialize_vm(po::variables_map &vm, int argc,char **argv) {
	    try {
	    	argslist.push_back(po::command_line_parser(argc, argv).options(options).run());
			po::store(argslist.back(), vm);
			po::notify(vm);
			std::string config_file = vm["config"].as<std::string>();
			if (config_file.length()) {
				std::ifstream ifs(config_file.c_str());
				argslist.push_back(parse_config_file(ifs, options));
				po::store(argslist.back(), vm);
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

	    std::cout << vm << std::endl;
	    return -1;
	}

	template<typename T>
	ConfigHandle<T> add(const std::string& section, const std::string &prefix) {
		T* item = new T();
		std::mutex *mtx = new std::mutex();
		item->addToOptions(options, section, prefix);
		items.push_back(item);
		mtxs.push_back(mtx);
		return ConfigHandle<T>(item,mtx);
	}

	void load() {
	}

	template<typename T>
	void subscribe(T* mux) {
	}

	void idle(int t = 1) {
	}
};
