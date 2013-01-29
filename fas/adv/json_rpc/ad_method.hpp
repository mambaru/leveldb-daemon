//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_ADV_JSON_RPC_AD_METHOD_HPP
#define FAS_ADV_JSON_RPC_AD_METHOD_HPP

#include <fas/pattern/type_list.hpp>
#include <fas/misc/json.hpp>

namespace fas{ namespace adv{ namespace json_rpc{

namespace ap = ::fas::pattern;
namespace amj = ::fas::misc::json;

// Псевдо-интерфейс
class ad_method1
{
public:
  // Обязателен для request и call
  const char* name() const {return "";}

  bool request(int id, const amj::json_x& jv) {}
  bool request(int id, const amj::json_object& jo) {}
  bool request(int id, const amj::json_array& ja) {}
  bool request(int id, const amj::json_value& jv) {}

  void notify(const amj::json_x& jv) {}
  void notify(const amj::json_object& jo) {}
  void notify(const amj::json_array& ja) {}
  void notify(const amj::json_value& jv) {}

  void response(amj::json_pack& pack) {}

  void call(amj::json_pack& pack) {}

  void result(const amj::json_x& jv) {}
  void result(const amj::json_object& jo) {}
  void result(const amj::json_array& ja) {}
  void result(const amj::json_value& jv) {}
};

}}}

#endif
