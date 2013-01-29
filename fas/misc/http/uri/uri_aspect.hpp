#ifndef FAS_MISC_HTTP_URI_URI_ASPECT_HPP
#define FAS_MISC_HTTP_URI_URI_ASPECT_HPP

#include "default_rule.hpp"
#include <fas/aop/aspect.hpp>
#include <fas/pattern/type_list.hpp>



namespace fas{ namespace http{ namespace uri{

namespace ap = ::fas::pattern;
namespace aa = ::fas::aop;

typedef ap::type_list_n<
  default_rule
>::type rule_list;

typedef aa::aspect<default_rule> uri_aspect;

}}}

#endif
