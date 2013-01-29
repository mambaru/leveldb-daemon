#ifndef FAS_MISC_COMMAND_DETAIL_AUTO_BINDER_HPP
#define FAS_MISC_COMMAND_DETAIL_AUTO_BINDER_HPP

#include <fas/aop/aspect.hpp>
#include <fas/pattern/type_list.hpp>

namespace fas{ namespace command{ namespace detail{

namespace aa = ::fas::aop;
namespace ap = ::fas::pattern;

/** *****************************************************/
/**  Извлекает спискии тегов из списка биндеров комманд */
template< typename BL >
struct target_tags_exctract_helper;

template< typename L, typename R >
struct target_tags_exctract_helper< ap::type_list<L, R> >
{
  typedef ap::type_list< typename L::target_tags_list, typename target_tags_exctract_helper<R>::type > type;
};

template< >
struct target_tags_exctract_helper<ap::empty_type>
{
   typedef ap::empty_type type;
};

template< typename BL >
struct target_tags_exctract
{
  typedef typename ap::unique< typename ap::organize< typename target_tags_exctract_helper<BL>::type >::type >::type type;
};

/** *************************************************************/
/**  стоит часть списка аспекта binder'a - адвайсы биндеров */

template< typename BL >
struct binder_builder;

template< typename L, typename R >
struct binder_builder<ap::type_list<L, R> >
{
  typedef ap::type_list< aa::advice<L> , typename binder_builder<R>::type > type;
};

template< >
struct binder_builder<ap::empty_type>
{
   typedef ap::empty_type type;
};

/** *************************************************************/
/**  стоит интерфейс auto_binder'a                              */

template< typename BL >
struct interface_builder_helper;

template< typename L, typename R >
struct interface_builder_helper<ap::type_list<L, R> >
{
  typedef ap::type_list< typename L::command_type, typename interface_builder_helper<R>::type > type;
};

template< >
struct interface_builder_helper<ap::empty_type>
{
   typedef ap::empty_type type;
};

template< typename BL >
struct interface_builder
{
  typedef command_interface< typename interface_builder_helper<BL>::type > type;
};


/** ********************************************************************/
/**  стоит часть списка аспекта binder'a - теги целей и интерфейсы */

template< typename BI, typename TL >
struct target_builder_helper;

template< typename BI, typename L, typename R >
struct target_builder_helper< BI, ap::type_list<L, R> >
{
  typedef ap::type_list< 
    aa::advice< L, ad_target<BI> >,
    typename target_builder_helper<BI, R>::type
  > type;
};

template< typename BI >
struct target_builder_helper< BI, ap::empty_type >
{
  typedef ap::empty_type type;
};


template< typename BI, typename BL >
struct target_builder
{
  typedef typename target_tags_exctract<BL>::type target_tags;
  typedef typename target_builder_helper<BI, target_tags>::type type;
};

/** *******************************/
/**  построитель аспекта binder'a */

template< typename BI, typename BL, typename A >
struct binder_aspect_bulder
{
  typedef typename ap::type_list_traits<BL>::type binder_list;
  typedef ap::type_list<
    typename target_builder<BI, binder_list>::type,
    typename binder_builder<binder_list>::type
  > raw_list;
  typedef aa::aspect< typename ap::organize<raw_list>::type > aspect;

  typedef typename aa::aspect_merge<aspect, A >::type type;
};

}}}

#endif

