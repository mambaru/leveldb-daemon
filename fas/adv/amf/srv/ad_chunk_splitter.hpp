//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//


#ifndef FAS_ADV_AMF_SRV_AD_CHUNK_SPLITTER_HPP
#define FAS_ADV_AMF_SRV_AD_CHUNK_SPLITTER_HPP

#include <vector>

#include <fas/adv/amf/srv/tags.hpp>
#include <fas/adv/amf/types.hpp>


namespace fas{ namespace  adv{ namespace amf{ namespace srv{

namespace ada = ::fas::adv::amf;

class chunk
{
  typedef enum { 
    _first_chunk_, 
    _next_chunk_, 
    _read_head_, 
    _skip_head_, 
    _read_chunk_, 
    _ready_chunk_,
    _ready_body_
  } state_chunk;

  typedef std::vector<char> data_type;

public:
  chunk()
    : _state(_first_chunk_)
    , _chunk_size(0)
    , _current_head_read(0)
    , _current_body_read(0) 
  {}

  void clear()
  { 
    data_type().swap(_data); 
    _state = _first_chunk_;
    _chunk_size = 0; 
    _current_head_read = 0;
    _current_body_read = 0;
  }

  const char* data() const { return &_data[0];}
  size_t size() const { return _data.size();}

  void reset(  )
  {
    if (_state == _ready_body_)
      _current_body_read = 0;

    _state = _next_chunk_;
    _data.resize(12);
    _current_head_read = 0;

  }

  bool ready() const { return _state==_ready_chunk_ || _ready_body_; }

  size_t append(const char* d, size_t s)
  {
    size_t offset = 0;
    while (  offset < s)
    {
      switch (_state)
      {
        case _first_chunk_: offset+=_first_chunk(d + offset, s - offset); break;
        case _next_chunk_: offset+=_next_chunk(d + offset, s - offset);break;
        case _read_head_: offset+=_read_head(d + offset, s - offset);break;
        case _skip_head_: offset+=_skip_head(d + offset, s - offset);break;
        case _read_chunk_: offset+=_read_chunk(d + offset, s - offset);break; 
        case _ready_chunk_: offset+=_ready_chunk(d + offset, s - offset);break;
        case _ready_body_: offset+=_ready_body(d + offset, s - offset);break;
      }

      if ( _state >= _ready_chunk_)
        break;
    }
    return offset;
  }

private:
  size_t _first_chunk(const char* d, size_t /*s*/)
  {
    _data.reserve(128);
    _data.resize(12, 0);
    _state = _read_head_;
    const ada::amf_head_1* h1_ = reinterpret_cast<const ada::amf_head_1*>(d);
    _current_head_size = h1_->head_size();
    _current_head_read = 0;
    return 0;
  }

  size_t _next_chunk(const char* d, size_t /*s*/)
  {
    _data.resize(12);
    const ada::amf_head_1* h1 = reinterpret_cast<const ada::amf_head_1*>(&_data[0]);
    const ada::amf_head_1* h1_ = reinterpret_cast<const ada::amf_head_1*>(d);
    _current_head_size = h1_->head_size();
    _current_head_read = 0;

    
    if ( h1_->head_size() >= h1->head_size() )
      _state = _read_head_;
    else
      _state = _skip_head_;
    return 0;
  }

  size_t _skip_head(const char* /*d*/, size_t s)
  {
    
    if ( _current_head_size - _current_head_read > s )
    {
      _current_head_read += s;
      return s;
    }
    _current_head_read = _current_head_size;
    _state = _read_chunk_;
    return _current_head_size;
    
  }

  size_t _read_head(const char* d, size_t s)
  {
    size_t offset = 0;
    while (_current_head_size != _current_head_read && offset < s)
      _data[_current_head_read++] = d[offset++];
    if (_current_head_size == _current_head_read)
    {
      _state = _read_chunk_;

    if (_chunk_size == 0)
    {
      const ada::amf_head_8* h8 = reinterpret_cast<const ada::amf_head_8*>(&_data[0]);
      _chunk_size = h8->content_type()!=0x08 ? 128 : 64;
    }
    }

    return offset;
  }
  
  size_t _read_chunk(const char* d, size_t s)
  {
    const ada::amf_head_8* h8 = reinterpret_cast<const ada::amf_head_8*>(&_data[0]);
    size_t body_size = h8->body_size();

    size_t body_need = body_size - _current_body_read;
    size_t need = body_need > _chunk_size ? _chunk_size : body_need;
    size_t have = need > s ? s : need;
    std::copy(d, d + have, std::back_inserter(_data));
    _current_body_read += have;

    if ( _data.size() - 12 == need )
      _state = _ready_chunk_;

    if ( _current_body_read == body_size )
      _state = _ready_body_;

    return have;
  }
  
  size_t _ready_chunk(const char* /*d*/, size_t /*s*/)
  {
    // _current_head_size = 0;
    // _current_head_read = 0;
    _state = _next_chunk_;
    return 0;
  }
  
  size_t _ready_body(const char* /*d*/, size_t /*s*/)
  {
    _state = _next_chunk_;
    _current_body_read = 0;
    return 0;
  }

private:
  data_type _data;
  state_chunk _state;
  size_t _chunk_size;
  size_t _current_head_size;
  size_t _current_head_read;
  size_t _current_body_read;
  // size_t _current_body_size;


};
/*
class chunk2
{
  
public:
  typedef std::vector<char> data_type;
  chunk2():_current_body_size(0){}
  void clear(){ data_type().swap(_data); _chunk_size = 128; _current_body_size = 0;}
  void reset() 
  {
    // _current_body_size = 0;
    ada::amf_head_1* h1 = reinterpret_cast<ada::amf_head_1*>(&_data[0]);
    _data.resize( h1->head_size() );
  }
  size_t size() const { return _data.size(); }
  const char* data() const { return &_data[0];}

  bool ready() const 
  {
    if ( _data.empty() )
      return false;

    const ada::amf_head_1* h1 = reinterpret_cast<const ada::amf_head_1*>(&_data[0]);
    size_t need = h1->head_size() + _chunk_size;
    if ( _data.size() > need)
      throw;
    return _data.size() == need;
  }

  /// @return >0 - то остаток
  size_t append(const char* d, size_t s)
  {
    // 1 - если данных нет, то копируем заголовок
    // 2 - если данные урезаны до заголовка, то обновляем заголовок
    // 3 - добавляем данные, до полного chunk'a
    const ada::amf_head_1* h1 = 0;
    const ada::amf_head_8* h8 = 0;
    size_t head_size = 0;
    if (_data.empty())
    {
      h1 = reinterpret_cast<const ada::amf_head_1*>(d);
      head_size = h1->head_size();

      _data.resize(12, 0);
      
      // Исправить
      if ( head_size > s)
      {
        std::copy(d, d + s, _data.begin() );
        return 0;
      }

      std::copy(d, d + head_size, _data.begin() );

      h1 = reinterpret_cast<const ada::amf_head_1*>(&_data[0]);
      d += head_size;
      s -= head_size;
      if (head_size >= 8)
      {
        h8 = reinterpret_cast<const ada::amf_head_8*>(h1);
        _chunk_size = h8->content_type()!=0x08 ? 128 : 64;
      }
    }
    else
    {
      h1 = reinterpret_cast<const ada::amf_head_1*>(&_data[0]);
      h8 = reinterpret_cast<const ada::amf_head_8*>(h1);
      head_size = h1->head_size();
      if ( head_size < _data.size() )
      {
        // TODO:
      }

      if ( head_size == _data.size() )
      {
        // Проверяем новый заголовок
        const ada::amf_head_1* h1_ = reinterpret_cast<const ada::amf_head_1*>(d);
        size_t head_size_ = h1_->head_size();
        if (h1_->head_size() >= head_size)
        {
          head_size = head_size_;
          _data.resize(head_size_);
        }
        std::copy(d, d + head_size_, _data.begin());
        h1 = reinterpret_cast<const ada::amf_head_1*>(&_data[0]);
        head_size = h1->head_size();
        d += head_size_;
        s -= head_size_;
      }
    }

    size_t next = h8->body_size() + head_size - _current_body_size;
    if (next > _chunk_size)
      next = _chunk_size;

    size_t need = next + head_size - _data.size();

    if (need > s)
    { 
      _current_body_size +=s;
      std::copy( d, d + s, std::back_inserter(_data));
      return 0;
    }

    if ( _current_body_size + need == h8->body_size() )
      _current_body_size = 0;
    else
      _current_body_size += need;

     std::copy( d, d + need, std::back_inserter(_data));
     return s - need ;
  }

private:
  size_t _chunk_size;
  size_t _current_body_size;
  data_type _data;
};


*/
/** Разбивает входной поток на chunk'и и восстанавливает их */
class ad_chunk_splitter
{
public:

  ad_chunk_splitter():_last_flag(false) {}

  template<typename T>
  void operator()(T& t, const char* d, size_t s)
  {
    /*
    size_t i = 0;
    while(i<s)
    {
      int j = 0;
      while (j++<12)
        ;

    }*/

    size_t offset = 0;
    if ( _last_flag  )
    {
      offset = _chunk_objects[_last_object].append(d, s);
      if (_chunk_objects.size() <= _last_object)
        _chunk_objects.resize(_last_object + 1);
      if (!_chunk_objects[_last_object].ready())
        return;

      _last_flag = false;
      t.get_aspect().template get<_content_>()(t, _chunk_objects[_last_object].data(), _chunk_objects[_last_object].size());
      // отправить
    }

    if (offset == s)
      return;

    while (!_last_flag && offset!=s)
    {
      const ada::amf_head_1* h1 = reinterpret_cast<const ada::amf_head_1*>(d + offset);
      _last_object = h1->object_id();
      if (_chunk_objects.size() <= _last_object)
      {
        _chunk_objects.resize(_last_object + 1);
      }

      offset += _chunk_objects[_last_object].append(d + offset, s - offset);
      if (!_chunk_objects[_last_object].ready())
        _last_flag = true;
      else
      {
        t.get_aspect().template get<_content_>()(t, _chunk_objects[_last_object].data(), _chunk_objects[_last_object].size());
        // отправить
        _chunk_objects[_last_object].reset();
      }
    }
  }


  template<typename T>
  void clear(T&)
  {
    _last_object = 0;
    _last_flag = false;
    _chunk_objects.clear();
  }

private:
  typedef std::vector<chunk> chunk_objects;
  chunk_objects _chunk_objects;
  size_t _last_object;
  bool _last_flag;
};


}}}}

#endif
