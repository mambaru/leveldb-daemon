#ifndef FAS_MISC_HTTP_WRITER_CLIENT_HEADER_READER_HPP
#define FAS_MISC_HTTP_WRITER_CLIENT_HEADER_READER_HPP

#include <fas/aop/aspect.hpp>
#include "header_writer.hpp"
#include "client_headers_aspect.hpp"

namespace fas{ namespace http{ namespace writer{

namespace aa = ::fas::aop;

template< typename A = aa::aspect<> >
class client_header_writer
  : public header_writer<A, aa::aspect_merge<header_writer_aspect, client_headers_aspect>::type >
{
};

}}}

#endif
