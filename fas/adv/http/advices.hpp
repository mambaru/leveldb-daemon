//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_ADV_HTTP_ADVICES_HPP
#define FAS_ADV_HTTP_ADVICES_HPP

#include <fas/aop/advice.hpp>
#include <fas/aop/aspect.hpp>
#include <fas/adv/ad_state.hpp>
#include <fas/adv/ad_stub.hpp>
#include <fas/adv/ad_cleaner.hpp>
#include <fas/adv/ad_named_proxy.hpp>
#include <fas/adv/http/tags.hpp>
#include <fas/adv/http/ad_request_header.hpp>
#include <fas/adv/http/ad_response_header.hpp>
#include <fas/adv/http/ad_header_reader.hpp>
//#include <fas/adv/http/ad_body_reader.hpp>
#include <fas/adv/http/ad_body_chunk_reader.hpp>
#include <fas/adv/http/ad_body_buf_reader.hpp>
#include <fas/adv/http/ad_nobody_reader.hpp>

#include <fas/adv/http/ad_writer.hpp>
#include <fas/adv/http/ad_initial_writer.hpp>
#include <fas/adv/http/ad_header_writer.hpp>
//#include <fas/adv/http/ad_body_writer.hpp>
#include <fas/adv/http/ad_body_chunk_writer.hpp>
#include <fas/adv/http/ad_body_buf_writer.hpp>
#include <fas/adv/http/ad_nobody_writer.hpp>
#include <fas/adv/http/ad_process.hpp>
#include <fas/adv/http/ad_incoming.hpp>

namespace fas{ namespace adv{ namespace http{

namespace aa = ::fas::aop;
namespace ad = ::fas::adv;
namespace ap = ::fas::pattern;


/// Заголовки

struct stubs_advice
  : aa::advice<
      aa::tag_list_n<
        aa::tag<_head_readed_>,
        aa::tag<_body_readed_>,
        aa::tag<_body_writed_>
      >::type,
      ad::ad_stub<>
    >
{};



struct request_header_advice
  : aa::advice<
      aa::tag_list_n<
        aa::gtag<ad::_cleaner_>,
        aa::tag<_request_header_>
      >::type,
      ad_request_header
    >
{};

struct response_header_advice
  : aa::advice<
      aa::tag_list_n<
        aa::gtag<ad::_cleaner_>,
        aa::tag<_response_header_>
      >::type,
      ad_response_header
    >
{};

/// Заголовки для клиента (меняем request на response и наоборот)

struct client_request_header_advice
  : aa::advice<
      aa::tag_list_n<
        aa::gtag<ad::_cleaner_>,
        aa::tag<_request_header_>
      >::type,
      ad_response_header
    >
{};

struct client_response_header_advice
  : aa::advice<
      aa::tag_list_n<
        aa::gtag<ad::_cleaner_>,
        aa::tag<_response_header_>
      >::type,
      ad_request_header
    >
{};


/// Читатели

struct reader_states_advice
  : aa::advice<
      aa::tag_list_n<
        aa::tag<_input_>,
        aa::tag<_reader_state_>,
        aa::gtag<ad::_cleaner_>
      >::type,
      ad::ad_state< _greader_, _state_read_header_, true>
    >
{};


struct header_reader_advice
  : aa::advice<
      aa::tag_list_n<
        aa::gtag<_greader_>,
        aa::tag<_state_read_header_>,
        aa::tag<_header_reader_>
      >::type,
      ad_header_reader
  >
{};


struct body_chunk_reader_advice
  : aa::advice<
      aa::tag_list_n<
        aa::gtag<_greader_>,
        aa::tag<_state_read_chunk_>,
        aa::tag<_body_chunk_reader_>
      >::type,
      ad_body_chunk_reader
  >
{};

struct body_buf_reader_advice
  : aa::advice<
      aa::tag_list_n<
        aa::gtag<_greader_>,
        aa::tag<_state_read_body_>,
        aa::tag<_body_buf_reader_>
      >::type,
      ad_body_buf_reader
  >
{};

struct nobody_reader_advice
  : aa::advice<
      aa::tag_list_n<
        aa::gtag<_greader_>,
        aa::tag<_state_read_nobody_>,
        aa::tag<_nobody_reader_>
      >::type,
      ad_nobody_reader
  >
{};

/// Писатели

struct writer_states_advice
  : aa::advice<
      aa::tag_list_n<
        aa::gtag<ad::_cleaner_>,
        aa::tag<_writer_state_>
      >::type,
      ad::ad_state< _gwriter_, _state_write_initial_, true>
    >
{};

struct initial_writer_advice
  : aa::advice<
      aa::tag_list_n<
        aa::gtag<_gwriter_>,
        aa::tag<_state_write_initial_>,
        aa::tag<_initial_writer_>
      >::type,
      ad_initial_writer
  >
{};

struct header_writer_advice
  : aa::advice<
      aa::tag_list_n<
        aa::gtag<_gwriter_>,
        aa::tag<_state_write_header_>,
        aa::tag<_header_writer_>
      >::type,
      ad_header_writer
  >
{};

struct body_buf_writer_advice
  : aa::advice<
      aa::tag_list_n<
        aa::gtag<_gwriter_>,
        aa::tag<_state_write_body_>,
        aa::tag<_body_buf_writer_>
      >::type,
      ad_body_buf_writer
  >
{};

struct body_chunk_writer_advice
  : aa::advice<
      aa::tag_list_n<
        aa::gtag<_gwriter_>,
        aa::tag<_state_write_chunk_>,
        aa::tag<_body_chunk_writer_>
      >::type,
      ad_body_chunk_writer
  >
{};

struct nobody_writer_advice
  : aa::advice<
      aa::tag_list_n<
        aa::gtag<_gwriter_>,
        aa::tag<_state_write_nobody_>,
        aa::tag<_nobody_writer_>
      >::type,
      ad_nobody_writer
  >
{};

/// Обработка

struct process_advice
  : aa::advice<
      aa::tag<_process_>,
      ad_process
  >
{};

struct process_uri_advice
  : aa::advice<
      aa::tag<_process_uri_>,
      ad::ad_named_proxy<_guri_, _outgoing_, true>
  >
{};

struct incoming_advice
  : aa::advice<
      aa::tag_list_n<
        aa::tag<_incoming_>,
        aa::gtag<ad::_cleaner_>
      >::type,
      ad_incoming
  >
{};


struct writer_advice
  : aa::advice<
      aa::tag_list_n<
        aa::tag<_writer_>
      >::type,
      ad_writer
  >
{};


/*

typedef aa::advice<
      aa::tag_list_n<
        aa::gtag<ad::_cleaner_>,
        aa::tag<_request_header_>
      >::type,
      ad_request_header
    > request_header_advice;


typedef aa::advice<
      aa::tag_list_n<
        aa::gtag<ad::_cleaner_>,
        aa::tag<_response_header_>
      >::type,
      ad_response_header
    > response_header_advice;

/// Заголовки для клиента (меняем request на response и наоборот)

typedef aa::advice<
      aa::tag_list_n<
        aa::gtag<ad::_cleaner_>,
        aa::tag<_request_header_>
      >::type,
      ad_response_header
    > client_request_header_advice;


typedef aa::advice<
      aa::tag_list_n<
        aa::gtag<ad::_cleaner_>,
        aa::tag<_response_header_>
      >::type,
      ad_request_header
    > client_response_header_advice;


/// Читатели

typedef aa::advice<
      aa::tag_list_n<
        aa::tag<_input_>,
        aa::tag<_reader_state_>,
        aa::gtag<ad::_cleaner_>
      >::type,
      ad::ad_state< _greader_, _state_read_header_, true>
    > reader_states_advice;



typedef aa::advice<
      aa::tag_list_n<
        aa::gtag<_greader_>,
        aa::tag<_state_read_header_>,
        aa::tag<_header_reader_>
      >::type,
      ad_header_reader
  > header_reader_advice;


typedef aa::advice<
      aa::tag_list_n<
        aa::gtag<_greader_>,
        aa::tag<_state_read_chunk_>,
        aa::tag<_body_chunk_reader_>
      >::type,
      ad_body_chunk_reader
  > body_chunk_reader_advice;

typedef aa::advice<
      aa::tag_list_n<
        aa::gtag<_greader_>,
        aa::tag<_state_read_body_>,
        aa::tag<_body_buf_reader_>
      >::type,
      ad_body_buf_reader
  > body_buf_reader_advice;

typedef aa::advice<
      aa::tag_list_n<
        aa::gtag<_greader_>,
        aa::tag<_state_read_nobody_>,
        aa::tag<_nobody_reader_>
      >::type,
      ad_nobody_reader
  > nobody_reader_advice;


/// Писатели

typedef aa::advice<
      aa::tag_list_n<
        aa::gtag<ad::_cleaner_>,
        aa::tag<_writer_state_>
      >::type,
      ad::ad_state< _gwriter_, _state_write_initial_, true>
    > writer_states_advice;

typedef aa::advice<
      aa::tag_list_n<
        aa::gtag<_gwriter_>,
        aa::tag<_state_write_initial_>,
        aa::tag<_initial_writer_>
      >::type,
      ad_initial_writer
  > initial_writer_advice;

typedef aa::advice<
      aa::tag_list_n<
        aa::gtag<_gwriter_>,
        aa::tag<_state_write_header_>,
        aa::tag<_header_writer_>
      >::type,
      ad_header_writer
  > header_writer_advice;

typedef aa::advice<
      aa::tag_list_n<
        aa::gtag<_gwriter_>,
        aa::tag<_state_write_body_>,
        aa::tag<_body_buf_writer_>
      >::type,
      ad_body_buf_writer
  > body_buf_writer_advice;

typedef aa::advice<
      aa::tag_list_n<
        aa::gtag<_gwriter_>,
        aa::tag<_state_write_chunk_>,
        aa::tag<_body_chunk_writer_>
      >::type,
      ad_body_chunk_writer
  > body_chunk_writer_advice;

typedef aa::advice<
      aa::tag_list_n<
        aa::gtag<_gwriter_>,
        aa::tag<_state_write_nobody_>,
        aa::tag<_nobody_writer_>
      >::type,
      ad_nobody_writer
  > nobody_writer_advice;

/// Обработка

typedef aa::advice<
      aa::tag<_process_>,
      ad_process
  > process_advice;


typedef aa::advice<
      aa::tag<_process_uri_>,
      ad::ad_named_proxy<_guri_, _outgoing_, true>
  > process_uri_advice;

typedef aa::advice<
      aa::tag<_incoming_>,
      ad_incoming
  > incoming_advice;


typedef aa::advice<
      aa::tag_list_n<
        aa::tag<_writer_>
      >::type,
      ad_writer
  > writer_advice;
*/

}}}

#endif
