//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_ADV_JSON_RPC_ADVICES_HPP
#define FAS_ADV_JSON_RPC_ADVICES_HPP
#include <fas/adv/ad_stub.hpp>
#include <fas/adv/ad_cleaner.hpp>
#include <fas/adv/json_rpc/tags.hpp>
#include <fas/adv/json_rpc/ad_invoke.hpp>
#include <fas/adv/json_rpc/ad_outgoing_object.hpp>
#include <fas/adv/json_rpc/ad_proxy.hpp>

namespace fas{ namespace adv{ namespace json_rpc{

namespace ad = ::fas::adv;

struct invoke_advice:
  aa::advice<
    aa::tag_list_n<
      aa::tag<_input_>,
      aa::tag<_invoke_>
    >::type,
   ad_invoke 
>
{};

struct outgoing_object_advice:
  aa::advice< 
    aa::tag_list_n<
      aa::gtag<ad::_cleaner_>,
      aa::tag<_outgoing_object_>
    >::type,
    ad_outgoing_object 
  >
{};

struct proxy_advice:
  aa::advice< 
    aa::tag_list_n<
      aa::gtag<ad::_cleaner_>,
      aa::tag<_proxy_>,
      aa::tag<_result_>
    >::type,
    ad_proxy
  >
{};

struct stubs_advice:
  aa::advice< 
    aa::tag_list_n<
      aa::tag<_unknown_method_>,
      aa::tag<_invalid_json_>,
      aa::tag<_lost_call_>
    >::type,
    ad::ad_stub<>
  >
{};

// struct cleaner_advice: aa::advice< ad::ad_cleaner<> > {};


}}}

#endif
