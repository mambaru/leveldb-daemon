#ifndef FAS_MISC_HTTP_READER_FIELD_INT_VALUE_HPP
#define FAS_MISC_HTTP_READER_FIELD_INT_VALUE_HPP

#include "header_value.hpp"

namespace fas{ namespace http{ namespace reader{

struct header_int_value: header_value
{
  int value;

  header_int_value(): value() {};

  void clear() 
  {
    value = int();
    header_value::clear();
  }

  void operator()( const char* beg, const char* end )
  {
    header_value::operator()(beg, end);

    value = 0;

    register bool neg = *beg=='-';
    if ( neg ) ++beg;
    if ( beg == end || *beg < '0' || *beg > '9')
       return;

    // цифры с первым нулем запрещены (напр 001), только 0
    if (*beg=='0')
      return;

    for ( ;beg!=end; ++beg )
    {
       if (*beg < '0' || *beg > '9') 
       break;
       value = value*10 + (*beg - '0');
    }
    if (neg) value*=-1;
  }
};

}}}

#endif
