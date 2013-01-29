#ifndef FAS_MISC_HTTP_AD_HTTP_READER_HPP
#define FAS_MISC_HTTP_AD_HTTP_READER_HPP

#include <fas/pattern/type_list.hpp>
#include <vector>
#include <algorithm>

#include "tags.hpp"
#include "reader/headers/content_length.hpp"
#include "reader/headers/transfer_encoding.hpp"

namespace fas{ namespace http {

namespace ap = ::fas::pattern;

template<typename H /* = reader::server_head_reader */, typename B/*  = reader::body_reader*/ >
class ad_reader
{
  typedef std::vector<char> data_type;
  typedef H head_reader_type;
  typedef B body_reader_type;
public:

  ad_reader()
    : _head_beg(0), _head_end(0)//, _body_beg(0), _body_end(0)
    , _reader_lock(false)
    , _reader_clear(false) 
  {}

  template<typename T>
  void clear(T&) 
  {
    if ( !_reader_lock  )
      this->reset();
    else
      _reader_clear = true;
  }

  void reset()
  {
    _head_beg = 0; _head_end = 0; //_body_beg = 0; _body_end = 0;
    _head_data.clear();
    _body_data.clear();
    _head_reader.clear();
    _body_reader.clear();
    _reader_clear = false;
    //_reader_lock = false;
  }

  template<typename T>
  void operator()(T& t, const char* beg, size_t s)
  {
    _lock_reader();
    const char* end = beg + s;
    while ( beg!=0 && beg!=end)
    {
      beg = _read_head(t, beg, end);
      if (beg!=0)
        beg = _read_body(t, beg, end);
      if ( _body_reader ) this->reset();
    }
    _unlock_reader();
  }

private:

  template<typename T>
  const char* _read_head(T& t, const char* beg, const char* end)
  {
    // заголовок уже готов, читем тело
    if ( _head_reader ) return beg;

    _head_beg = beg;
    _head_end = end;

    data_type::size_type pred_head_size = _head_data.size();
    if ( pred_head_size != 0 )
    {
      if ( beg != end )
      {
        _head_data.reserve( pred_head_size + std::distance( beg,  end) );
        std::copy( beg, end, std::back_inserter(_head_data) );
      }
      if ( !_head_data.empty() )
      {
        _head_beg = &(_head_data[0]);
        _head_end = _head_beg + _head_data.size();
      }
    }

    const char *head_cur = _head_reader(t, _head_beg, _head_end);

    data_type::size_type head_size = 0;
    if ( head_cur == 0 ) 
    {
      // Если заголовок не готов
      if ( _head_data.empty() )
      {
        // если _head_data не пустой то данные уже туда скопированны
        _head_data.reserve( std::distance( beg,  end) );
        std::copy( beg, end, std::back_inserter(_head_data) );
      }
      return 0;
    }
    else
    {
      head_size = std::distance(_head_beg, head_cur);

      if ( !_head_data.empty() )
      {
        // Если размер буффера слишком велик, то перераспределяем его и парсим заного
        // _head_data.resize(head_size);
      }
    }

    t.get_aspect().template get<_head_readed_>()(t, _head_reader);

    // Определяем сколько байт из пришедших данных были от заголовка
    data_type::size_type head_read_size = head_size - pred_head_size;

    return beg + head_read_size;
  }

  template<typename T>
  const char* _read_body(T& t, const char* beg, const char* end)
  {
    // Заголовок не готов
    if ( !_head_reader ) return beg;

    // Тело готово (необходимо сбросить) TODO: ошибка логики
    if ( _body_reader ) return beg;

    const char* body_beg = beg;
    const char* body_end = end;

    /// Устанавливаем аттрибуты необходимы для корректного чтения
    this->_set_content_length( _head_reader.get_aspect().template get<Content_Length>() );
    this->_set_chunked( _head_reader.get_aspect().template get<Transfer_Encoding>() );

    /// Определяем размер буферизированных данных
    data_type::size_type pred_body_size = _body_data.size();

    if ( pred_body_size != 0 )
    {
      /// Если буфер тела пакета не пустой, то тупо копируем туда все
      if (beg!=end)
      {
        _body_data.reserve( pred_body_size + std::distance( beg,  end) );
        std::copy( beg, end, std::back_inserter(_body_data) );
      }
      if ( !_body_data.empty() )
      {
        body_beg = &(_body_data[0]);
        body_end = body_beg + _body_data.size();
      }
    }

    const char *body_next = body_beg;
    /// Пока тело не будет полностью прочитано
    while ( !_body_reader )
    {
      const char *body_cur = _body_reader(t, body_next, body_end);
      if ( body_cur == 0 ) 
      {
        if ( !_body_data.empty() )
        {
          // Все данные в буфере, срезаем обработанную часть
          if ( body_beg != body_next )
          {
            data_type::iterator itr_beg =  _body_data.begin() + std::distance( static_cast<const char*>(&_body_data[0]),  body_beg);
            data_type::iterator itr_end = _body_data.begin() + std::distance(  static_cast<const char*>(&_body_data[0]),  body_next);
            _body_data.erase(itr_beg, itr_end);
          }
        }
        else
        {
          // Не оконченный блок
          _body_data.reserve( std::distance( body_next,  body_end) );
          std::copy(body_next, body_end, std::back_inserter(_body_data) );
        }

        /// Если тело не готово и в буфере заголовка нет данных, то копируем их туда и парсим заного 
        /// (т.е. попытка обработки без буферизации не прошла)
        if ( _head_data.empty() && _head_beg!=0 && _head_beg!=_head_end)
        {
           _head_data.assign(_head_beg, _head_end );
           _head_beg = &(_head_data[0]);
           _head_end = _head_beg + _head_data.size();
           _head_reader.clear();
           _head_reader(t, _head_beg, _head_end);
        }
        return 0;
      };

/// #warning МЕГА АХТУНГ перед выходными - Если во время обработки соединение закрывается то писец
/// заблокировал
      t.get_aspect().template get<_body_readed_>()( t, _head_reader, _body_reader );
      body_next = body_cur;
    }

    data_type::size_type body_size = std::distance(body_beg, body_next);

    // Определяем сколько байт из пришедших данных были от заголовка
    data_type::size_type body_read_size = body_size - pred_body_size;
    return beg + body_read_size;
  }

  template<typename ADV>
  void _set_content_length(const ADV& cl)
  {
    if ( cl.has_value() )
      _body_reader.set_content_length( cl.value );
    else
      _body_reader.set_content_length( -1 );
  }

  void _set_content_length(ap::empty_type)
  {
    _body_reader.set_content_length( -1 );
  }

  template<typename ADV>
  void _set_chunked(const ADV& cl)
  {
    
    if ( cl.has_value() )
    {
      _body_reader.set_chunked( cl.value );
    }
    else
    {
      _body_reader.set_chunked( false );
    }
  }

  void _set_chunked(ap::empty_type)
  {
    _body_reader.set_chunked( false );
  }

private:

  void _lock_reader() 
  {
     if ( _reader_clear )
       this->reset();

     _reader_lock = true;
     _reader_clear = false;
  }

  void _unlock_reader()
  {
    if ( _reader_clear )
      this->reset();
    _reader_lock = false;
    _reader_clear = false;
  }
  data_type _head_data;
  data_type _body_data;
  head_reader_type _head_reader;
  body_reader_type _body_reader;

  const char* _head_beg;
  const char* _head_end;
/*
  const char* _body_beg;
  const char* _body_end;
*/

  bool _reader_lock;
  bool _reader_clear;

};

}}

#endif
