#ifndef FAS_ADV_JSON_RPC2_HTTP_CHUNKED_ASPECT_HPP
#define FAS_ADV_JSON_RPC2_HTTP_CHUNKED_ASPECT_HPP

#include <fas/pattern/type_list.hpp>
#include <fas/aop/aspect.hpp>
#include <fas/adv/ad_stub.hpp>
#include <fas/adv/ad_cleaner.hpp>


#include <fas/adv/json_rpc2/aspect.hpp>
#include <fas/adv/json_rpc2/http_chunked/tags.hpp>
#include <fas/adv/json_rpc2/http_chunked/ad_chunk_reader.hpp>
#include <fas/adv/json_rpc2/http_chunked/ad_chunk_writer.hpp>

#include <fas/adv/http/aspects.hpp>

namespace fas{ namespace adv{ namespace json_rpc2{ namespace http_chunked{

namespace ap = ::fas::pattern;
namespace aa = ::fas::aop;
namespace ad = ::fas::adv;
namespace adjr2 = ::fas::adv::json_rpc2;
namespace adh = ::fas::adv::http;

/**
struct json_http_alias_advice_list:
  ap::type_list_n<
    aa::alias< adh::_input_, af::_on_read_ >,
    aa::alias< adjr::_input_, adh::_outgoing_ >,
    aa::alias< adh::_incoming_, adjr::_output_ >,
    aa::alias< af::_writer_, adh::_output_  >
  >::type
{};

*/
struct advice_list
  : ap::type_list_n<
      aa::alias< adh::_input_, adjr2::_input_>,
      aa::alias< adjr2::_invoke_, adh::_outgoing_>,
      aa::alias< adh::_incoming_, adjr2::_writer_>,
      aa::alias< adjr2::_output_, adh::_output_>
    >::type
{};

struct aspect:
  aa::aspect_merge<
    aa::aspect_merge<
      adh::server_aspect,
      adjr2::aspect
    >::type,
    aa::aspect<advice_list>
  >::type
{};

}}}}

#endif
