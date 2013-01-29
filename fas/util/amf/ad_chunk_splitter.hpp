//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//


#ifndef FAS_UTIL_AMF_AD_CHUNK_SPLITTER_HPP
#define FAS_UTIL_AMF_AD_CHUNK_SPLITTER_HPP

#include <vector>

#include <fas/util/amf/tags.hpp>

namespace fas{ namespace  util{ namespace amf{


/** Разбивает входной поток на chunk'и */
class ad_chunk_splitter
{
  struct amf_object
  {
    uint8_t chunk_size;
    uint8_t head_size;
    uint8_t content;
    uint32_t body_size;
    /// Сколько осталось 
    uint32_t body_recv;
    amf_object()
      : chunk_size(0), head_size(0)
      , content(0), body_size(0), body_recv(0)
    {}
  };

  typedef std::vector<char> data_type;
public:

  ad_chunk_splitter() {}

  template<typename T>
  void operator()(T& t, const char* d, size_t s)
  {
    const char* beg = d;

    if ( !_buffer.empty() )
    {
      _buffer.reserve(_buffer.size() + s);
      std::copy( d, d + s, std::back_inserter(_buffer) );
      beg = &_buffer[0];
    }

    const char* chunk = beg;
    size_t size = s - std::distance(beg, chunk);

    //try
    {
      while ( size!=0 && _split(chunk, &size) )
      {
        const amf_head_1* h1 = reinterpret_cast<const amf_head_1*>(chunk);
        uint8_t content = _object[ h1->object_id() ].content;
        t.get_aspect().template get<_process_content_>()( t, content, chunk, size );
        chunk += size;
        size = s - std::distance(beg, chunk);
      }
    }
    /*catch(...)
    {
      /// если изменяеться размер chunk
      t.release(false);
      throw;
    }*/

    if ( _buffer.empty() )
    {
      if ( size!=0 )
        _buffer.assign( chunk, chunk + size );
      else if ( chunk > d + s )
        throw std::out_of_range("ad_amf_chunk_splitter");
    }
    else
    {
      if ( size == 0 )
        _buffer.clear();
      else
        _buffer.erase( _buffer.begin(),
                       _buffer.begin() + std::distance(beg, chunk) );
    }
  }

  /// Определяет достаточно ли данных для пеередачи одного chunk
  bool _split(const char* chunk, size_t* size)
  {
    const char* d = chunk;
    size_t s = *size;

    const amf_head_1* h1 = reinterpret_cast<const amf_head_1*>(d);

    if (h1->head_size() > s)
      return false;

    unsigned int obj_id = h1->object_id();
    if (_object.size() <= obj_id )
      _object.resize( obj_id + 1);

    amf_object& obj = _object[obj_id];

    obj.head_size = h1->head_size();
    if ( h1->head_size( ) >= 8 )
    {
      const amf_head_8* h8 = static_cast<const amf_head_8*>(h1);
      obj.body_size = h8->body_size();
      obj.content = h8->content_type();
      obj.chunk_size = obj.content!=0x08 ? 128 : 64;
      /// TODO: для obj.content == 0x01 изменение размера chunk
      if (obj.content == 1)
        throw std::out_of_range("ad_amf_chunk_splitter: change chunk not support");
    }

    if ( obj.body_recv == 0 )
      obj.body_recv = obj.body_size;

    size_t need = obj.chunk_size + obj.head_size;
    if (need > obj.body_recv + obj.head_size )
      need = obj.body_recv + obj.head_size;

    if ( s < need )
      return false;

    obj.body_recv -= need - obj.head_size;

    *size = need;

    return true;
  }


  template<typename T>
  void clear(T&)
  {
    _object.clear();
    _buffer.clear();
  }


private:
  typedef std::vector<amf_object> amf_objects;
  amf_objects _object;
  data_type _buffer;

};

}}}
#endif
