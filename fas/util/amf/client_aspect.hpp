//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_UTIL_AMF_CLIENT_ASPECT_HPP
#define FAS_UTIL_AMF_CLIENT_ASPECT_HPP

#include <fas/filter/ad_cleaner.hpp>
#include <fas/inet/client.hpp>

#include <fas/util/amf/amf_advices.hpp>
#include <fas/util/amf/ad_client_handshake.hpp>
#include <fas/util/amf/ad_invoke_connect.hpp>


namespace fas{ namespace  util{ namespace amf{

namespace aa = ::fas::aop;
namespace af = ::fas::filter;
namespace ap = ::fas::pattern;
namespace ai = ::fas::inet;

typedef ap::type_list_n< 
          _process_client_handshake_,
          _chunk_splitter_,
          _chunk_collector_
        >::type client_cleaner_list;

struct client_cleaner_advice:
  aa::advice<
    ap::tag_list_n< _cleaner_, af::_on_assign_ >,
    af::ad_cleaner<client_cleaner_list>
  >
{};

struct client_handshake_advice:
  aa::advice<
    ap::tag_list_n<_client_handshake_, ai::_on_client_connect_>,
    ad_client_handshake
  >
{};

struct process_client_handshake_advice:
  aa::advice<
    ap::tag<_process_client_handshake_>,
    ad_process_client_handshake 
  >
{};

template<typename PL, typename SL>
struct make_server_advice_list
{
  typedef typename ap::type_list_n<
    client_cleaner_advice,
    client_handshake_advice,
    process_client_handshake_advice,
    chunk_splitter_advice,
    process_content_advice,
    chunk_collector_advice
  >::type receive_list;

  typedef typename ap::type_list_n<
    proxy_invocation_advice<PL>,
    stub_invocation_advice<SL>
  >::type invoke_list;

  typedef typename ap::type_list_n<
    receive_list,
    invoke_list,
    chunk_prepare_advice,
    aa::alias<af::_writer_, _write_>,
    aa::alias<_process_client_handshake_, af::_on_read_>
    
  >::type type;
};


template<typename AL = aa::aspect< >,
         typename PL = ap::empty_type,
         typename SL = ap::empty_type
        >
struct client_aspect
  : aa::aspect_merge<
      aa::aspect<
        typename make_server_advice_list<
          typename ap::type_list_n<_stub_invocation_, PL>::type,
          SL
        >::type
      >,
      AL
    >::type
{};


}}}

#endif
