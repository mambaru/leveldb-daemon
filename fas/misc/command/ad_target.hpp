#ifndef FAS_MISC_COMMAND_AD_TARGET_HPP
#define FAS_MISC_COMMAND_AD_TARGET_HPP

#include <fas/misc/command/tags.hpp>

namespace fas{ namespace command{

template<typename I>
class ad_target: public _target_
{
public:
  typedef I dynamic_interface;
  typedef typename dynamic_interface::pointer dynamic_pointer;

  void set( dynamic_pointer dp ) { _pointer = dp; };
  dynamic_pointer& get() { return _pointer; }
  const dynamic_pointer& get() const { return _pointer; }
private:
  dynamic_pointer _pointer;
};

}}

#endif

