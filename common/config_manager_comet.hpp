#pragma once

#include "common/config_manager.hpp"


#include "../mamba_comet/comet/gateway/json/channel_client_impl.hpp"
#include "../mamba_comet/comet/isubscriber.hpp"
#include "../mamba_comet/comet/ipubsub_callback.hpp"
#include <fas/mux/epoller.hpp>
#include <fas/mux/best_mux.hpp>
#include <iostream>

struct CometConfig : IConfig {
	std::string host;
	in_port_t port;
	std::string channel;

	virtual void addToOptions(po::options_description &options, std::string section, std::string prefix) {
		po::options_description desc(section);
		desc.add_options()
			((prefix+"host").c_str(), po::value<std::string>(&host)->default_value(std::string("127.0.0.1")),"host")
			((prefix+"port").c_str(), po::value<in_port_t>(&port)->default_value(20100), "port")
			((prefix+"channel").c_str(), po::value<std::string>(&channel)->default_value(""),"channel (required)")
		;
		options.add(desc);
	}
	bool operator==(const CometConfig &other) const {
		return other.host == host && other.port == port && other.channel == channel;
	}
	bool operator!=(const CometConfig &other) const {
		return !(*this == other);
	}
};

class ConfigManagerComet : public ConfigManager,public ::mamba::comet::isubscriber, public ::mamba::comet::ipubsub_callback {
	ConfigHandle<CometConfig> cfg;
	CometConfig cfgCopy;
	std::mutex loading;
public:

	void updateVariables(std::string data) {
		std::for_each(mtxs.begin(),mtxs.end(),[](std::mutex *mtx){ mtx->lock();});
	    po::variables_map vm;
	    try {
	    	std::istringstream str(data);
	    	po::store(parse_config_file(str, options) , vm);
	    	//argslist.push_front( parse_config_file(str, options) );
	    	for (auto it=argslist.begin();it!=argslist.end();++it) {
	    		po::store(*it, vm);
	    	}
			po::notify(vm);
	    } catch(std::exception &e) {
			std::cout << e.what() << std::endl;
	    }
		std::for_each(mtxs.begin(),mtxs.end(),[](std::mutex *mtx){ mtx->unlock();});

	    std::cout << vm << std::endl;

	    this->reconnectComet();
	}

	virtual void publish(const ::mamba::comet::message& m) {
		//std::cout << "publish(const message& m)" << std::endl;
		//std::cout << "content: " << std::string( m.content.begin(), m.content.end() ) << std::endl;

		fas::json::value<std::string>::serializer s;
		std::string str;
		s(str,m.content.begin(), m.content.end());
		std::cout << "publish recieved:\n" << str << std::endl;
		updateVariables(str);
		//fas::json::value<std::string> str;
	}
	virtual void publish(const std::vector< ::mamba::comet::message > & m) {
		for (auto p = m.begin();p!=m.end();p++) publish(*p);
	}
	virtual void timeout() { }
	virtual void die() { }
	virtual void bad_gateway() { }
	virtual void published( ::mamba::comet::status::type s, const std::string& channel ) { }
	virtual void published( const std::vector< std::pair< ::mamba::comet::status::type, std::string> >& result ) { }
	virtual void subscribed( ::mamba::comet::status::type s, const std::string& channel ) { }
	virtual void described( ::mamba::comet::status::type s, const std::string& channel ) { }
	virtual void loaded( const std::vector< ::mamba::comet::load_response>& cr ) { }
	virtual void loaded( ::mamba::comet::status::type s, const std::string& channel, const std::vector< ::mamba::comet::message_base>& m ) {
		std::cout << "virtual void loaded( status::type s, const std::string& channel, const std::vector<message_base>& m )   " << std::endl;
		if ( !m.empty() ) {
			fas::json::value<std::string>::serializer s;
			std::string str;
			s(str,m[0].content.begin(), m[0].content.end());
			std::cout << "loaded:\n" << str << std::endl;
			updateVariables(str);
		} else {
			std::cout << s << std::endl;
		}
		loading.unlock();
	};

public:
	ConfigManagerComet(po::options_description &opt) : ConfigManager(opt),ccs(NULL),ccs_mux(NULL) {
		cfg = this->add<CometConfig>("Comet dynamic configuration daemon","cometcfg.");
	}

	void load() {
		if (!cfg->host.length() || !cfg->channel.length()) return;
		::mamba::comet::gateway::json::channel_client_impl cc;
		::fas::mux::best_mux mux;
		cc.set_mux(&mux);
		cc.set_nonblock(true);
		cc.connect(cfg->host,cfg->port);

		// virtual status::type load(const load_request& lc, std::vector<message_base>& m, ipubsub_callback* callback ) = 0;
		// load_request(const std::string& channel, const cursor_t& cursor, size_t limit)
		std::vector< ::mamba::comet::message_base > m;
		auto status = cc.load( ::mamba::comet::load_request(cfg->channel, 0, 1), m, this);
		if (status == mamba::comet::status::wait) {
			loading.lock();
			while (!loading.try_lock()) {
				mux.select(1);
				cc.idle(cc);
			}
			loading.unlock();
		}
	}
private:
	::mamba::comet::gateway::json::channel_client_impl* ccs;
	spinlock lck_client;
	int counter;
	::fas::mux::best_mux *ccs_mux;

	void reconnectComet() {
		std::lock_guard<spinlock> lck(lck_client);
		if (ccs_mux && ccs && cfg.get() != cfgCopy) {
			ccs->close(true);
			delete ccs;
			subscribe(ccs_mux);
		}

	}

public:
	void subscribe(::fas::mux::best_mux* mux) {
		if (!cfg->host.length() || !cfg->channel.length()) return;
		std::lock_guard<spinlock> lck(lck_client);
		ccs = new ::mamba::comet::gateway::json::channel_client_impl();
		ccs->set_mux(mux);
		ccs->set_nonblock(true);
		ccs->connect(cfg->host,cfg->port);
		ccs->subscribe(cfg->channel, this, this);
		counter = 0;

		cfgCopy = cfg.get();
		ccs_mux = mux;
	}

	void idle(int divisor = 1) {
		counter++;
		if ((counter%divisor) != 0) return;
		counter = 0;
		if (!cfg->host.length() || !cfg->channel.length()) return;
		std::lock_guard<spinlock> lck(lck_client);
		ccs->idle(*ccs);
	}
};
