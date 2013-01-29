#ifndef FAS_MISC_HTTP_WRITER_SERVER_HEADER_WRITER_HPP
#define FAS_MISC_HTTP_WRITER_SERVER_HEADER_WRITER_HPP

#include <fas/aop/aspect.hpp>
#include "header_writer.hpp"
#include "server_headers_aspect.hpp"

namespace fas{ namespace http{ namespace writer{

namespace aa = ::fas::aop;

template< typename A = aa::aspect<> >
class server_header_writer
  : public header_writer<aa::aspect_merge<header_writer_aspect, server_headers_aspect>::type, A >
{
};

}}}

#endif
