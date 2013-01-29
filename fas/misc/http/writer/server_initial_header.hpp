#ifndef FAS_MISC_HTTP_WRITER_SERVER_INITIAL_HEADER_HPP
#define FAS_MISC_HTTP_WRITER_SERVER_INITIAL_HEADER_HPP

#include "../tags.hpp"
#include "ad_initial.hpp"
// #include "initial_base.hpp"
#include "initial_response.hpp"
#include <fas/aop/advice.hpp>

namespace fas{ namespace http{ namespace writer{

namespace aa = ::fas::aop;

/**  */
struct server_initial_header: aa::advice< _initial_, ad_initial<initial_response> > {};

}}}

#endif

