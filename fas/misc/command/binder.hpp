#ifndef FAS_MISC_COMMAND_BINDER_HPP
#define FAS_MISC_COMMAND_BINDER_HPP


#include <stdexcept>
#include <fas/aop/aspect.hpp>
#include <fas/pattern/type_list.hpp>
#include <fas/misc/command/tags.hpp>
#include <fas/pattern/dynamic_interface.hpp>
namespace fas{ namespace command{

namespace aa = ::fas::aop;
namespace ap = ::fas::pattern;

template< typename A = aa::aspect<> >
class binder_base
  : public aa::aspect_class<A>
{
  typedef aa::aspect_class<A> super;
public:
  typedef typename super::aspect aspect;
  template<typename T, typename C>
  void bind(T& t, C& cmd)
  {
    t.get_aspect().template get< ap::type2type<C> >()(t, cmd);
  }

  template<typename B>
  void initialize(const B& from)
  {
     // typedef typename aa::extract_aspect_tags< typename B::aspect >::tag_list from_tag_list;
     typedef typename B::aspect::template select<_target_>::type target_list;
     typedef typename aa::extract_advice_tags< target_list >::tag_list from_tag_list;
     typedef typename aa::extract_aspect_tags< typename super::aspect >::tag_list this_tag_list;
     typedef typename ap::intersection<from_tag_list, this_tag_list>::type result_tag_list;

     //result_tag_list a(1000);
     _initialize(from, result_tag_list());
  }

private:
  template<typename B, typename L>
  void _initialize(const B& from, L)
  {
    
    typedef typename L::left_type tag;

    typedef typename aa::advice_cast<tag, typename super::aspect>::type current_target;
    typedef typename current_target::dynamic_pointer target_ponter;

    typedef typename aa::advice_cast<tag, typename B::aspect>::type from_target;
    typedef typename from_target::dynamic_pointer from_ponter;

    from_ponter from_ptr = from.get_aspect().template get<tag>().get();

    this->get_aspect().template get<tag>().set(
       ap::interface_cast<target_ponter>( &from_ptr )
    );
    _initialize(from, typename L::right_type() );
  }

  template<typename B>
  void _initialize(const B& , ap::empty_type)
  {
  }

};

template< typename A = aa::aspect<> >
class binder: public binder_base<A>
{
  typedef binder_base<A> super;
public:
  typedef typename super::aspect aspect;

  template<typename T, typename C>
  void operator()(T&, C& cmd) { super::bind(*this, cmd); }

  template<typename C>
  void bind(C& cmd) { super::bind(*this, cmd); }
 
};

}}

#endif
