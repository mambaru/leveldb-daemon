//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_ADV_HTTP_BODY_BUF_READER_HPP
#define FAS_ADV_HTTP_BODY_BUF_READER_HPP

#include <fas/misc/http.hpp>
#include <fas/adv/http/tags.hpp>

namespace fas{ namespace adv{ namespace http{

namespace amh = ::fas::misc::http;

class ad_body_buf_reader
{
  int _content_length;
  typedef std::vector<char> data_type;
  data_type _buffer;
public:

  size_t capacity() const { return _buffer.capacity(); }

  template<typename T>
  void activate(T& t)
  {
    _content_length = t.get_aspect().template get<_request_header_>().get_content_length();
    // _buffer.clear();
    data_type().swap(_buffer);
  }

  template<typename T>
  void operator ()(T& t, const char *d, size_t s)
  {
    if (s == 0) return;
    if ( static_cast<size_t>(_content_length) <= _buffer.size() )
      throw;

    size_t needed = _content_length - _buffer.size();
    size_t tail = 0;

    if (!_buffer.empty())
    {
      if (needed < s)
      {
        std::copy( d, d + needed, std::back_inserter(_buffer));
        tail = s - needed;
      }
      else
        std::copy( d, d + s, std::back_inserter(_buffer));
    }
    else
    {
      if ( needed > s)
        _buffer.assign(d, d + s);
      else
        tail = s - needed;
    }

    if ( _buffer.empty() )
      t.get_aspect().template get<_process_>()(t, d, needed);
    else if (_buffer.size() == static_cast<size_t>(_content_length) )
    {
      t.get_aspect().template get<_process_>()(t, &(_buffer[0]), _buffer.size());
      _buffer.clear();
    }
    else if (_buffer.size() > static_cast<size_t>(_content_length) )
      throw;

    if (tail!=0 && !_buffer.empty())
      throw;

    if (_buffer.empty())
    {
      data_type().swap(_buffer);
      if (tail == 0)
        t.get_aspect().template get<_reader_state_>().template state<_state_read_header_>(t);
      else
        t.get_aspect().template get<_reader_state_>()
           .template state<_state_read_header_>(t)
           (t, d + (s - tail), tail);

      t.get_aspect().template get<_body_readed_>()(t);
    }
  }
};

}}}


#endif
