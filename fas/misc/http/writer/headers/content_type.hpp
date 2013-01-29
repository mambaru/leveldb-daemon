#ifndef FAS_MISC_HTTP_WRITER_HEADERS_CONTENT_TYPE_HPP
#define FAS_MISC_HTTP_WRITER_HEADERS_CONTENT_TYPE_HPP

#include "../header_int_value.hpp"
#include "../../names.hpp"
#include "header.hpp"

namespace fas{ namespace http{ namespace writer{

struct content_type: header< Content_Type, header_value<application_xhtml_xml> > {};

}}}

#endif
