//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_UTIL_AMF_AD_CHUNK_COLLECTOR_HPP
#define FAS_UTIL_AMF_AD_CHUNK_COLLECTOR_HPP

#include <vector>
#include <fstream>

#include <fas/util/amf/tags.hpp>
#include <fas/util/amf/types.hpp>

namespace fas{ namespace  util{ namespace amf{


template<typename N>
class ad_chunk_collector
{
public:
  ad_chunk_collector() {}

  template<typename T>
  void clear(T&)
  {
    _object.clear();
  }

  template<typename T>
  void operator()(T& t, const char* d, size_t s)
  {
    size_t obj_id = _collect( d, s );
    if (obj_id!=0xFF)
    {
      t.get_aspect().template get<N>()( t, &_object[obj_id][0], _object[obj_id].size() );
      amf_head_1* ch1 = reinterpret_cast<amf_head_1*>(&_object[obj_id][0]);
      _object[obj_id].resize( ch1->head_size() );
    }
  }

private:

  size_t _collect( const char* d, size_t s )
  {
    const amf_head_1* h1 = reinterpret_cast<const amf_head_1*>(d);
    size_t obj_id = h1->object_id();
    size_t head_size = h1->head_size();
    if ( head_size == 0)
      throw std::out_of_range("ad_amf_chunk_collector::_collect invalid head_size");;

    if (_object.size() <= obj_id )
      _object.resize( obj_id + 1);

    if ( _object[obj_id].empty() )
      _object[obj_id].assign(d, d + s);
    else
    {
      /// 1. Обновляем заголовок
      /// 1.1заголовок текущего объекта
      amf_head_1* ch1 = reinterpret_cast<amf_head_1*>(&_object[obj_id][0]);
      /// 1.2 Если новый больше, то полностью заменяем его
      if ( head_size > ch1->head_size() )
      {
        if ( head_size > _object[obj_id].size() )
          _object[obj_id].assign(d, d + s);
        else
        {
          /// Врят ли сюда попадем
          data_type tmp( d, d + head_size );
          std::copy(
            _object[obj_id].begin() + ch1->head_size(),
            _object[obj_id].end(),
            std::back_inserter(tmp)
          );
          std::copy( d + head_size, d + s, std::back_inserter(tmp) );
          _object[obj_id].clear();
          _object[obj_id].swap(tmp);
        }
      }
      else
      {
        size_t hz = ch1->head_size();
        std::copy( d , d + head_size , _object[obj_id].begin() );
        ch1->head_size( hz );
        std::copy( d + head_size , d + s, std::back_inserter(_object[obj_id]) );
      }
    }

    const amf_head_1* ch1 = reinterpret_cast<const amf_head_1*>(&_object[obj_id][0]);
    bool ok = false;
    if (ch1->head_size() < 8) 
      ok = true;
    else
    {
      const amf_head_8* ch8 = static_cast<const amf_head_8*>(ch1);
      size_t pack_size = ch8->head_size() + ch8->body_size();
      size_t cur_size = _object[obj_id].size();
      if ( cur_size == pack_size )
        ok = true;
      else if ( _object[obj_id].size() > pack_size )
        throw std::out_of_range("ad_amf_chunk_collector");
    }

    return ok ? obj_id : 0xFF;

  }


  typedef std::vector<char> data_type;
  typedef std::vector<data_type> data_object;
  data_object _object;
};

}}}
#endif
