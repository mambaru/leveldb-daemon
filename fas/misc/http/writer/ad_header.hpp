#ifndef FAS_MISC_HTTP_WRITER_AD_HEADER_HPP
#define FAS_MISC_HTTP_WRITER_AD_HEADER_HPP

#include "header_value.hpp"

#include "../tags.hpp"
#include <fas/aop/tag.hpp>
#include "header_value.hpp"

namespace fas{ namespace http{ namespace writer{

namespace aa = ::fas::aop;

template<typename N, typename V = header_value<> >
struct ad_header
  : N, V, aa::gtag<_header_>, aa::gtag<_cleaner_>
{
  int a;
  typedef N name_holder;
  typedef V value_holder;

  ad_header() {};

  template<typename T>
  void clear(T&)
  {
    value_holder::clear();
  }

  template<typename T, typename Itr>
  Itr operator()(T&, Itr itr) const
  {
    if ( !value_holder::has_value() )
       return itr;

    const char* n = name_holder::name();
    for ( ;*n!='\0'; ++n)
      *(++itr) = *n;
    *(++itr) = ':';
    *(++itr) = ' ';
    itr = value_holder::operator()(itr);
    *(++itr) = '\r';
    *(++itr) = '\n';
    return itr;
  }

private:
};

}}}

#endif
