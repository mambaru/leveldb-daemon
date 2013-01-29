#ifndef FAS_MISC_HTTP_READER_HEADER_READER_HPP
#define FAS_MISC_HTTP_READER_HEADER_READER_HPP

#include <fas/aop/aspect.hpp>
#include "header_reader_aspect.hpp"

namespace fas{ namespace http{ namespace reader{

namespace aa = ::fas::aop;

template<typename U = aa::aspect<>, typename A = header_reader_aspect >
class header_reader
  : public aa::aspect_class<A, U>
{
  typedef aa::aspect_class<A, U> super;
  typedef typename super::aspect aspect;

  typedef typename super::template select<_header_>::type fields_list;

  bool _ready;
public:

  typedef typename aa::advice_cast< _initial_, aspect >::type initial_type;

  header_reader(): _ready(false) {}

  void clear() 
  {
    _ready = false;
    this->get_aspect().template get< _make_clean_ >()(*this); 
  }

  operator bool () const { return _ready;}

/** this->get_aspect().template get<_initial_>()(beg, vend); */

  const initial_type& initial() const
  {
    return this->get_aspect().template get<_initial_>();
  }

  /** return 0 - неоконченный заголовок
             -   заголовок распарсен*/
  template<typename T>
  const char* operator()(T&, const char* beg, const char* end)
  {
    if (_ready) return beg;
    
    const char* cur = http_parser::parse_end_head(beg, end);
    if (cur!=0) return cur;

    cur = _parse_initial( beg, end);
    if (cur == 0 ) return 0;
    beg = cur;

    cur = http_parser::parse_end_head(beg, end);
    if (cur!=0)
    {
      _ready = true;
      return cur;
    }

    while ( beg!=end )
    {
      cur = _parse_field(fields_list(), beg, end);
      if ( cur == 0 ) return 0;

      /// Если обработчика поля не найдено 
      if ( cur == beg )
        cur = http_parser::parse_header( beg, end);

      // если не распарсился то обрезанный заголовок (это ошибка)
      // если ошибка то выбрасывается исключение обработчиком полей
      if ( cur == 0 )
        return 0;
      beg = cur;

      cur = http_parser::parse_end_head(beg, end);
      if (cur!=0) 
      {
        _ready = true;
        return cur;
      }
    }
    return 0;
  }

private:

  const char* _parse_initial( const char* beg, const char* end )
  {
    const char* cur = http_parser::parse_initial(beg, end);
    if (cur == 0 ) return 0;
    if ( (beg > cur) || ( (cur - beg) < 3 ) ) return cur;
    const char *vend = cur - 2;

    this->get_aspect().template get<_initial_>()(beg, vend);
    return cur;
  }

  template<typename L>
  const char* _parse_field(L, const char* beg, const char* end)
  {
    typedef typename L::left_type advice_type;
    typedef typename L::right_type list_type;
    beg = this->get_aspect().template get< advice_type >()( *this, beg, end);
    if (beg!=0)
      beg = _parse_field(list_type(), beg, end);
    return beg;
  }

  const char* _parse_field(ap::empty_type, const char* beg, const char*  )
  {
    return beg;
  }
};


}}}

#endif
