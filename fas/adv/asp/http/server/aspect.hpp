//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2010
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_ADV_ASP_HTTP_SERVER_ASPECT_HPP
#define FAS_ADV_ASP_HTTP_SERVER_ASPECT_HPP

#include "../tags.hpp"
#include "ad_exception.hpp"
#include "ad_incoming.hpp"
#include "ad_outgoing.hpp"
#include "ad_input.hpp"
#include "ad_output.hpp"
#include "ad_Not_Implemented.hpp"
#include <fas/misc/http/tags.hpp>
#include <fas/adv/ad_stub.hpp>
#include <fas/adv/ad_cleaner.hpp>

#include <fas/misc/http/reader/body_reader.hpp>
#include <fas/misc/http/reader/server_header_reader.hpp>
#include <fas/misc/http/writer/header_writer.hpp>
#include <fas/misc/http/writer/server_header_writer.hpp>
#include <fas/misc/http/writer/body_writer.hpp>
#include <fas/misc/http/ad_writer.hpp>
#include <fas/misc/http/ad_reader.hpp>



namespace fas{ namespace adv{ namespace asp{ namespace http{ namespace server{

namespace aa = ::fas::aop;
namespace ap = ::fas::pattern;
namespace ad = ::fas::adv;
namespace ah = ::fas::http;
namespace ahr = ::fas::http::reader;
namespace ahw = ::fas::http::writer;

struct exception_advice
  : aa::advice<_exception_, ad_exception >{};

struct incoming_advice
  : aa::advice< aa::tag_list_n< ah::_body_readed_, ah::_head_readed_, _pre_incoming_, ah::_default_uri_ready_ >::type, ad_incoming > { };

struct outgoing_advice
  : aa::advice< _outgoing_, ad_outgoing > { };

struct input_advice
  : aa::advice< _input_, ad_input > { };

struct output_advice
  : aa::advice< aa::tag_list_n< ah::_body_ready_, ah::_head_ready_, _pre_output_>::type, ad_output > { };

struct not_implemented_advice: aa::advice< _incoming_, ad_Not_Implemented >
{
};

struct stubs_advice
  : aa::advice< 
     aa::tag_list_n<
       _output_
     >::type
     , ad_stub<> > 
{};

struct server_reader: ah::ad_reader< ahr::server_header_reader<>, ahr::body_reader<> > {};

struct server_writer: ah::ad_writer< ahw::server_header_writer<>, ahw::body_writer<> > {};

struct server_reader_advice: aa::advice< aa::tag_list_n< aa::gtag<ad::_cleaner_>, _reader_ >::type, server_reader>{};

struct server_writer_advice: aa::advice< aa::tag_list_n< aa::gtag<ad::_cleaner_>, _writer_ >::type, server_writer>{};



typedef ap::type_list_n<
  exception_advice,
  incoming_advice,
  outgoing_advice,
  input_advice,
  output_advice,
  not_implemented_advice,
  server_reader_advice,
  server_writer_advice
>::type advice_list;

typedef aa::aspect< advice_list > aspect;

}}}}}

#endif
