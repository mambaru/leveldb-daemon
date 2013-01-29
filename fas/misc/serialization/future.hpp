#ifndef FAS_MISC_SEREALIZETION_FUTURE_HPP
#define FAS_MISC_SEREALIZETION_FUTURE_HPP

#include <fas/pattern/type_list.hpp>


#define FASLIB_HAS_TYPENAME(N, D)\
namespace N##_detail{\
struct N##_helper {\
  typedef char small_type;\
  typedef class big_tag { char dummy[2];} big_type;\
  template<typename P>\
  static small_type test(typename P::D const *);\
  template<typename P>\
  static big_type test(...);\
}; \
template<typename T, bool>\
struct get_type;\
template<typename T>\
struct get_type<T, false>{typedef ::fas::pattern::empty_type type; };\
template<typename T>\
struct get_type<T, true>{ typedef typename T::D type; };\
} \
template<typename T>\
struct N\
{\
  typedef N##_detail::N##_helper helper; \
  enum { result = sizeof( helper::test<T>(0) ) == sizeof(helper::small_type) };\
  typedef typename N##_detail::get_type<T, result>::type type;\
};






namespace fas{ namespace pattern{


/// /////////////////////////////////////////////////////////
/// select_if


namespace detail
{

  template<typename L, template<typename> class IF >
  struct select_if;

  template<typename L, template<typename> class IF, bool >
  struct select_if_helper;

  template<typename L, template<typename> class IF>
  struct select_if_helper<L, IF, false>
  {
    typedef typename select_if< typename L::right_type, IF>::type type;
  };

  template<typename L, template<typename> class IF>
  struct select_if_helper<L, IF, true>
  {
    typedef type_list<
              typename L::left_type,
              typename select_if< typename L::right_type, IF>::type
            > type;
  };

  template<typename L, template<typename> class IF >
  struct select_if
  {
    typedef typename select_if_helper<L, IF, IF<typename L::left_type>::result >::type type;
  };

  template<template<typename> class IF >
  struct select_if<empty_type, IF>
  {
    typedef empty_type type;
  };

}

template< template<typename> class IF >
struct negate_if
{
  template<typename T>
  struct type
  {
    enum { result = !IF<T>::result };
  };
};

template<typename L, template<typename> class IF >
struct select_if
{
  typedef typename type_list_traits<L>::type common_list;
  typedef typename detail::select_if<common_list, IF>::type type;
  typedef typename detail::select_if<common_list, negate_if<IF>::template type >::type margin;
  // typedef typename erase_from_list<common_list, type, 0>::type margin;

};


/// @return  IF<L[0]>::result || IF<L[1]>::result || IF<L[2]>::result ...
template<typename L, template<typename> class IF >
struct at_least_one
{
  enum { result = length< typename select_if<L, IF>::type >::result!=0 };
};


template<typename T1, template<typename, typename > class F>
struct bind1st
{
  template<typename T2>
  struct type: F<T1, T2>{};
};

template<typename T2, template<typename, typename > class F>
struct bind2st
{
  template<typename T1>
  struct type:F<T1, T2>{};
};

template<template<typename> class IF1, template<typename> class IF2>
struct logical_and
{
  template<typename T>
  struct type
  {
    enum { result = IF1<T>::result && IF2<T>::result };
  };
};

template<typename T, typename L>
struct find
{
  enum { result = type_count<T, L>::result!=0};
};

template<typename L>
struct find<empty_type, L>
{
  enum { result = 0};
};


template<template<typename> class F>
struct type_f
{
  template<typename T>
  struct type: F<typename T::type>{};
  /*{
    enum { result = IF<typename T::type>::result };
  };*/
};

template<template<typename> class F1, template<typename> class F2>
struct rebind
{
  template<typename T>
  struct type: F2< typename F1<T>::type >{};
};

template<typename>
struct the_true
{
  enum {result=1};
};

/**
 * @tparam L список чего ищем
 * @tparam SL список где ищем
 * @tparam IF условие
*/
template<typename L, typename SL,  template<typename> class IF = the_true >
struct select_from
{
  /*typedef typename select_if<
      SL,
      rebind<
          IF,
          bind2st<L, find >::template type
      >::template type
  >::type type;*/
  typedef select_if<SL, rebind< IF, bind2st<L, find >::template type >::template type > select;
  typedef typename select::type type;
  typedef typename select::margin margin;
};

/**
 *   typedef ap::select_if<
      temp::abc_list,
      ap::rebind<
        temp::has_meta,
         ap::bind2st<
           temp::meta_list,
           ap::find
          >::type
      >::type
  >::type seleted_list;

*/
/**
 * 
 * typedef common_list;
 * typedef ap::type_list_n< meta::metatype::attribute, meta::metatype::property >::type metalist;
 * at_least_one< metalist, has_meta<>
 * select_if< common_list, если тип имеет metatype и он соответствует одному из metalist >
 * 
 * select_if< common_list,
 *      bind2st< metalist, find< type_if<has_metatype>::type >  >
 *    >
 * >
 * 
 * 
  typedef typename V::memberlist memberlist;
  typedef typename V::target target; // Тип исходного класса

  // property и аттрибуты
  typedef typename meta::select_meta<memberlist, metalist>::type first_list;
*/

}}

#endif
