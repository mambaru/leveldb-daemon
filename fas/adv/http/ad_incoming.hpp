//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_ADV_HTTP_AD_INCOMING_HPP
#define FAS_ADV_HTTP_AD_INCOMING_HPP

#include <fas/misc/http.hpp>
#include <fas/adv/http/tags.hpp>

namespace fas{ namespace adv{ namespace http{

namespace amh = ::fas::misc::http;

// Отправлят пользовательские данные
class ad_incoming
{
  amh::header _header;
  // bool _chunked;
  bool _use_buff; // используется с не chunked для запихивания в один пакет
  typedef std::vector<char> data_type;
  data_type _data;
public:

  size_t capacity() const { return _data.capacity(); }

  ad_incoming(): _use_buff(false) {}

  template<typename T>
  void clear(T&)  { _use_buff = false; data_type().swap(_data);  }

  amh::header& header() { return _header;}

  const amh::header& header() const { return _header;}

  void begin_buffering() 
  {
    _use_buff = true;
  }

  template<typename T>
  void flush(T& t)
  {
    if ( _use_buff )
    {
       _use_buff = false;
       t.get_aspect().template get<_response_header_>().header() = _header;

       if ( !_header.is_chunked() )
       {
         t.get_aspect().template get<_response_header_>().set_content_length(_data.size());
       }

       if ( !_data.empty() )
       {
         t.get_aspect().template get<_writer_state_>()(t, &(_data[0]), _data.size());
       }
       else
       {
         t.get_aspect().template get<_writer_state_>()(t, (const char*)0, (size_t)0);
       }
    }

    data_type().swap(_data);
  }

  template<typename T>
  void operator ()(T& t, const char *d, size_t s)
  {
    if ( _use_buff )
    {
      _data.reserve(s);
      std::copy(d, d + s, std::back_inserter(_data) );
    }
    else
    {
      t.get_aspect().template get<_response_header_>().header() = _header;
      if ( !_header.is_chunked() )
        t.get_aspect().template get<_response_header_>().set_content_length(s);
      t.get_aspect().template get<_writer_state_>()(t, d, s);
    }
  }
};

}}}


#endif
