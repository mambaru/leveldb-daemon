#ifndef FAS_MISC_HTTP_READER_HEADERS_HEADER_HPP
#define FAS_MISC_HTTP_READER_HEADERS_HEADER_HPP

#include "../ad_header.hpp"
#include "../header_value.hpp"
#include "../../names.hpp"

#include <fas/aop/advice.hpp>

namespace fas{ namespace http{ namespace reader{

namespace aa = ::fas::aop;

template<typename N, typename V = header_value>
struct header: aa::advice< ad_header<N, V> >
{
};


}}}

#endif
