//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_UTIL_AMF_AD_SERVER_HANDSHAKE_HPP
#define FAS_UTIL_AMF_AD_SERVER_HANDSHAKE_HPP

#include <vector>

#include <fas/util/amf/types.hpp>
#include <fas/util/amf/tags.hpp>

namespace fas{ namespace  util{ namespace amf{


class ad_server_handshake
{
  typedef std::vector<char> data_type;
  enum server_state { wait_first_handshake, wait_second_handshake, established};

public:

  ad_server_handshake()
    : _state(wait_first_handshake)
  {
  }

  ad_server_handshake( const ad_server_handshake&)
    : _state(wait_first_handshake)
  {
  }

  template<typename T>
  void clear(T&)
  {
    _handshake1.clear();
    _handshake2.clear();
    _databuffer.clear();
    _state = wait_first_handshake;
  }

  template<typename T>
  void operator()(T& t, const char* d, size_t s)
  {
    if ( _state == wait_first_handshake )
    {
      if (const char* dd = _receive(d, s, true, _handshake1, _handshake2) )
      {
        t.get_aspect().template get<_process_first_handshake_>()
            ( t, dd, HANDSHAKE_SIZE + 1);
        _state = wait_second_handshake;
        _handshake1.clear();
        data_type(_handshake1).swap(_handshake1);
        d = 0;
        s = 0;
        if ( _handshake2.empty() ) return;
      }
    }

    if ( _state == wait_second_handshake )
    {
      if (const char* dd = _receive(d, s, false, _handshake2, _databuffer) )
      {
        t.get_aspect().template get<_process_second_handshake_>()
            ( t, dd, HANDSHAKE_SIZE /*+ 1*/);
        _state = established;
        _handshake2.clear();
        data_type(_handshake2).swap(_handshake2);
        d = 0;
        s = 0;
        if (_databuffer.empty()) return;
      }
    }

    if (_state == established)
    {
      if (_databuffer.empty())
        t.get_aspect().template get<_chunk_splitter_>()(t, d, s);
      else
      {
        t.get_aspect().template get<_chunk_splitter_>()(t,
                       &_databuffer[0], _databuffer.size());
        _databuffer.clear();
        data_type(_databuffer).swap(_databuffer);
      }
    }
  }

private:

  const char* _receive( const char* d, size_t s, bool add,
                        data_type& first, data_type& second )
  {
    const char* result = 0;
    if ( d == 0 )
    {
      if ( !first.empty() && first.size() >= HANDSHAKE_SIZE + 1)
        result = &first[0];
    }
    else if ( first.empty() )
    {
      if ( s < HANDSHAKE_SIZE + add )
        std::copy(d, d + s, std::back_inserter(first) );
      else
      {
        result = d;
        if ( s > HANDSHAKE_SIZE )
          second.assign(d + HANDSHAKE_SIZE + add, d + s);
      }
    }
    else
    {
      if ( first.size() + s >= HANDSHAKE_SIZE + add )
      {
        size_t offset = HANDSHAKE_SIZE + add - first.size();
        std::copy(d, d + offset, std::back_inserter(first) );
        result = &_handshake1[0];
        if (offset < s)
          second.assign(d + offset, d + s);
        else
          second.clear();
      }
      else
        std::copy(d, d + s, std::back_inserter(second) );
    }
    return result;
  }

  data_type _handshake1;
  data_type _handshake2;
  data_type _databuffer;
  server_state _state;

};

class ad_process_first_handshake
{
public:
  template<typename T>
  void operator()(T& t, const char* d, size_t s)
  {
    if ( s!=HANDSHAKE_SIZE+1)
      throw std::runtime_error("ad_process_first_handshake: invalid handshake size");
    t.get_aspect().template get<_write_>()( t, d, HANDSHAKE_SIZE + 1);
    t.get_aspect().template get<_write_>()( t, d + 1, HANDSHAKE_SIZE);
  }
};

class ad_process_second_handshake
{
public:
  template<typename T>
  void operator()(T& t, const char* d, size_t s)
  {
  }
};

}}}

#endif //TR_AMF_SPLITTER_H
