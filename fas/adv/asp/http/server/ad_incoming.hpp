//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2010
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_ADV_ASP_HTTP_SERVER_AD_INCOMING_HPP
#define FAS_ADV_ASP_HTTP_SERVER_AD_INCOMING_HPP

#include <fas/misc/http/exceptions.hpp>

namespace fas{ namespace adv{ namespace asp{ namespace http{ namespace server{

namespace ah = ::fas::http;

struct ad_incoming
{
public:
  template<typename T, typename H>
  void operator()(T& , H&)
  {
  }

  template<typename T, typename H, typename B>
  void operator()(T& t, H& , B& b)
  {
    if ( b.begin() != 0 )
      t.get_aspect().template get<_incoming_>()(t, b.begin(), b.size() );
  }
};

}}}}}

#endif
