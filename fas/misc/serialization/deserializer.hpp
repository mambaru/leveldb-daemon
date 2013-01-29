#ifndef FAS_MISC_SEREALIZETION_DESERIALIZER_HPP
#define FAS_MISC_SEREALIZETION_DESERIALIZER_HPP

#include "ranges.hpp"
#include <fas/aop/aspect.hpp>
#include <fas/pattern/type_list.hpp>
#include "tags.hpp"

namespace fas{ namespace serialization{

namespace ap = ::fas::pattern;
namespace aa = ::fas::aop;

template<typename V>
struct setter
{
	typedef typename V::target target;
	void operator()(target& v) {};
};

/// заглушка - неделает ничего
template<typename V>
struct deserializer_t
{
	// В реализации специализация по target
  typedef typename V::target target;
  template<typename T, typename R>
  R operator()(T& t, target& v, R r)
	{
    return r;
  }
};

/*template_holder*/
template<template<typename> class T>
struct tbox
{
  template<typename V>
	struct build
  {
    typedef T<V> type;
  };
};

template<typename A>
struct box
{
  typedef A aspect;
};

struct deserializer_aspect
	: aa::aspect<
			ap::type_list_n<
				aa::advice< _deserializer_, tbox<deserializer_t> >,
				aa::advice< _setter_, tbox<setter> >
			>::type
    > {};

struct default_aspect
	: aa::aspect<
			aa::advice< _default_, box<deserializer_aspect> >
    >
{
};

namespace detail
{
  // Tr - тег елемента который ищется в аспектах TL
  // TL - сисок тегов аспектов где осуществляется поиск

	template<typename Tr, typename TL, typename A>
	struct search_helper;

  // Если аспект найден или нет 
	template<typename Tr, typename TL, typename A, bool >
	struct search_helper1;

  // Если елемент в аспект найден или нет 
	template<typename Tr, typename TL, typename A/*, typename AA*/, bool >
	struct search_helper2;

	template<typename Tr, typename TL, typename A>
	struct search_helper
  {
    typedef typename TL::left_type tag_aspect;
		typedef typename search_helper1< Tr, TL, A, A::template has<tag_aspect>::result  >::type type;
  };

	template<typename Tr, typename A>
	struct search_helper<Tr, ap::empty_type, A>
  {
		typedef ap::static_check<0> type;
		// ap::static_check<0> element_not_found;
  };

	template<typename Tr, typename TL, typename A >
	struct search_helper1<Tr, TL, A, false>
  {
    // Аспект не найден, продолжаем поиск
		typedef typename TL::right_type tag_list;
    typedef typename search_helper<Tr, tag_list, A>::type type;
  };

	template<typename Tr, typename TL, typename A >
	struct search_helper1<Tr, TL, A, true>
  {
		// Аспект нашли ищем элемент 
		typedef typename TL::left_type tag_aspect;
    typedef typename aa::advice_cast<tag_aspect, A>::type::aspect desired_aspect;
    typedef typename search_helper2<Tr, TL, A, /*desired_aspect,*/ desired_aspect::template has<Tr>::result >::type type;
  };

	template<typename Tr, typename TL, typename A>
	struct search_helper2<Tr, TL, A, true>
  {
		// Супер! элемент нашли - заканчиваем поиск
		typedef typename TL::left_type tag_aspect;
    typedef typename aa::advice_cast<tag_aspect, A>::type::aspect desired_aspect;
    typedef typename aa::advice_cast<Tr, desired_aspect>::type type;
  };

	template<typename Tr, typename TL, typename A>
	struct search_helper2<Tr, TL, A, false>
  {
		// Подстава, элемента нет, ищем дальше
		typedef typename TL::right_type tag_list;
    typedef typename search_helper<Tr, tag_list, A>::type type;
  };

}

template<typename Tg, typename TL, typename A>
struct search_indep
{
	typedef typename detail::search_helper<Tg, TL, A>::type type;
};

template<typename V, typename A = aa::aspect<> >
struct deserializer
	: aa::aspect_class<default_aspect, A>
{
  typedef typename V::target target;
  typedef aa::aspect_class<default_aspect, A> super;
  typedef typename super::aspect aspect;

  template< typename VV, typename Tg = _deserializer_ >
  struct build
  {
    typedef typename VV::tag_list tag_list;
		typedef typename detail::search_helper<Tg, tag_list, aspect>::type th;
    typedef typename th::template build<VV>::type type;
  };

	template<typename P>
  P operator()(target& v, P beg, P end)
  {
    return build<V>::type()(*this, v, forward_range<P>(beg, end) ).begin();
  }

	template<typename T>
  T operator()(target& v, T range)
  {
		return typename build<V>::type()(*this, v, range);
	}

};


}}

#endif
