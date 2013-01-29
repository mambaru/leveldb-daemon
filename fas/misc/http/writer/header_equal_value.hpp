#ifndef FAS_MISC_HTTP_WRITER_HEADER_EQUAL_VALUE_HPP
#define FAS_MISC_HTTP_WRITER_HEADER_EQUAL_VALUE_HPP

#include "header_value.hpp"

namespace fas{ namespace http{ namespace writer{

template<typename V>
struct header_equal_value
  : public header_value<>
{
  typedef header_value<> super;
private:
  bool value;

public:

  typedef bool value_type;

  header_equal_value()
  : value()
  {
  }

  value_type get_value() const { return value; }

  const char* get_str_value() const { return super::get_value(); }

  void set_value(value_type v)
  {
    value = v;
    super::set_value( V().value() );
  }

  void clear() 
  {
    value = value_type();
    super::clear();
  }

};


}}}

#endif
