//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_ADV_AMF_SRV_AD_CONTENT_HPP
#define FAS_ADV_AMF_SRV_AD_CONTENT_HPP

#include <vector>

#include <fas/pattern/type_list.hpp>
#include <fas/adv/amf/srv/tags.hpp>
//#include <fas/amf/types.hpp>

namespace fas{ namespace  adv{ namespace amf{ namespace srv{

namespace ap = ::fas::pattern;
namespace ada = ::fas::adv::amf;

class ad_content
{
public:

  template<typename T>
  void operator()(T& t, const char* d, size_t s)
  {
    typedef typename T::/*aspect::*/advice_list advice_list;
    typedef typename ap::select<_content_group_, advice_list >::type content_list;
   // ap::static_check< ap::type_count<_content_group_, content_list>::result==0 >();
    const ada::amf_head_8* h8 = reinterpret_cast<const ada::amf_head_8*>(d);
    unsigned char content = h8->content_type();
    _select(content_list(), content, t, d, s);
  } 

  template<typename L, typename T>
  void _select(L, size_t content, T& t, const char* d, size_t s)
  {
    typedef typename L::left_type advice;

    if ( advice::number == content )
      t.get_aspect().template get<advice>()(t, d, s);
    else
      _select( typename L::right_type(), content, t, d, s);
  }

  template< typename T>
  void _select(ap::empty_type, size_t /*content*/, T& /*t*/, const char* /*d*/, size_t /*s*/)
  {
    return;
  }
private:

};


}}}}

#endif //TR_AMF_SPLITTER_H
