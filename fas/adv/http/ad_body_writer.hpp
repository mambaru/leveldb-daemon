//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_ADV_HTTP_AD_BODY_WRITER_HPP
#define FAS_ADV_HTTP_AD_BODY_WRITER_HPP

#include <fas/misc/http.hpp>
#include <fas/adv/http/tags.hpp>


namespace fas{ namespace adv{ namespace http{

namespace amh = ::fas::misc::http;

/*
class ad_body_writer
{
  bool _ischunked;
  bool _keep_alive;
  int _content_length;
public:

  bool ischunked() const { return _ischunked;}
  bool is_keep_alive() const { return _keep_alive; }
  int get_content_length() const {return _content_length;}

  template<typename T>
  void activate(T& t)
  {
    _ischunked = t.get_aspect().template get<_response_header_>().ischunked();
    _content_length = t.get_aspect().template get<_response_header_>().get_content_length();
    _keep_alive = t.get_aspect().template get<_request_header_>().is_keep_alive();
    t.get_aspect().template get<_process_>().activate(t);
    t.get_aspect().template get<_body_buf_writer_>().activate(t);
    t.get_aspect().template get<_nobody_writer_>().activate(t);
    t.get_aspect().template get<_body_chunk_writer_>().activate(t);
  }

  template<typename T>
  void operator ()(T& t, const char *d, size_t s)
  {

    if (_ischunked)
      t.get_aspect().template get<_body_chunk_writer_>()(t, d, s);
    else
    {
      if (_content_length > 0 )
        t.get_aspect().template get<_body_buf_writer_>()(t, d, s);
      else if (_content_length < 0 && s!=0)
        t.get_aspect().template get<_nobody_writer_>()(t, d, s);
    }

    if ( !_keep_alive )
    {
      if ( _ischunked && s==0 || !_ischunked)
        t.release();
    }
  }
};
*/

}}}


#endif
