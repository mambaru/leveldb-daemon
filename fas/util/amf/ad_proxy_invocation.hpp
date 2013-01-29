//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_UTIL_AMF_AD_PROXY_INVOCATION_HPP
#define FAS_UTIL_AMF_AD_PROXY_INVOCATION_HPP


#include <fas/util/amf/unserialize_invoke.hpp>
#include <fas/util/amf/serialize_invoke.hpp>
#include <fas/util/multi_type.hpp>
#include <fas/pattern/type_list.hpp>
#include <fas/util/amf/tags.hpp>

#include <string>

namespace fas{ namespace  util{ namespace amf{

namespace ap = ::fas::pattern;

template<typename L>
class ad_proxy_invocation
{
public:

  typedef typename ap::type_list_traits<L>::type invoke_list;
  template<typename T>
  void operator()(T& t, const char* d, size_t s)
  {
    _ausi.assign(d, s);
    _select_invoke(t, invoke_list() );
  }

private:

  template<typename TI, typename T>
  void _invoke(T& t)
  {
    serialize_invoke assr("_result", _ausi.call_id());
    if ( t.get_aspect().template get<TI>()(t, _ausi, assr) )
    {
      assr.finalize( _ausi.head()->object_id() );
      t.get_aspect().template get<_chunk_prepare_>()(t, assr);
    }
  }

  template<typename LL, typename LR, typename T>
  void _select_invoke(T& t, ap::type_list<LL, LR>)
  {
    if ( _ausi.name() == t.get_aspect().template get<LL>().name() )
      _invoke<LL>(t);
    else
      _select_invoke( t, LR());
  }
  
  template<typename T>
  void _select_invoke(T& t, ap::empty_type)
  {
  }


  unserialize_invoke _ausi;

};

}}}

#endif
