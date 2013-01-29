//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_ADV_AMF_SRV_AD_RESPONSE_HPP
#define FAS_ADV_AMF_SRV_AD_RESPONSE_HPP

#include <vector>

#include <fas/adv/amf/srv/tags.hpp>
//#include <fas/amf/types.hpp>
#include <fas/adv/amf/srv/serializer.hpp>
#include <fas/adv/amf/array.hpp>

namespace fas{ namespace  adv{ namespace amf{ namespace srv{

namespace ada = ::fas::adv::amf;


class ad_response
{
public:

  template<typename T>
  void operator()(T& t, ada::invoke_result& arr)
  {
    invoke_serializer ser;
    invoke_serializer::data_type d;
    ser.serialize(arr, d);
    t.get_aspect().template get<_chunk_prepare_>()(t, 
      &d[0], 
      static_cast<typename T::write_size_type>(d.size())
    );
    
    return;
  } 

private:


};


}}}}

#endif //FAS_ADV_AMF_SRV_AD_INVOKE_UNSERIALIZER_HPP
