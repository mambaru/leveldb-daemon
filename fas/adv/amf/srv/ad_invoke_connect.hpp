//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_ADV_AMF_SRV_AD_INVOKE_CONNECT_HPP
#define FAS_ADV_AMF_SRV_AD_INVOKE_CONNECT_HPP

#include <vector>

#include <fas/adv/amf/srv/tags.hpp>
//#include <fas/amf/types.hpp>
#include <fas/adv/amf/srv/unserializer.hpp>
#include <fas/adv/amf/array.hpp>

namespace fas{ namespace  adv{ namespace amf{ namespace srv{

namespace ada = ::fas::adv::amf;

class ad_invoke_connect
{
public:
  const char* name() const { return "connect";}

  template<typename T>
  void operator()(T& t, ada::invoke_info& info)
  {
    ada::invoke_result result(info.get_object_id(), info.get_call_id());
    result[0]["level"] = "status";
    result[0]["code"] = "NetConnection.Connect.Success";
    result[0]["description"] = "Connection succeeded."; 

    t.get_aspect().template get<_response_>()(t, result);
    return;
  } 

private:

};


}}}}

#endif //FAS_ADV_AMF_SRV_AD_INVOKE_UNSERIALIZER_HPP
