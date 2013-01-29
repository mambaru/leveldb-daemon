//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_ADV_HTTP_BODY_READER_HPP
#define FAS_ADV_HTTP_BODY_READER_HPP

#include <fas/misc/http.hpp>
#include <fas/adv/http/tags.hpp>


namespace fas{ namespace adv{ namespace http{

namespace amh = ::fas::misc::http;

/*
class ad_body_reader
{
  bool _ischunked;
  int _content_length;
public:

  bool ischunked() const { return _ischunked;}
  int get_content_length() const {return _content_length;}
  template<typename T>
  void activate(T& t)
  {
    _ischunked = t.get_aspect().template get<_request_header_>().ischunked();
    _content_length = t.get_aspect().template get<_request_header_>().get_content_length();
    t.get_aspect().template get<_process_>().activate(t);
    t.get_aspect().template get<_body_buf_reader_>().activate(t);
    t.get_aspect().template get<_nobody_reader_>().activate(t);
    t.get_aspect().template get<_body_chunk_reader_>().activate(t);
  }

  template<typename T>
  void operator ()(T& t, const char *d, size_t s)
  {


    if (_ischunked)
      t.get_aspect().template get<_body_chunk_reader_>()(t, d, s);
    else
    {
      if (_content_length > 0 )
        t.get_aspect().template get<_body_buf_reader_>()(t, d, s);
      else if (_content_length < 0 && s!=0)
        t.get_aspect().template get<_body_nonbuf_reader_>()(t, d, s);
      else
        t.get_aspect().template get<_reader_state_>()
                      .template state<_header_read_state_>(t)(t, d, s);
    }
  }
};
*/
}}}


#endif
