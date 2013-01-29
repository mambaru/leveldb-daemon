//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_ADV_HTTP_ASPECTS_HPP
#define FAS_ADV_HTTP_ASPECTS_HPP

#include <fas/aop/advice.hpp>
#include <fas/aop/aspect.hpp>
#include <fas/adv/http/advices.hpp>

namespace fas{ namespace adv{ namespace http{

namespace aa = ::fas::aop;
namespace ad = ::fas::adv;
namespace ap = ::fas::pattern;

typedef ap::type_list_n<
          reader_states_advice,
          header_reader_advice,
          body_chunk_reader_advice,
          body_buf_reader_advice,
          nobody_reader_advice
        >::type reader_advice_list;

typedef ap::type_list_n<
          writer_states_advice,
          initial_writer_advice,
          header_writer_advice,
          body_chunk_writer_advice,
          body_buf_writer_advice,
          nobody_writer_advice,
          writer_advice
        >::type writer_advice_list;

typedef ap::type_list_n<
          request_header_advice,
          response_header_advice
        >::type server_header_list;

typedef ap::type_list_n<
          client_request_header_advice,
          client_response_header_advice
        >::type client_header_list;

typedef ap::type_list_n<
          stubs_advice,
          process_advice,
          process_uri_advice,
          incoming_advice
        >::type process_advice_list;


typedef ap::type_list_n<
          server_header_list,
          reader_advice_list,
          writer_advice_list,
          process_advice_list
        >::type server_advice_list;

typedef ap::type_list_n<
          client_header_list,
          reader_advice_list,
          writer_advice_list,
          process_advice_list
        >::type client_advice_list;

/*
struct advice_list
  : ap::type_list_n<
      request_header_advice,
      response_header_advice,
      reader_advice_list,
      writer_advice_list,
      process_advice,
      process_uri_advice,
      incoming_advice
  >::type{};
*/

/*
struct server_aspect: aa::aspect<server_advice_list>{};
struct client_aspect: aa::aspect<client_advice_list>{};
*/


typedef aa::aspect<server_advice_list> server_aspect;
typedef aa::aspect<client_advice_list> client_aspect;



}}}

#endif
