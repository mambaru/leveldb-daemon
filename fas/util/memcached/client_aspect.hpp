//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_UTIL_MEMCACHED_CLIENT_ASPECT_HPP
#define FAS_UTIL_MEMCACHED_CLIENT_ASPECT_HPP

#include <fas/aop/advice.hpp>
#include <fas/aop/aspect.hpp>
#include <fas/pattern/type_list.hpp>
#include <fas/adv/ad_stub.hpp>
#include <fas/inet/client.hpp>
#include <fas/util/memcached/ad_client_parser.hpp>
#include <fas/util/memcached/state_get.hpp>
#include <fas/util/memcached/state_storage.hpp>
#include <fas/util/memcached/state_ready.hpp>
#include <fas/util/memcached/tags.hpp>


namespace fas{ namespace util { namespace memcached{

namespace aa = ::fas::aop;
namespace ad = ::fas::adv;
namespace ap = ::fas::pattern;
namespace af = ::fas::filter;
namespace ai = ::fas::inet;

struct client_stub_list: ap::type_list_n<
  /*_set_, _add_, _replace_,*/ _get_, _storage_
>::type{};

struct client_stub_advice:
  aa::advice< aa::tag_list_n< aa::tag<_get_>, aa::tag<_storage_> >::type, ad::ad_stub<bool> > {};


struct client_state_list: ap::type_list_n<
  /*state_command_get*/
  state_ready<mode_client>, state_storage<mode_client>, state_get<mode_client>
>::type{};

struct client_parser_advice:
  aa::advice< aa::tag<_parser_>, ad_client_parser<client_state_list> > {};

struct client_advice_list: ap::type_list_n<
  client_stub_advice,
  client_parser_advice,
  aa::alias<_parser_, af::_on_read_>,
  aa::alias<af::_writer_, _writer_>
  
>::type {};

struct client_aspect: aa::aspect<client_advice_list>{};

template<typename A = aa::aspect<> >
struct client
  : ai::mux_client< typename aa::aspect_merge< client_aspect, A >::type >
{
  void set(const std::string& key, const std::string& value, unsigned short flags = 0 , int exptime = 0 )
  {
    storage stg;
    stg.key = key;
    stg.data.assign(value.begin(), value.end());
    stg.flags = flags;
    stg.exptime = exptime;
    this->get_aspect().template get<_parser_>().command_storage(*this, stg); 
  }

  void get(const std::string& key)
  {
    this->get_aspect().template get<_parser_>().command_get(*this, key);
  }
};

template<typename A = aa::aspect<> >
class client_base
  : public ai::mux_client_base< typename aa::aspect_merge< client_aspect, A >::type >
{
protected:
  template<typename T>
  void _set(T& t, const std::string& key, const std::string& value, 
                  unsigned short flags = 0 , int exptime = 0 )
  {
    storage stg;
    stg.key = key;
    stg.data.assign(value.begin(), value.end());
    stg.flags = flags;
    stg.exptime = exptime;
    this->get_aspect().template get<_parser_>().command_storage(*this, stg);
  }

  template<typename T>
  void _get(T& t, const std::string& key)
  {
    this->get_aspect().template get<_parser_>().command_get(*this, key);
  }
};

}}}

#endif
