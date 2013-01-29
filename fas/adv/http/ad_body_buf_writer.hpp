//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_ADV_HTTP_BODY_BUF_WRITER_HPP
#define FAS_ADV_HTTP_BODY_BUF_WRITER_HPP

#include <vector>
#include <cstdlib>
#include <fas/misc/http.hpp>
#include <fas/adv/http/tags.hpp>
#include <fas/adv/ad_state.hpp>

namespace fas{ namespace adv{ namespace http{

namespace amh = ::fas::misc::http;

class ad_body_buf_writer
{
  typedef std::vector<char> data_type;
  data_type _buffer;
  int _content_length;
public:

  size_t capacity() const { return _buffer.capacity(); }

  template<typename T>
  void activate(T& t)
  {
    _content_length = t.get_aspect().template get<_response_header_>().get_content_length();
    // _buffer.clear();
    data_type().swap(_buffer);
  }

  template<typename T>
  void operator ()(T& t, const char *d, size_t s)
  {
    

    if ( _content_length <= static_cast<int>(_buffer.size()) )
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
      t.get_aspect().template get<_writer_>()(t, d, needed);
    else if ( static_cast<int>(_buffer.size()) == _content_length)
    {
      t.get_aspect().template get<_writer_>()(t, &(_buffer[0]), _buffer.size());
      _buffer.clear();
    }

    else if ( static_cast<int>( _buffer.size() ) > _content_length)
      throw;

    if (tail!=0 && !_buffer.empty())
      throw;

    if (_buffer.empty())
    {
      if ( tail > 0)
        fas::adv::state<_writer_state_, _state_write_initial_>(t)(t, d + (s - tail), tail);
      else
        fas::adv::state<_writer_state_, _state_write_initial_>(t)(t);
      t.get_aspect().template get<_body_writed_>()(t);
    }
  }

private:

};

}}}


#endif
