#ifndef FAS_MISC_HTTP_READER_MULTIPLE_HEADER_HPP
#define FAS_MISC_HTTP_READER_MULTIPLE_HEADER_HPP

#include "header_value.hpp"
#include <deque>

namespace fas{ namespace http{ namespace reader{

template<typename V = header_value>
struct multiple_header: V
{
  typedef V value_type;
  typedef std::deque<V> values_type;
  values_type values;

  void clear()
  {
    values.clear();
    value_type::clear();
  }

  void operator()( const char* beg, const char* end )
  {
    value_type::operator()(beg, end);
    value_type v;
    v(beg, end);
    values.push_front(v);
  }

  size_t size() const { return values.size(); }

  /** Позиция (0 - поледнее пришедшее значение)*/
  value_type operator[](size_t pos) const
  {
    if ( pos >= values.size() )
      return value_type();
    return values[pos];
  }
};


}}}

#endif
