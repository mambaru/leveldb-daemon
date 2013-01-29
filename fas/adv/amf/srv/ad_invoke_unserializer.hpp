//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_ADV_AMF_SRV_AD_INVOKE_UNSERIALIZER_HPP
#define FAS_ADV_AMF_SRV_AD_INVOKE_UNSERIALIZER_HPP

#include <vector>

#include <fas/adv/amf/srv/tags.hpp>
//#include <fas/amf/types.hpp>
#include <fas/adv/amf/srv/unserializer.hpp>
#include <fas/adv/amf/array.hpp>

namespace fas{ namespace  adv{ namespace amf{ namespace srv{

namespace ada = ::fas::adv::amf;

class ad_invoke_unserializer
{
public:

  template<typename T>
  void operator()(T& t, const char* d, size_t s)
  {
    ada::invoke_info arr;
    invoke_unserializer uns;
    uns.unserialize(d /*+ 12*/, s /*- 12*/, arr);
    // arr.set_object_id(0x03);
    /*arr.set_name( arr[-1] );
    arr.set_object_id( 0x03 );
    arr.set_call_id( arr[-2] );*/
    t.get_aspect().template get<_invoke_>()(t, arr);
  } 

private:

};


}}}}

#endif //FAS_ADV_AMF_SRV_AD_INVOKE_UNSERIALIZER_HPP
