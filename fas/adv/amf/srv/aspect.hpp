//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_ADV_AMF_SRV_ASPECT_HPP
#define FAS_ADV_AMF_SRV_ASPECT_HPP


#include <fas/adv/amf/srv/ad_state1.hpp>
#include <fas/adv/amf/srv/ad_state2.hpp>
#include <fas/adv/amf/srv/ad_state3.hpp>
#include <fas/adv/amf/srv/ad_state_error.hpp>

#include <fas/adv/amf/srv/ad_chunk_splitter.hpp>
#include <fas/adv/amf/srv/ad_content.hpp>
#include <fas/adv/amf/srv/ad_collector.hpp>
#include <fas/adv/amf/srv/ad_invoke_unserializer.hpp>
#include <fas/adv/amf/srv/ad_invoke.hpp>
#include <fas/adv/amf/srv/ad_invoke_connect.hpp>
#include <fas/adv/amf/srv/ad_response.hpp>
#include <fas/adv/amf/srv/ad_call.hpp>
#include <fas/adv/amf/srv/ad_chunk_prepare.hpp>

#include <fas/adv/amf/srv/tags.hpp>
#include <fas/adv/ad_state.hpp>
#include <fas/adv/ad_stub.hpp>
#include <fas/adv/ad_cleaner.hpp>
#include <fas/aop/advice.hpp>
#include <fas/aop/aspect.hpp>
#include <fas/aop/tag.hpp>

#include <fas/filter/binary_filter.hpp>

namespace fas{ namespace adv{ namespace amf{ namespace srv{ 

namespace aa = ::fas::aop;
namespace ad = ::fas::adv;
namespace af = ::fas::filter;
namespace ap = ::fas::pattern;
//namespace ai = ::fas::inet;
//namespace am = ::fas::mux;



struct state_advice
  : aa::advice< 
      aa::tag_list_n< 
        aa::tag<_state_>, 
        aa::gtag<ad::_cleaner_>
      >::type,
      ad::ad_state<_state_group_, _state1_, true> 
  >{};

struct state1_advice
  : aa::advice< 
      aa::tag_list_n< 
        aa::tag<_state1_>, 
        aa::gtag<_state_group_> 
      >::type,
      ad_state1 
  >{};

struct state2_advice
  : aa::advice< 
      aa::tag_list_n< 
        aa::tag<_state2_>, 
        aa::gtag<_state_group_> 
      >::type,
      ad_state2 
  >{};

struct state3_advice
  : aa::advice< 
      aa::tag_list_n< 
        aa::tag<_state3_>, 
        aa::gtag<_state_group_> 
      >::type,
      ad_state3 
  >{};

struct state_error_advice
  : aa::advice< 
      aa::tag_list_n< 
        aa::tag<_state_error_>, 
        aa::gtag<_state_group_>
      >::type,
      ad_state_error 
  >{};

struct chunk_splitter_advice
  : aa::advice< 
      aa::tag_list_n< 
        aa::tag<_chunk_splitter_>,
        aa::gtag<ad::_cleaner_>
      >::type,
      ad_chunk_splitter
  >{};

struct content_advive
  : aa::advice<
      aa::tag<_content_>,
      ad_content
  >{};

struct collector_advice
  : aa::advice< 
      aa::tag_list_n< 
        aa::tag<_collector_>,
        aa::ntag<20, _content_>,
        aa::gtag<ad::_cleaner_>
      >::type,
      ad_collector<_invoke_unserializer_>
  >{};

struct invoke_unserializer_advice
  : aa::advice< 
      aa::tag_list_n< 
        aa::tag<_invoke_unserializer_>
      >::type,
      ad_invoke_unserializer
  >{};

struct invoke_advice
  : aa::advice< 
      aa::tag_list_n< 
        aa::tag<_invoke_>
      >::type,
      ad_invoke
  >{};

struct invoke_connect_advice
  : aa::advice< 
      aa::tag_list_n< 
        aa::gtag<_invoke_group_>,
        aa::tag<_invoke_connect_>
      >::type,
      ad_invoke_connect
  >{};

struct call_advice
  : aa::advice< 
      aa::tag_list_n< 
        aa::tag<_call_>
      >::type,
      ad_call
  >{};

struct invoke_resonse_advice
  : aa::advice< 
      aa::tag_list_n< 
        aa::tag<_response_>
      >::type,
      ad_response
  >{};

struct chunk_prepare_advice
  : aa::advice< 
      aa::tag_list_n< 
        aa::tag<_chunk_prepare_>
      >::type,
      ad_chunk_prepare
  >{};
/*
struct _invoke_method1_{};

struct invoke_method1_advice
  : aa::advice< 
      aa::tag_list_n< 
        aa::gtag<_invoke_group_>,
        aa::tag<_invoke_method1_>
      >::type,
      ad_invoke_method1
  >{};
*/
struct advice_list
  : ap::type_list_n<
      state_advice,
      state1_advice,
      state2_advice,
      state3_advice,
      chunk_splitter_advice,
      content_advive,
      collector_advice,
      invoke_unserializer_advice,
      invoke_advice,
      ap::type_list_n<
        //invoke_connect_advice,
        invoke_resonse_advice,
        call_advice,
        chunk_prepare_advice,
        // invoke_method1_advice,
        aa::advice< aa::tag<_result_>, ad::ad_stub<> >,
        aa::alias<_state_, af::_on_read_> ,
        aa::alias<af::_writer_, _writer_ > 
      >::type
  >::type{};

struct aspect: aa::aspect<advice_list>{};


}}}}

#endif
