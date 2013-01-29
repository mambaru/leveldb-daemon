#ifndef FAS_MISC_COMMAND_TARGET_LIST_HPP
#define FAS_MISC_COMMAND_TARGET_LIST_HPP

#include <fas/aop/aspect.hpp>

namespace fas{ namespace command{

namespace aa = ::fas::aop;

template<typename A>
class target_list
  : public aa::aspect_class<A>
{
  typedef aa::aspect_class<A> super;
public:
  template<typename Tg, typename P>
  void set(P& p)
  {
    super::get_aspect().template get<Tg>().set(p);
  };
};

}}

#endif
