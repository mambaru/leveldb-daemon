#ifndef FAS_MISC_HTTP_READER_AD_INITIAL_HPP
#define FAS_MISC_HTTP_READER_AD_INITIAL_HPP

#include "initial_base.hpp"

namespace fas{ namespace http{ namespace reader{


template<typename H = initial_base>
struct ad_initial
  : H, aa::gtag< _cleaner_ >
{
  typedef H initial_head;

  void operator()( const char* beg, const char* end )
  {
    initial_head::operator ( )( beg, end);
  }

  template<typename T>
  void clear(T&) { initial_head::clear(); }


};

}}}

#endif

