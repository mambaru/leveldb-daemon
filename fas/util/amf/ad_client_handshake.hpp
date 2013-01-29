//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_UTIL_AMF_AD_CLIENT_HANDSHAKE_HPP
#define FAS_UTIL_AMF_AD_CLIENT_HANDSHAKE_HPP

#include <vector>

#include <fas/util/amf/types.hpp>
#include <fas/util/amf/tags.hpp>

namespace fas{ namespace  util{ namespace amf{


class ad_client_handshake
{
  typedef std::vector<char> data_type;
public:

  /// on_connect
  template<typename T>
  void operator()(T& t, bool cr)
  {
    if (cr)
    {
      data_type d;
      d.resize(HANDSHAKE_SIZE + 1);
      d[0]=0x3;
      t.get_aspect().template get<_write_>()( t, &(d[0]), d.size());
    }
  }

  template<typename T>
  void operator()(T& t, char* d, size_t s)
  {
    if (s!=1536)
      throw std::runtime_error("invalid client handshake size for server");
    //char head = 0x03;
    //t.get_aspect().template get<_write_>()( t, &head, 1);
    t.get_aspect().template get<_write_>()( t, d, s);
  }
};

class ad_process_client_handshake
{
  enum client_state { wait_handshake, established};
public:
  typedef std::vector<char> data_type;

  ad_process_client_handshake()
    : _state(wait_handshake)
  {
  }

  ad_process_client_handshake( const ad_process_client_handshake&)
    : _state(wait_handshake)
  {
  }

  template<typename T>
  void operator()(T& t, const char* d, size_t s)
  {
    if ( _state == wait_handshake)
    {
      std::copy(d, d + s, std::back_inserter(_handshake) );
      if ( _handshake.size() >= HANDSHAKE_SIZE*2 + 1)
      {
        _state = established;
        t.get_aspect().template get<_client_handshake_>()
          (t, &(_handshake[HANDSHAKE_SIZE+1]), HANDSHAKE_SIZE);
        _handshake.erase(_handshake.begin(), _handshake.begin() + HANDSHAKE_SIZE*2 + 1  );
      }
    }

    if ( _handshake.empty() )
      t.get_aspect().template get<_chunk_splitter_>()(t, d, s);
    else
    {
      t.get_aspect().template get<_chunk_splitter_>()(t, &(_handshake[0]), _handshake.size());
      data_type().swap(_handshake);
    }
  }

  template<typename T>
  void clear(T&)
  {
    _handshake.clear();
  }

public:
  data_type _handshake;
  client_state _state;
};

}}}

#endif //TR_AMF_SPLITTER_H
