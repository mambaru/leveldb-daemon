#ifndef FAS_MISC_HTTP_WRITER_AD_INITIAL_HPP
#define FAS_MISC_HTTP_WRITER_AD_INITIAL_HPP

#include "initial_base.hpp"
#include "header_value.hpp"

namespace fas{ namespace http{ namespace writer{


template<typename H = initial_base<header_value<>, header_value<>, header_value<> > >
struct ad_initial
  : H, aa::gtag< _cleaner_ >
{
  typedef H initial_head;

  template<typename Itr>
  Itr operator()( Itr itr )
  {
    return initial_head::operator ( )( itr );
  }

  template<typename T>
  void clear(T&) { initial_head::clear(); }


};

}}}

#endif

