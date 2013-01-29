//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2010
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_ADV_ASP_HTTP_SERVER_URI_PROCESSOR_HPP
#define FAS_ADV_ASP_HTTP_SERVER_URI_PROCESSOR_HPP

namespace fas{ namespace adv{ namespace asp{ namespace http{ namespace server{


template<typename A = aa::aspect<> >
struct uri_processor
{
  template< typename T, typename H, typename B >
  void operator()( T& t, const H& h, typename B& b )
  {
  }
};

}}}}}

#endif
