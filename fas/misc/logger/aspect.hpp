#ifndef FAS_MISC_LOGGER_ASPECT_HPP
#define FAS_MISC_LOGGER_ASPECT_HPP

#include "advices.hpp"

#include <fas/aop/advice.hpp>
#include <fas/adv/ad_stub.hpp>

namespace fas{ namespace logger{

namespace ap = ::fas::pattern;
namespace aa = ::fas::aop;
namespace ad = ::fas::adv;

typedef ap::type_list_n<
  aa::advice< _date_, ad_date>,
  aa::advice<_time_, ad_time>,
  aa::advice<_head_, ad_head>,
  aa::advice<_field_separator_, ad_sep<'\t'> >,
  aa::advice<_head_separator_, ad_sep<' '> >,
  aa::advice<_fields_, ad_fields>,
  aa::advice<_line_separator_, ad_sep<'\n'> >,
  ap::type_list_n<
    aa::advice<_file_, ad_file>,
    aa::advice<_buffer_, ad_buffer>,
    aa::advice<_writer_, ad_writer>,
    aa::advice<_flush_, ad_flush>,
    aa::advice<_on_writed_, ad::ad_stub<> >
  >::type
>::type advice_list;

typedef aa::aspect< advice_list > aspect;

}}

#endif

