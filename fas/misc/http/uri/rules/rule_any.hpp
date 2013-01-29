#ifndef FAS_MISC_HTTP_URI_RULES_RULE_ANY_HPP
#define FAS_MISC_HTTP_URI_RULES_RULE_ANY_HPP

namespace fas{ namespace http{ namespace uri{

struct rule_any
{
  bool operator()(const char*, const char*)
  {
    return true;
  }
};

}}}

#endif
