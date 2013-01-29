#ifndef FAS_MISC_COMMAND_PROCESSOR_LIST_HPP
#define FAS_MISC_COMMAND_PROCESSOR_LIST_HPP

#include <stdexcept>

#include <fas/pattern/type_list.hpp>
#include <fas/aop/aspect.hpp>


namespace fas{ namespace command{

/*
namespace ap = ::fas::pattern;
namespace aa = ::fas::aop;

template<typename C, template <typename> class I>
class iprocessor_impl
   : public I<C>
{
};

namespace detail
{
  template<typename L, template <typename> class I>
  struct make_impl_list
  {
    typedef aa::advice< ap::type2type<L>, iprocessor_impl<L, I> > type;
    typedef I<L> pure;
  };

  template<typename T, typename C, bool B,  template <typename> class I>
  struct make_impl_list< aa::advice<T, C, B>, I >
  {
    typedef aa::advice<T, C, B> advice;
    typedef typename advice::advice_class advice_class;
    typedef typename advice::all_tag_list all_tag_list;
    typedef ap::type_list< ap::type2type<advice_class>, all_tag_list> new_tag_list;
    typedef typename advice::template rebind< new_tag_list, iprocessor_impl<advice_class, I> , advice::make_advice_tag >::type type;

    typedef I<advice_class> pure;
  };

  template<typename L, typename R,  template <typename> class I>
  struct make_impl_list<ap::type_list<L, R>, I >
  {
    typedef L advice;
    typedef ap::type_list<
      typename make_impl_list<L, I>::type,
      typename make_impl_list<R, I>::type
    > type;

    typedef ap::type_list<
      typename make_impl_list<L, I>::pure,
      typename make_impl_list<R, I>::pure
    > pure;
  };


  template< template <typename> class I>
  struct make_impl_list<ap::empty_type, I>
  {
    typedef ap::empty_type type;
    typedef ap::empty_type pure;
  };

}

/// ////////////////////////////////////////

template<typename C>
class without_implementation
  : public C::iprocessor
{
};

template<typename C>
class except_implementation
  : public C::iprocessor
{
public:
  virtual void process(C& cmd)
  {
    throw std::runtime_error( std::string("command not implementation") );
  }
};

template<typename C>
class except_named_implementation
  : public C::iprocessor
{
public:
  virtual void process(C& cmd)
  {
    throw std::runtime_error( std::string("command ") +  std::string(cmd.name()) + std::string(" not implementation") );
  }
};

/// //////////////////////////////////////////////////////////////////////////

template<typename L, template<typename> class I = without_implementation >
class iprocessor_list
  : public aa::aspect_class< aa::aspect< typename detail::make_impl_list<L, I >::type > >
{
public:
  typedef L command_list;
  typedef ap::scatter_hierarchy< typename detail::make_impl_list<L, I >::pure > pure;
  template<typename LL = L, template<typename> class II = I>
  struct rebind
  {
    typedef iprocessor_list<LL, II> type;
  };

  template<typename C>
  void process(C& cmd)
  {
    this->get_aspect().template get< ap::type2type<C> >().process(cmd);
  }
};

*/

}}

#endif

