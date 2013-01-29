//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2010
//
// Copyright: See COPYING file that comes with this distribution
// 

#ifndef FAS_ADV_ASP_HTTP_SERVER_AD_NOT_IMPLEMENTED_HPP
#define FAS_ADV_ASP_HTTP_SERVER_AD_NOT_IMPLEMENTED_HPP

#include <fas/misc/http/exceptions/server_error.hpp>

namespace fas{ namespace adv{ namespace asp{ namespace http{ namespace server{

namespace ah = ::fas::http;

struct ad_Not_Implemented
{
public:
  template<typename T>
  void operator()(T& , const char* , size_t )
  {
    throw ah::Not_Implemented_error();
  }
};

}}}}}

#endif
