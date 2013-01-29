#ifndef FAS_MISC_HTTP_READER_CLIENT_HEADERS_ASPECT_HPP
#define FAS_MISC_HTTP_READER_CLIENT_HEADERS_ASPECT_HPP

#include <fas/aop/aspect.hpp>

#include "client_initial_header.hpp"

namespace fas{ namespace http{ namespace reader{

typedef ap::type_list_n<
  client_initial_header
>::type client_header_list;

typedef aa::aspect< client_header_list > client_headers_aspect;

}}}


#endif