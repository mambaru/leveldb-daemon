//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_UTIL_AMF_AMF_ADVICES_HPP
#define FAS_UTIL_AMF_AMF_ADVICES_HPP

#include <fas/util/amf/ad_chunk_collector.hpp>
#include <fas/util/amf/ad_chunk_prepare.hpp>
#include <fas/util/amf/ad_process_content.hpp>
#include <fas/util/amf/ad_proxy_invocation.hpp>
#include <fas/util/amf/ad_stub_invocation.hpp>
#include <fas/util/amf/ad_chunk_splitter.hpp>

#include <fas/util/amf/tags.hpp>
namespace fas{ namespace  util{ namespace amf{

namespace aa = ::fas::aop;
namespace af = ::fas::filter;
namespace ap = ::fas::pattern;

struct chunk_splitter_advice:
  aa::advice<
    aa::tag<_chunk_splitter_>,
    ad_chunk_splitter
  >
{};

struct process_content_advice:
  aa::advice<
    aa::tag<_process_content_>,
    ad_process_content<
      content<0x14, _chunk_collector_>
    >
  >
{};

struct chunk_collector_advice:
  aa::advice<
    aa::tag<_chunk_collector_>,
    ad_chunk_collector<_proxy_invocation_>
  >
{};

template<typename IL>
struct proxy_invocation_advice:
  aa::advice<
    aa::tag<_proxy_invocation_>,
    ad_proxy_invocation< IL >
  >
{};

template<typename IL>
struct stub_invocation_advice:
  aa::advice<
    aa::tag<_stub_invocation_>,
    ad_stub_invocation< IL >
  >
{};


struct chunk_prepare_advice:
  aa::advice<
    aa::tag<_chunk_prepare_>,
    ad_chunk_prepare
  >
{};

}}}

#endif
