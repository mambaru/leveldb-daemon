#ifndef FAS_MISC_HTTP_READER_HEADER_EQUAL_VALUE_HPP
#define FAS_MISC_HTTP_READER_HEADER_EQUAL_VALUE_HPP

#include "http_parser.hpp"
#include "header_value.hpp"

namespace fas{ namespace http{ namespace reader{

template<typename E>
struct header_equal_value: header_value
{
private:
  E _equal_value;
public:
  bool value;

  header_equal_value(): value() {};

  void clear() 
  {
    value = bool();
    header_value::clear();
  }

  void operator()( const char* beg, const char* end )
  {
    header_value::operator()(beg, end);
    value = ( http_parser::check_value( _equal_value.value(), beg, end) != 0 );
  }
};

}}}

#endif
