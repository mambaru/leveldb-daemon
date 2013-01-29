#ifndef FAS_MISC_HTTP_URI_DEFAULT_RULE_HPP
#define FAS_MISC_HTTP_URI_DEFAULT_RULE_HPP

#include "ad_rule.hpp"
#include "rules/rule_any.hpp"

namespace fas{ namespace http{ namespace uri{

struct default_rule: aa::advice< ad_rule< _default_uri_ready_, rule_any > > {};

}}}

#endif
