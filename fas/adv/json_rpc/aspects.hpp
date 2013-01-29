//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_ADV_JSON_RPC_ASPECT_HPP
#define FAS_ADV_JSON_RPC_ASPECT_HPP

#include <fas/adv/json_rpc/tags.hpp>
#include <fas/adv/json_rpc/advices.hpp>

namespace fas{ namespace adv{ namespace json_rpc{

struct advice_list
  : ap::type_list_n<
      invoke_advice,
      outgoing_object_advice,
      proxy_advice,
      stubs_advice
    >::type
{};

struct aspect: aa::aspect<advice_list> {};

}}}

#endif
