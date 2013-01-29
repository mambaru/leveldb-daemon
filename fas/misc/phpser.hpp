#ifndef FAS_MISC_PHPSER_HPP
#define FAS_MISC_PHPSER_HPP

#include <stdexcept>
#include <vector>
#include <string>
#include <set>
#include <map>

#include <fas/aop/aspect.hpp>

#include <fas/pattern/type_list.hpp>


namespace fas{ namespace phpser{

namespace aa = ::fas::aop;
namespace ap = ::fas::pattern;


template<typename V>
struct deserializer;

struct deserializer_builder
{
  template<typename V, typename A>
  struct build
  {
    typedef deserializer<V> type;
  };
};

struct _default_{};
struct _deserializer_{};

struct default_aspect:
  aa::aspect<
    aa::advice<_deserializer_, deserializer_builder>
  >
{};

template<typename A>
struct rule
{
  typedef A aspect;
};

struct default_rule: rule<default_aspect> {};

typedef aa::aspect<
    ap::type_list_n<
      aa::advice<_default_, default_rule>
    >::type
  >
main_aspect;

template<typename Tr, typename Tg, typename A, bool >
struct rule_serach_helper;

template<typename Tr, typename A, typename AA, bool B1, bool B2 >
struct rule_serach_helper2;



template<typename Tr, typename Tg, typename A >
struct rule_serach_helper<Tr, Tg, A, true>
{
  typedef typename aa::advice_cast<Tg, A>::type rule_advice;
  typedef typename rule_advice::aspect rule_aspect;
  typedef typename rule_serach_helper2< Tr, rule_aspect, A, true, ap::type_count<Tr, rule_aspect>::result >::type type;
};

template<typename Tr, typename Tg, typename A >
struct rule_serach_helper<Tr, Tg, A, false>
{
  typedef typename aa::advice_cast<_default_, A>::type rule_advice;
  typedef typename rule_advice::aspect rule_aspect;
  typedef typename rule_serach_helper2< Tr, rule_aspect, A, false, ap::type_count<Tr, rule_aspect>::result >::type type;
};

template<typename Tr, typename A, typename AA, bool B1, bool B2 >
struct rule_serach_helper2
{
  enum { rule_not_found = ap::static_check<B1||B2>::result };
};

/** Поиск правила в заданном наборе. 
    (парвило найдено в текущем наборе)
  */
template<typename Tr, typename A, typename AA>
struct rule_serach_helper2<Tr, A, AA, true, true>
{
  typedef typename aa::advice_cast<Tr, A>::type type;
};

/** Поиск правила в заданном наборе. 
    (парвило не найдено в текущем наборе ищем в _default_)*/
template<typename Tr, typename A, typename AA>
struct rule_serach_helper2<Tr, A, AA, true, false>
{
  typedef typename rule_serach_helper<Tr, _default_, AA, true >::type type;
};

/** Поиск правила в заданном наборе. 
    (парвило найдено в наборе в _default_)*/
template<typename Tr, typename A, typename AA>
struct rule_serach_helper2<Tr, A, AA, false, true>
{
    typedef typename aa::advice_cast<Tr, A>::type type;
};

	/*! Поиск правила по заданному тегу набора правил, и тегу правила.
   *  Сначала ищет правило в наборе Tg, затем в наборе _default_
   *  если не нашел, то ошибка компиляции. Если тега Tg в аспекте наборов
   *  нет, то сразу в _default_
   * @tparam Tr - тег правила 
   * @tparam Tg - тег набора правил
   * @tparam A  - аспект наборов правил
  */
template<typename Tr, typename Tg, typename A >
struct rule_serach
{
  typedef typename rule_serach_helper<Tr, Tg, A, ap::type_count<Tg, A>::result >::type type;
};

template<typename A = aa::aspect<> >
struct aspect_class
{
public:
  typedef typename aa::aspect_merge<A, main_aspect>::type aspect;

  template<typename Tr = _deserializer_, typename Tg = _default_ >
  struct builder
  {
		typedef typename rule_serach<Tr, Tg, aspect>::type type;
		/*
    typedef typename aa::advice_cast<Tg, aspect>::type rule_advice;
    typedef typename rule_advice::aspect rule_aspect;
    typedef typename aa::advice_cast<Tr, rule_aspect>::type target_advice;
    typedef target_advice type;
    */
  };

  template<typename V, typename AA, typename Tg = _default_ >
  struct deserializer
  {
    typedef typename builder<_deserializer_, Tg>::type::template build<V, AA>::type type;
  };
};

// A::builder<>::type::build<V, A>::type;



template<typename, typename, typename A = aspect_class<> /*template<typename> class = deserializer*/ >
struct deserializer_t;

template<typename V>
struct deserializer:
  public deserializer_t< typename V::metatype, V>
{
};


}}

#include "phpser/phpser_parser.hpp"
#include "phpser/phpser_value.hpp"
#include "phpser/phpser_string.hpp"
#include "phpser/phpser_field.hpp"
#include "phpser/phpser_object.hpp"


#endif
