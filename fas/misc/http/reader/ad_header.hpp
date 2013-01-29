#ifndef FAS_MISC_HTTP_READER_AD_HEADER_HPP
#define FAS_MISC_HTTP_READER_AD_HEADER_HPP

#include "http_parser.hpp"
#include "header_value.hpp"

#include "../tags.hpp"
#include <fas/aop/tag.hpp>

namespace fas{ namespace http{ namespace reader{

namespace aa = ::fas::aop;
template<typename N, typename V = header_value>
struct ad_header
  : N, V, aa::gtag<_header_>, aa::gtag<_cleaner_>
{
  typedef N super;
  typedef V value_holder;
  const char *data_beg;
  const char *data_end;

  ad_header(): data_beg(), data_end() {};

  template<typename T>
  void clear(T&)
  {
    value_holder::clear();
    data_beg = 0;
    data_end = 0;
  }


  /**
    * return указатель на конец поля (если обработанно (совпало имя) )
             или beg если не обработанно (не совпало имя)
             или 0 если не найденно окончание поля
    */
  template<typename T>
  const char* operator()(T&, const char* beg, const char* end)
  {
    // Проверяем имя
    const char* nend = http_parser::check_name( super::name(), beg, end);
    // Если не совпадает, то нет смысла парсить далее
    if ( nend==0 ) return beg;

    // Находим окончание поля заголовка
    const char* fend = http_parser::parse_header(beg, end);

    // Окончание не найдено
    if ( fend == 0 ) return 0;

    // Пердпологается что за любым полем есть еще данные (как минимум \r\n)
    // Т.е. заголовок должен быть полностю буфферизирован
    // if ( fend == end ) return beg;
    const char *vbegin = http_parser::parse_space(nend, fend);
    if ( vbegin == 0 || *vbegin!=':')
      return fend;
    ++vbegin;
    vbegin = http_parser::parse_space(vbegin, fend);
    if (vbegin == 0) 
       return fend;
    if ( (vbegin > fend) || ( (fend - vbegin) < 3 ) ) 
       return fend;
    const char *vend = fend - 2;

    value_holder::operator()(vbegin, vend);

    return fend;
  }

private:
};

}}}

#endif
