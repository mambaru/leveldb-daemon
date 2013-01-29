#ifndef FAS_MISC_HTTP_READER_CLIENT_INITIAL_HEADER_HPP
#define FAS_MISC_HTTP_READER_CLIENT_INITIAL_HEADER_HPP

#include "../tags.hpp"
#include "ad_initial.hpp"
#include "initial_response.hpp"

namespace fas{ namespace http{ namespace reader{

struct client_initial_header
  : aa::advice< _initial_, ad_initial<initial_response> >
{

};

}}}

#endif

