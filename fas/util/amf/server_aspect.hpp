//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_UTIL_AMF_AMF_SERVER_ASPECT_HPP
#define FAS_UTIL_AMF_AMF_SERVER_ASPECT_HPP

#include <fas/adv/ad_cleaner.hpp>
#include <fas/util/amf/ad_server_handshake.hpp>
#include <fas/util/amf/amf_advices.hpp>
#include <fas/util/amf/ad_invoke_connect.hpp>


namespace fas{ namespace  util{ namespace amf{

namespace aa = ::fas::aop;
namespace ad = ::fas::adv;
namespace af = ::fas::filter;
namespace ap = ::fas::pattern;

struct cleaner_list
  :ap::type_list_n< 
          _server_handshake_,
          _chunk_splitter_,
          _chunk_collector_,
          _invoke_connect_
        >::type 
{};

struct cleaner_advice:
  aa::advice<
    aa::tag_list_n< _cleaner_, af::_on_assign_ >,
    ad::ad_cleaner<cleaner_list>
  >
{};

struct server_handshake_advice:
  aa::advice<
    aa::tag_list_n<_server_handshake_, af::_on_read_>,
    ad_server_handshake
  >
{};

struct process_first_handshake_advice:
  aa::advice<
    aa::tag<_process_first_handshake_>,
    ad_process_first_handshake 
  >
{};

struct process_second_handshake_advice:
  aa::advice<
    aa::tag<_process_second_handshake_>,
    ad_process_second_handshake
  >
{};

struct invoke_connect_advice:
  aa::advice<
     aa::tag<_invoke_connect_>,
     ad_invoke_connect
  >
{};


template<typename PL, typename SL>
struct make_server_advice_list_old
{
  typedef typename ap::type_list_n<
    cleaner_advice,
    server_handshake_advice,
    process_first_handshake_advice,
    process_second_handshake_advice,
    chunk_splitter_advice,
    process_content_advice,
    chunk_collector_advice
  >::type receive_list;

  typedef typename ap::type_list_n<
    proxy_invocation_advice<PL>,
    stub_invocation_advice<SL>,
    invoke_connect_advice
  >::type invoke_list;

  typedef typename ap::type_list_n<
    receive_list,
    invoke_list,
    chunk_prepare_advice,
    aa::alias<af::_writer_, _write_>
  >::type type;
};

struct amf_server_advice_list
  : ap::type_list_n <
      cleaner_advice,
      server_handshake_advice,
      process_first_handshake_advice,
      process_second_handshake_advice,
      chunk_splitter_advice,
      process_content_advice,
      chunk_collector_advice,
      chunk_prepare_advice,
      aa::alias<af::_writer_, _write_>
  >::type
{};

template<typename PL, typename SL>
struct make_server_advice_list
{
  typedef typename ap::type_list_n<_invoke_connect_, _stub_invocation_, PL>::type pl;
  struct pia: proxy_invocation_advice<pl>{};
  struct sia: stub_invocation_advice<SL>{};
  struct type: ap::type_list_n<
    amf_server_advice_list,
    sia,
    pia
  >::type{};
};

template<typename AL = aa::aspect< >,
         typename PL = ap::empty_type,
         typename SL = ap::empty_type
        >
struct server_aspect
  : aa::aspect_merge<
      aa::aspect<
        typename make_server_advice_list<
          /*typename ap::type_list_n<_invoke_connect_, _stub_invocation_,*/ PL /*>::type*/,
          SL
        >::type
      >,
      AL
    >::type
{};


struct amf_receive_list
  : ap::type_list_n<
    cleaner_advice,
    server_handshake_advice,
    process_first_handshake_advice,
    process_second_handshake_advice,
    chunk_splitter_advice,
    process_content_advice,
    chunk_collector_advice,
    invoke_connect_advice,
    chunk_prepare_advice,
    aa::alias<af::_writer_, _write_>
  >::type {};

template<typename PL = ap::empty_type,
         typename SL = ap::empty_type>
struct amf_advice_list:
  ap::type_list_n<
     amf_receive_list,
     proxy_invocation_advice<
        typename ap::type_list_n<
            _invoke_connect_, 
            _stub_invocation_, 
            PL 
        >::type 
     >,
     stub_invocation_advice<SL>
  >::type
{};


}}}

#endif
