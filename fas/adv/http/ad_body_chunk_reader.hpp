//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_ADV_HTTP_BODY_CHUNK_READER_HPP
#define FAS_ADV_HTTP_BODY_CHUNK_READER_HPP

#include <vector>
#include <cstdlib>
#include <fas/misc/http.hpp>
#include <fas/adv/http/tags.hpp>

namespace fas{ namespace adv{ namespace http{

namespace amh = ::fas::misc::http;

class ad_body_chunk_reader
{
  typedef std::vector<char> data_type;
  data_type _data;
  int chunk_size;

public:

  size_t capacity() const { return _data.capacity(); }

  template<typename T>
  void activate(T& )
  {
    chunk_size = -1;
    // _data.clear();
    data_type().swap(_data);
  }

  template<typename T>
  void operator ()(T& t, const char *d, size_t s)
  {
    if (s == 0) return;
    const char* b = d;
    const char* e = d + s;
    const char* body = 0;
    int ss = 0;
    for (;b!=e; )
    {
      b = _process_chunk(b, e, &body, &ss);
      if (ss > 0)
      {
        t.get_aspect().template get<_process_>()(t, body, ss);
        _data.clear();
      }
      else if (ss == 0)
      {
        t.get_aspect().template get<_process_>()(t, 0, 0);
        t.get_aspect().template get<_reader_state_>()
           .template state<_state_read_header_>(t)
           (t, b, std::distance(b, e));
        t.get_aspect().template get<_body_readed_>()(t);
        _data.clear();
      }
    }
  }

private:
  // s < 0 пакет не готов
  // s - звершено
  // s > 0 длина пакета (без rn)
  const char* _process_chunk(const char* beg, const char* end, const char** d, int* s )
  {
    if (chunk_size == -1)
      beg = _process_length(beg, end);
    if (chunk_size != -1)
      end = _process_body(beg, end);
    if ( chunk_size > 1)
      *s = chunk_size - 2;
    else 
      *s = chunk_size;
    if (chunk_size == -1)
      *d = 0;
    else
    {
      if (_data.empty() && std::distance(beg, end) == chunk_size)
      {
        *d = beg;
        chunk_size = -1;
      }
      else if (_data.size() == static_cast<size_t>(chunk_size) && !_data.empty() ) {
        *d = &(_data[0]);
        chunk_size = -1;
      }
      else
      {
        *d = 0;
        *s = -1;
      }
    }
    return end;
  }

  int prepare_chunk_size( const char* beg, size_t s)
  {
    if ( s == 0 || s > 8 )
      return 0;
    char hex[16]={0};
    for ( register size_t i = 0; i < s; ++i )
      hex[i]=beg[i];
    return std::strtol(hex, 0, 16);
  }

  const char* _process_length(const char* beg, const char* end)
  {
    if (chunk_size!=-1)
      return beg;

    if (!_data.empty())
    {
      // в _data кусок длины чанка
      for (;beg!=end && _ready_length();++beg)
        _data.push_back(*beg);

      if (_ready_length())
      {
        // 
        chunk_size = prepare_chunk_size( &(_data[0]), _data.size() );
        // chunk_size = std::strtol(&(_data[0]), 0, 16);
        if ( chunk_size > 0 ) chunk_size += 2; // захватываем rn, т.к. данные передаютяс len\r\ndata\r\n
        _data.clear();
      }
      return beg;
    }

    const char* ne = std::find(beg, end, '\r');
    if (std::distance(ne, end) < 2)
    {
      _data.assign(beg, end);
      return end;
    }
    else
    {
      ne+=2;
      chunk_size = prepare_chunk_size( beg, std::distance(beg, ne) );
      // chunk_size = std::strtol(beg, 0, 16);
      if ( chunk_size > 0 ) chunk_size += 2;
    }
    return ne;
  }

  const char* _process_body(const char* beg, const char* end)
  {
    if ( chunk_size == 0 )
      return beg;
    if ( _data.empty() )
    {
      if ( chunk_size <= std::distance(beg, end) )
      {
        
        return beg + chunk_size;
      }
      else
      {
        
        _data.assign(beg, end);
        return end;
      }
    }
    else
    {
      if ( int(std::distance(beg, end) + _data.size()) >= chunk_size )
      {
        
        size_t tail = chunk_size - _data.size();
        std::copy(beg, beg+tail, std::back_inserter(_data));
        return beg + tail;
      }
      else
      {
        
        std::copy(beg, end, std::back_inserter(_data));
        return end;
      }
    }
    throw;
  }

  bool _ready_length() const 
  {
    if (_data.size()<2) return false;
    data_type::const_reverse_iterator ritr = _data.rbegin();
    if (*(ritr++)!='\n') return false;
    if (*(ritr++)!='\r') return false;
    //if (*(ritr++)!='\n') return false;
    //if (*(ritr++)!='\r') return false;
    return true;
  };

};

}}}


#endif
