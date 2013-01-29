#ifndef FAS_MISC_COMMAND_AD_BINDER_PTR_HPP
#define FAS_MISC_COMMAND_AD_BINDER_PTR_HPP

#include <stdexcept>
//#include <fas/aop/>

namespace fas{ namespace command{

/*
namespace aa = ::fas::aop;
namespace ap = ::fas::pattern;
*/


template<typename B>
class ad_binder_ptr
{
public:
  typedef B binder_type;

  ad_binder_ptr():  _binder(0) {}

  void set(B* ptr) { _binder = ptr; }
  B* get() { return _binder; }
  const B* get() const { return _binder; }

  template<typename T, typename C>
  void operator()(T&, C& cmd) 
  {
    if ( _binder )
      _binder->process(cmd);
    else
      throw std::logic_error("ad_binder_ptr: pointer binder is not set");
  }

private:
  binder_type* _binder;
};

}}

#endif
