//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2010
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_ADV_ASP_HTTP_SERVER_AD_INPUT_HPP
#define FAS_ADV_ASP_HTTP_SERVER_AD_INPUT_HPP

#include <fas/misc/http/exceptions.hpp>

namespace fas{ namespace adv{ namespace asp{ namespace http{ namespace server{

namespace ah = ::fas::http;

struct ad_input
{
public:
  template<typename T>
  void operator()(T& t, const char* d, size_t s)
  {
    try
    {
       t.get_aspect().template get<_reader_>()(t, d, s);
    }
    catch( const ah::http_exception& e )
    {
      t.get_aspect().template get<_exception_>()(t, e);
    }
  }
};

}}}}}

#endif
