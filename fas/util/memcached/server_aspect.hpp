//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_UTIL_MEMCACHED_SERVER_ASPECT_HPP
#define FAS_UTIL_MEMCACHED_SERVER_ASPECT_HPP

#include <fas/aop/advice.hpp>
#include <fas/aop/aspect.hpp>
#include <fas/pattern/type_list.hpp>
#include <fas/adv/ad_stub.hpp>
#include <fas/inet/server.hpp>
#include <fas/util/memcached/ad_server_parser.hpp>
#include <fas/util/memcached/state_ready.hpp>
//#include <fas/util/memcached/state_command.hpp>
#include <fas/util/memcached/state_storage.hpp>
#include <fas/util/memcached/state_get.hpp>
#include <fas/util/memcached/tags.hpp>


namespace fas{ namespace util { namespace memcached{

namespace aa = ::fas::aop;
namespace ap = ::fas::pattern;
namespace af = ::fas::filter;
namespace ai = ::fas::inet;

struct server_stub_list: ap::type_list_n<
  /*_set_, _add_, _replace_,*/ _get_, _storage_
>::type{};

struct server_stub_advice:
  aa::advice< server_stub_list, af::ad_stub<bool> > {};


struct server_state_list: ap::type_list_n<
  state_ready<mode_server>, /*state_command,*/ state_storage<mode_server>, state_get<mode_server>
>::type{};

struct server_parser_advice:
  aa::advice< ap::tag<_parser_>, ad_server_parser<server_state_list> > {};

struct server_advice_list: ap::type_list_n<
  server_stub_advice,
  server_parser_advice,
  aa::alias<_parser_, af::_on_read_>,
  aa::alias<af::_writer_, _writer_>
  
>::type {};

struct server_aspect: aa::aspect<server_advice_list>{};

template<typename A = aa::aspect<> >
struct server: ai::server< typename aa::aspect_merge< server_aspect, A >::type >{};

}}}

#endif
