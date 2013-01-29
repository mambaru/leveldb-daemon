#ifndef FAS_MISC_HTTP_URI_AD_RULE_HPP
#define FAS_MISC_HTTP_URI_AD_RULE_HPP

#include "../tags.hpp"
#include <fas/aop/tag.hpp>

namespace fas{ namespace http{ namespace uri{

namespace aa = ::fas::aop;

template<typename Tg, typename R>
struct ad_rule
  : aa::gtag<_rule_>
{
  typedef Tg processor_tag;

  template<typename T>
  bool operator()(T&, const char* beg, const char* end)
  {
    return _rule(beg, end);
  }

private:
  R _rule;
};

}}}

#endif
