#ifndef FAS_MISC_HTTP_READER_HEADERS_CONTENT_LENGHT_HPP
#define FAS_MISC_HTTP_READER_HEADERS_CONTENT_LENGHT_HPP

//#include "../ad_header.hpp"
#include "../header_int_value.hpp"
#include "../../names.hpp"
//#include "../../tags.hpp"

//#include <fas/aop/advice.hpp>
#include "header.hpp"

namespace fas{ namespace http{ namespace reader{

struct content_length: header< Content_Length, header_int_value > {};
/*
namespace aa = ::fas::aop;

struct content_length_header
  : ad_header<Content_Length, header_int_value>
{
};

struct content_length
  : aa::advice<content_length_header >
{
};
*/


}}}

#endif
