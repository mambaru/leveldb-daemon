//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_ADV_AMF_SRV_AD_CALL_HPP
#define FAS_ADV_AMF_SRV_AD_CALL_HPP

#include <vector>

#include <fas/adv/amf/srv/tags.hpp>
//#include <fas/amf/types.hpp>
#include <fas/adv/amf/srv/serializer.hpp>
#include <fas/adv/amf/array.hpp>

namespace fas{ namespace  adv{ namespace amf{ namespace srv{

namespace ada = ::fas::adv::amf;


class ad_call
{
  double _call_id_count;
public:
  ad_call():_call_id_count(100.0){}
  
  template<typename T>
  void operator()(T& t, ada::call_info& arr)
  {
    if (arr.get_call_id() == 0.0 ) 
      arr.set_call_id(++_call_id_count);

    invoke_serializer ser;
    invoke_serializer::data_type d;
    ser.serialize(arr, d);
    t.get_aspect().template get<_chunk_prepare_>()(t, &d[0], d.size());
  } 

private:


};


}}}}

#endif //FAS_ADV_AMF_SRV_AD_INVOKE_UNSERIALIZER_HPP
