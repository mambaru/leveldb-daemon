//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2010
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_ADV_ASP_HTTP_SERVER_AD_OUTPUT_HPP
#define FAS_ADV_ASP_HTTP_SERVER_AD_OUTPUT_HPP

#include <fas/misc/http/exceptions.hpp>

namespace fas{ namespace adv{ namespace asp{ namespace http{ namespace server{

namespace ah = ::fas::http;

struct ad_output
{
public:
  template<typename T>
  void operator()(T& t, const char* d, size_t s)
  {
    if ( s > 0)
      t.get_aspect().template get<_output_>()(t, d, s);
    // Если запишем 0 то соединение закроется - нам этого не нужно
  }
};

}}}}}

#endif
