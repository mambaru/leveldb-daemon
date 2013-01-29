//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_ADV_AMF_SRV_STATE1_HPP
#define FAS_ADV_AMF_SRV_STATE1_HPP

#include <vector>
#include <iostream>

#include <fas/adv/amf/srv/tags.hpp>

// #include <fas/amf/types.hpp>

namespace fas{ namespace  adv{ namespace amf{ namespace srv{

class ad_state1
{
  typedef std::vector<char> data_type;
public:
  template<typename T>
  void activate(T&)
  {
    _head_size = 0;
    _handshake.clear();
  }

  template<typename T>
  void operator()(T& t, const char* d, size_t s)
  {
    if (s == 0)
      return;
    if (_handshake.empty())
    {
       //const adas::amf_head_1* h1 = reinterpret_cast<const adas::amf_head_1*>(d);
      _head_size = 1;//h1->head_size();
    }
    size_t need = (1536 + _head_size) - _handshake.size();
//    size_t tail = 0;
    if ( s >= need )
    {
      if ( !_handshake.empty() )
      {
        std::copy(d, d + need, std::back_inserter(_handshake));
        t.get_aspect().template get<_writer_>()(t, 
          &_handshake[0], static_cast<typename T::write_size_type>(_handshake.size()));
        t.get_aspect().template get<_writer_>()(t, 
          &_handshake[_head_size], 
          static_cast<typename T::write_size_type>(_handshake.size() - _head_size));
      }
      else
      {
        t.get_aspect().template get<_writer_>()(t, d, 
          static_cast<typename T::write_size_type>(1536 + _head_size) );
        t.get_aspect().template get<_writer_>()(t, d + _head_size, 
          static_cast<typename T::write_size_type>(1536) );
      }
      _handshake.clear();

      t.get_aspect().template get<_state_>().template state<_state2_>(t)( t, d + need, s - need);
    }
    else if ( _handshake.empty() )
    {
      _handshake.assign(d, d + s);
    }
    else
    {
      std::copy(d, d + s, std::back_inserter(_handshake));
    }
  } 
private:

  data_type _handshake;
  size_t _head_size;
};


}}}}

#endif //TR_AMF_SPLITTER_H
