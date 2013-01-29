#ifndef FAS_MISC_HTTP_CLEANER_ADVICE_HPP
#define FAS_MISC_HTTP_CLEANER_ADVICE_HPP

#include <fas/aop/advice.hpp>
#include <fas/adv/ad_cleaner.hpp>
#include "tags.hpp"

namespace fas{ namespace http{ 

namespace aa = ::fas::aop;
namespace ad = ::fas::adv;

struct cleaner_advice: aa::advice< _make_clean_, ad::ad_cleaner< _cleaner_> > {};

}}

#endif
