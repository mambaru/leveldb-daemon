#ifndef FAS_MISC_COMMAND_COMMAND_INTERFACE_HPP
#define FAS_MISC_COMMAND_COMMAND_INTERFACE_HPP

#include <stdexcept>

#include <fas/misc/command.hpp>
#include <fas/pattern/dynamic_interface.hpp>
#include <fas/pattern/type_list.hpp>
#include <fas/aop/aspect.hpp>


namespace fas{ namespace command{

namespace ap = ::fas::pattern;
namespace aa = ::fas::aop;

namespace detail
{
  template<typename T, bool>
  struct has_interface;

  template<typename T>
  struct has_interface<T, true>
  {
    typedef typename T::iprocessor type;
  };

  template<typename T>
  struct extract_interface
  {
     typedef typename has_interface< T, ap::template_conversion1<T, command>::result >::type type;
  };

  template<typename L>
  struct interface_list_traits_helper;

  template<typename L, typename R>
  struct interface_list_traits_helper< ap::type_list<L, R> >
  {
    typedef ap::type_list< 
      typename extract_interface<L>::type, 
      typename interface_list_traits_helper<R>::type 
    > type;
  };

  template<>
  struct interface_list_traits_helper< ap::empty_type >
  {
    typedef ap::empty_type type;
  };

}

template<typename L>
struct interface_list_traits
{
  typedef typename detail::interface_list_traits_helper< typename ap::type_list_traits<L>::type >::type type;
};

/// //////////////////////////
/// ////////////////////////////////////////

template<typename C>
class without_implementation
  : public C::iprocessor
{
};

template<typename C>
class empty_implementation
  : public C::iprocessor
{
public:
  virtual void process(typename C::command_type& cmd)
  {
  }
};

template<typename C>
class except_implementation
  : public C::iprocessor
{
public:
  virtual void process(typename C::command_type& cmd)
  {
    throw std::runtime_error( std::string("command not implementation") );
  }
};

template<typename C>
class except_named_implementation
  : public C::iprocessor
{
public:
  virtual void process(typename C::command_type& cmd)
  {
    throw std::runtime_error( std::string("command ") +  std::string(cmd.name()) + std::string(" not implementation") );
  }
};

/// //////////////////////////


template< template<typename> class IMPL>
struct apply_impl
{
  template< typename T >
  struct maker
  {
    typedef IMPL<T> type;
  };
};

template<typename L, typename CIL , template<typename> class IMPL>
struct make_impl_list
{
  // применяем реализацию по умолчанию для всех комманд из списка
  typedef typename ap::for_each<L, apply_impl<IMPL>::template maker >::type default_impl;
  // объеденяем список реализааций по умолчанию со списком заданныз реализаций
  typedef typename ap::merge<default_impl, CIL>::type merged;
  // извлекаем список интерфейсов комманд
  typedef typename interface_list_traits<L>::type interface_list;
  // удаляем дубликаты с общими интерфейсами оставляя последнее вхождение
  // (т.к. реализации по умолчанию идут в начале, то удаляются они, если пользователь задал свою реализацию)
  typedef typename ap::unique_from_list<merged, interface_list>::type uniqued;
  typedef typename ap::organize<uniqued>::type type;
  
//  typedef merged type;
};

/// @param L - строго список комманд
/// @param CIL - список пользовательских реализаций
/// @param IMPL - реализация по умолчанию
template<typename L, typename CIL = ap::empty_type, template<typename> class IMPL = without_implementation>
class command_implementation
  : public ap::scatter_hierarchy< typename make_impl_list<L, CIL, IMPL>::type >
{
  typedef ap::scatter_hierarchy< typename make_impl_list<L, CIL, IMPL>::type > super;
public:
  typedef typename interface_list_traits<L>::type interface_list;

  template<typename C>
  void process(C& cmd)
  {
    super::template get< typename C::iprocessor >().process(cmd);
  }
};
/*
template<typename L, typename CIL = ap::empty_type, template<typename> class IMPL = without_implementation>
class command_implementation
  : public ap::scatter_hierarchy< typename ap::for_each<L, apply_impl<IMPL>::template maker >::type> 
{
  typedef ap::scatter_hierarchy< typename ap::for_each<L, apply_impl<IMPL>::template maker >::type> super;
public:
  typedef typename interface_list_traits<L>::type interface_list;

  template<typename C>
  void process(C& cmd)
  {
    super::template get< typename C::iprocessor >().process(cmd);
  }
};
*/

template<typename DP>
class command_pointer : public DP
{
  typedef DP super;
public:
  command_pointer():DP(){};
  command_pointer(int null):DP(null){};
//  command_pointer( const command_pointer<DP>& p ):DP(static_cast<const DP&>(p)){};
  /*
  command_pointer( const command_pointer<DP>& p ):DP(static_cast<const DP&>(p)){};
  command_pointer( const DP& p ):DP(p){};
  */

  template<typename C>
  void process(C& cmd)
  {
    if ( typename C::iprocessor *p = super::template get<typename C::iprocessor>() )
      p->process(cmd);
  }
}; 

/// @param L - любой список типов, унаследованных от command<typename>
template<typename L,
         template<typename> class IPM = ap::interface_pointer_maker >
class command_interface
  : public ap::dynamic_interface< typename interface_list_traits<L>::type, IPM >
{
  typedef ap::dynamic_interface< typename interface_list_traits<L>::type, IPM > super;
public:
  typedef typename ap::type_list_traits<L>::type command_list;
  typedef typename super::interface_list interface_list;
  typedef command_pointer<typename super::pointer> pointer;

  operator pointer() { return ap::interface_cast<pointer>(this); }

  template<
    typename CIL = ap::empty_type, // command implementation list - список реализаций командных интерфейсов
    template<typename> class IMPL = without_implementation // реализация по умолчанию
  >
  struct implement
  {
    typedef command_implementation<command_list, CIL, IMPL> type;
  };
};

}}

#endif

