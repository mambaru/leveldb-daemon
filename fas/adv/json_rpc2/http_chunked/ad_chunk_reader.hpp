//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_ADV_JSON_RPC2_HTTP_CHUNKED_AD_CHUNK_READER_HPP
#define FAS_ADV_JSON_RPC2_HTTP_CHUNKED_AD_CHUNK_READER_HPP

#include <vector>
#include <cstdlib>
#include <fas/adv/json_rpc2/http_chunked/tags.hpp>

namespace fas{ namespace adv{ namespace json_rpc2{ namespace http_chunked{


class ad_chunk_reader
{
  typedef std::vector<char> data_type;
  data_type _data;
  int chunk_size;

public:
  template<typename T>
  void clear(T& t)
  {
    chunk_size = -1;
    _data.clear();
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
        t.get_aspect().template get<_process_>()(t, body, ss);
      else if (ss == 0)
      {
        t.get_aspect().template get<_process_>()(t, body, ss);
      }
    }
  }

private:
  // s < 0 пакет не готов
  // s - звершено
  // s > 0 длина пакета
  const char* _process_chunk(const char* beg, const char* end, const char** d, int* s )
  {
    if (chunk_size == -1)
      beg = _process_length(beg, end);
    if (chunk_size != -1)
      end = _process_body(beg, end);
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
      else if (_data.size() == chunk_size) {
        *d = &(_data[0]);
        if (_data.size()!=*s){
          throw std::logic_error("read chunk error");
        }
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
        chunk_size = std::strtol(&(_data[0]), 0, 16);
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
      chunk_size = std::strtol(beg, 0, 16);
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
      if ( std::distance(beg, end) + _data.size() >= chunk_size )
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
    return true;
  };

};

}}}}


#endif
