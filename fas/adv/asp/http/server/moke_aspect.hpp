//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2010
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_ADV_ASP_HTTP_SERVER_MOKE_ASPECT_HPP
#define FAS_ADV_ASP_HTTP_SERVER_MOKE_ASPECT_HPP

#include "aspect.hpp"

#include <fas/misc/http/reader/server_header_reader.hpp>
#include <fas/misc/http/reader/body_reader.hpp>

namespace fas{ namespace adv{ namespace asp{ namespace http{ namespace server{

namespace aa = ::fas::aop;
namespace ap = ::fas::pattern;

struct moke_server_reader: ah::ad_reader< ahr::server_header_reader<>, ahr::body_reader<> > {};

struct moke_server_writer: ad_writer< server_header_writer<>, body_writer<> > {};

struct moke_server_uri: ad_uri< uri_processor<> > {};

struct moke_server_reader_advice: aa::advice< _reader_, moke_server_reader>{};

struct moke_server_writer_advice: aa::advice< _writer_, moke_server_writer>{};

typedef ap::type_list<
  moke_server_reader_advice,
  moke_server_writer_advice
>::type moke_advice_list;

typedef aa::aspect_merge<
  aspect,
  aa::aspect< moke_advice_list >
>::type moke_aspect;

// typedef aa::aspect< advice_list > aspect;

}}}}}

#endif
