#ifndef FAS_MISC_HTTP_WRITER_HEADERS_CONTENT_LENGHT_HPP
#define FAS_MISC_HTTP_WRITER_HEADERS_CONTENT_LENGHT_HPP

#include "../header_int_value.hpp"
#include "../../names.hpp"
#include "header.hpp"

namespace fas{ namespace http{ namespace writer{

struct content_length: header< Content_Length, header_int_value > {};

}}}

#endif
