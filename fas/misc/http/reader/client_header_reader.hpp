#ifndef FAS_MISC_HTTP_READER_CLIENT_HEADER_READER_HPP
#define FAS_MISC_HTTP_READER_CLIENT_HEADER_READER_HPP

#include <fas/aop/aspect.hpp>
#include "header_reader.hpp"
#include "client_headers_aspect.hpp"

namespace fas{ namespace http{ namespace reader{

namespace aa = ::fas::aop;

template< typename A = aa::aspect<> >
class client_header_reader
  : public header_reader<A, aa::aspect_merge<header_reader_aspect, client_headers_aspect>::type >
{
};

}}}

#endif
