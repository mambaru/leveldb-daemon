//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_ADV_HTTP_AD_PROCESS_HPP
#define FAS_ADV_HTTP_AD_PROCESS_HPP

#include <fas/misc/http.hpp>
#include <fas/adv/http/tags.hpp>

namespace fas{ namespace adv{ namespace http{

namespace amh = ::fas::misc::http;

class ad_process
{
public:
  template<typename T>
  void activate(T& t)
  {
  }

  template<typename T>
  void operator ()(T& t, const char *d, size_t s)
  {
    
    const amh::initial_line& initial_line =
      t.get_aspect().template get<_request_header_>().initial_line();

    // если это клиент, то вместо ури будет получен код ошибки
    std::string uri( initial_line.request_uri() );

    t.get_aspect().template get<_process_uri_>()(t, uri.c_str(), d, s);

    // t.get_aspect().template get<_outgoing_>()(t, d, s);
    /*
    t.get_aspect().template get<_response_header_>().get_status_line() 
      = t.get_aspect().template get<_request_header_>().get_status_line();
    t.get_aspect().template get<_response_header_>().get_header() 
      = t.get_aspect().template get<_request_header_>().get_header();
    t.get_aspect().template get<_body_writer_>()(t, d, s);

    */
  }
};

}}}


#endif
