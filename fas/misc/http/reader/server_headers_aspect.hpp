#ifndef FAS_MISC_HTTP_READER_SERVER_HEADERS_ASPECT_HPP
#define FAS_MISC_HTTP_READER_SERVER_HEADERS_ASPECT_HPP

#include <fas/aop/aspect.hpp>

#include "server_initial_header.hpp"

namespace fas{ namespace http{ namespace reader{

typedef ap::type_list_n<
  server_initial_header
>::type server_header_list;

typedef aa::aspect< server_header_list > server_headers_aspect;

}}}


#endif