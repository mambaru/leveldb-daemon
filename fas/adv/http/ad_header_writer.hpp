//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_ADV_HTTP_AD_HEADER_WRITER_HPP
#define FAS_ADV_HTTP_AD_HEADER_WRITER_HPP

#include <vector>
#include <fas/misc/http.hpp>
#include <fas/adv/http/tags.hpp>

namespace fas{ namespace adv{ namespace http{

namespace amh = ::fas::misc::http;

class ad_header_writer
{
public:

  template<typename T>
  void activate(T& t)
  {
    
    const amh::initial_line& sl =
      t.get_aspect().template get<_response_header_>().initial_line();
    if (!sl.empty())
       t.get_aspect().template get<_writer_>()(t, &(sl[0]), sl.size());
  }

  template<typename T>
  void operator ()(T& t, const char *d, size_t s)
  {
    amh::header& h =
      t.get_aspect().template get<_response_header_>().header();

    h.complete();
    if (!h.empty())
      t.get_aspect().template get<_writer_>()(t, &(h[0]), h.size());

    bool ischunked = h.is_chunked();
    int content_length = h.get_content_length();
    if (ischunked)
    {
      t.get_aspect().template get<_writer_state_>().template state<_state_write_chunk_>(t)(t, d, s);
    }
    else if (content_length > 0)
    {
      t.get_aspect().template get<_writer_state_>().template state<_state_write_body_>(t)(t, d, s);
    }
    else
    {
      t.get_aspect().template get<_writer_state_>().template state<_state_write_nobody_>(t)(t, d, s);
    }
  }
};

}}}


#endif
