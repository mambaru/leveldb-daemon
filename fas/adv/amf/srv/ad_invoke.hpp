//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_ADV_AMF_SRV_AD_INVOKE_HPP
#define FAS_ADV_AMF_SRV_AD_INVOKE_HPP

#include <vector>

#include <fas/adv/amf/srv/tags.hpp>
//#include <fas/amf/types.hpp>
#include <fas/adv/amf/srv/unserializer.hpp>
#include <fas/adv/amf/array.hpp>

namespace fas{ namespace  adv{ namespace amf{ namespace srv{

namespace ada = ::fas::adv::amf;

class ad_invoke
{
public:

  template<typename T>
  void operator()(T& t, ada::invoke_info& arr)
  {
    if ( arr.get_name()=="_result" )
    {
      t.get_aspect().template get<_result_>()(t, arr);
    }
    else
    {
      typedef typename T::aspect::advice_list advice_list;
      typedef typename ap::select<_invoke_group_, advice_list>::type invoke_list;
  
      _select(invoke_list(), t, arr);
    }
  } 

private:

  template<typename L, typename T>
  void _select(L, T& t, /*const std::string& name, double call_id, */ada::invoke_info& arr)
  {
    if (arr.get_name() == t.get_aspect().template get<typename L::left_type>().name())
    {
      t.get_aspect().template get<typename L::left_type>()(t, arr);
    }
    else
      _select(typename L::right_type(), t, /*name, call_id,*/ arr);

  }

  template< typename T>
  void _select(ap::empty_type, T& t/*, const std::string& name, double call_id*/, ada::invoke_info& arr)
  {

  }

};


}}}}

#endif //FAS_ADV_AMF_SRV_AD_INVOKE_UNSERIALIZER_HPP
