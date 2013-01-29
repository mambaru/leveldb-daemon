#ifndef FAS_MISC_SEREALIZETION_PHPSER_DESERIALIZER_HPP
#define FAS_MISC_SEREALIZETION_PHPSER_DESERIALIZER_HPP

#include "../ranges.hpp"
#include <fas/aop/aspect.hpp>
#include <fas/pattern/type_list.hpp>
#include "../tags.hpp"
#include "../deserializer.hpp"
#include "../future.hpp"
#include "parser.hpp"

namespace fas{ namespace serialization{ namespace phpser{

namespace ap = ::fas::pattern;
namespace aa = ::fas::aop;

struct _type_deserializer_{};

template<typename V>
struct deserializer_t;

/*
struct type_category
{
  struct array;    // массивы c++
  struct numeric; // все челочисленные типы
  struct string;
  struct wstring;
  struct real;
};

template<typename T>
struct category;

template<>
struct category<int> { typedef type_category::numeric type; };

template<typename K, typename T>
struct type_deserializer_impl;

template<typename T>
struct type_deserializer_impl< type_category::numeric, T>
{
  template<typename R>
  R operator ()(T& v, R r)
  {
    return parser::unserialize_integer(v, r);
  }
};
*/


struct type_deserializer_integer
{
  template<typename T, typename R>
  R operator ()(T& v, R r, size_t)
  {
    return parser::unserialize_integer(v, r);
  }
};

template<typename T>
struct type_deserializer_impl;

template<> struct type_deserializer_impl<int> : type_deserializer_integer{};
template<> struct type_deserializer_impl<unsigned int> : type_deserializer_integer{};

template<> 
struct type_deserializer_impl<std::string>
{
  template<typename R>
  R operator ()(std::string& v, R r, size_t s)
  {
    v.reserve(s);
    if (!r) throw unexpected_end_fragment();
    ++r;
    for ( ;s; --s, ++r  ) v.push_back(*r);
    if (!r) throw unexpected_end_fragment();
    ++r;
    return r;
  }
};

template<typename V>
struct type_deserializer
  // : type_deserializer_impl< typename category<typename V::target>::type, typename V::target >
  : type_deserializer_impl< typename V::target >
{
};


template<typename MT, typename V>
struct deserializer_impl;

template<typename V>
struct deserializer_impl<metatype::numeric, V>
{
  typedef typename V::target target;
  template<typename T, typename R>
  R operator()(T& t, target& v, R r)
  {
    if ( parser::is_int(r) )
    {
      r = parser::parse_typename<'i'>(r);
      typedef typename T::template build<V, _type_deserializer_>::type type_deserializer;
      r = type_deserializer()( v, r, 0);
      typedef typename T::template build<V, _setter_>::type setter;
      setter()(v);
    }
    else if ( parser::is_string(r) )
    {
      R income = r;
      r = parser::parse_typename<'s'>(r);
      ++r;
      typedef typename T::template build<V, _type_deserializer_>::type type_deserializer;
      r = type_deserializer()( v, r, 0);
      r = parser::parse_string(income);
      typedef typename T::template build<V, _setter_>::type setter;
      setter()(v);
    }
    return r;
  }
};

template<typename V>
struct deserializer_impl<metatype::string, V>
{
  typedef typename V::target target;
  template<typename T, typename R>
  R operator()(T& t, target& v, R r)
  {
    size_t s = 0;
    r = parser::parse_typename<'s'>(r, &s);
    typedef typename T::template build<V, _type_deserializer_>::type type_deserializer;
    r = type_deserializer()( v, r, s);
    typedef typename T::template build<V, _setter_>::type setter;
    setter()(v);
    return r;
  }
};

template<typename V>
struct deserializer_impl<metatype::key, V>
{
  // TODO: сделать специализации для разных типов ключей
  typedef typename V::target target;

/*  template<typename T>
  void test(T& t, const char* v){};*/
  template<typename T, typename R>
  R operator()(T& t, const char* v, R r)
  {
    R income = r;
    if ( parser::is_string(r) )
    {
      size_t s = 0;
      r = parser::parse_typename<'s'>(r, &s);
      if (!r || *r!='"') return income;
      ++r;
      for ( ; r && *v!='\0' && *r==*v ; ++r, ++v );
      if ( !r || *r!='"' || *v!='\0' ) return income;
      ++r;
      return r;
    }
    return income;
  }
};


template<typename V>
struct deserializer_impl<metatype::attribute, V>
{
  // typedef typename V::classtype classtype;
  typedef typename V::target target;
  typedef typename V::keytype keytype;
  typedef typename V::membertype membertype;
  // typedef deserializer_t<keytype> key_deserializer;
  typedef typename V::valuetype valuetype; // мета
  // typedef deserializer_t<valuetype> value_deserializer;

  template<typename T, typename R>
  R operator()(T& t, target& v, R r)
  {
    typedef typename T::template build<keytype, _deserializer_>::type key_deserializer;
    typedef typename T::template build<valuetype, _deserializer_>::type value_deserializer;

    R income = r;
    //key_deserializer kd;
    /*ap::static_check< ap::template_conversion1<key_deserializer, ::fas::serialization::phpser::deserializer_t >::result >();
    kd.test(t, keytype()());*/
//    kd.key();
    r = key_deserializer()(t, keytype()(), r);
    if ( r == income ) return income;
    r = parser::parse_space(r);
    if ( !r || *r!=';' ) return income;
    ++r;
    r = parser::parse_space(r);
    if ( !r ) return income;
    R current = r;
    r = value_deserializer()( t, membertype()(v), r);
    if ( r == current ) return income;
    r = parser::parse_space(r);
    if ( r && *r==';' ) ++r;
    return r;
  };
};

template<typename V>
struct deserializer_impl<metatype::object, V>
{
  
  typedef typename V::target target;
  typedef typename V::fieldlist fieldlist;
  template<typename T, typename R>
  R operator()(T& t, target& v, R r)
  {
    typedef typename T::template build<fieldlist, _deserializer_>::type deserializer;
    R income = r;
    size_t s;
    r = parser::parse_typename<'a'>(r, &s);
    if ( !r || r == income ) return income;
    r = parser::parse_space(r);
    if (  !r || *r!='{' ) return income;
    ++r;
    r = parser::parse_space(r);
    if ( !r ) return income;
    r = deserializer()(t, v, r);
    r = parser::parse_space(r);
    if ( !r ) throw unexpected_end_fragment();
    if ( *r != '}' ) throw expected_of('}', r);
    ++r;
    return r;
  }
};

template<typename V, typename L>
struct deserializer_fieldlist
  : deserializer_fieldlist<V, typename L::right_type>
{
  typedef deserializer_fieldlist<V, typename L::right_type> super;
  typedef typename L::left_type fieldtype;
  typedef typename V::target target;

  template<typename T, typename R>
  R operator()(T&t, target& v, R r)
  {
    typedef typename T::template build<fieldtype, _deserializer_>::type deserializer;
    r = parser::parse_space(r);
    if ( r && *r=='}' ) return r;
    r = deserializer()(t, v, r);
    return r ? super::operator()(t, v, r) : r;
  }; 
};

template<typename V>
struct deserializer_fieldlist<V, ap::empty_type>
{
  typedef typename V::target target;
  template<typename T, typename R>
  R operator()(T&, target&, R r) { return r;}
};

template<typename V>
struct deserializer_impl<metatype::fieldlist, V>
{
  typedef typename V::target target;
  typedef typename V::field_type_list fieldlist_type;
  typedef typename V::metatype metatype_type;
  typedef deserializer_fieldlist<V, fieldlist_type> deserializer;

  typedef ap::type_list_n< metatype::attribute, metatype::property >::type metalist;
  typedef ap::select_from<metalist, fieldlist_type, has_metatype> select;
  typedef typename select::type prop_list;
  typedef typename select::margin other_list;

  typedef deserializer_fieldlist<V, prop_list> prop_deserializer;
  typedef deserializer_fieldlist<V, other_list> other_deserializer;

  template<typename T, typename R>
  R operator()(T& t, target& v, R r)
  {
//    deserializer d1;
    prop_deserializer d1;
    other_deserializer d2;
    while( r )
    {
      R current = r;
      r = d1(t, v, r);
      if ( !r || *r=='}') return r;
      if ( current == r )
      {
        r = d2(t, v, r);
        if ( !r || *r=='}') return r;
        if ( current == r )
        {
          r = parser::parse_member(r);
          if ( current == r )
            return r;
        }
      }
    }
    return r;
  }
};

template<typename V>
struct deserializer_impl<metatype::pair, V>
{
  typedef typename V::first_valuetype first_valuetype;
  typedef typename V::second_valuetype second_valuetype;
  typedef typename V::target target;

  template<typename T, typename R>
  R operator()(T& t, target& v, R r)
  {
    R income = r;
    typedef typename T::template build<first_valuetype, _deserializer_>::type first_deserializer;    
    r = first_deserializer()(t, v.first, r);
    if ( r == income ) return income;
    r = parser::parse_space(r);
    if (!r || *r!=';') return income;
    r = parser::parse_space(++r);
    R current = r;
    typedef typename T::template build<second_valuetype, _deserializer_>::type second_deserializer;    
    r = second_deserializer()(t, v.second, r);
    if ( r == current ) return income;
    r = parser::parse_space(r);
    if ( r && *r==';') ++r;
    return r;
  }
};

// Выгребает только одно вхождение
template<typename V>
struct deserializer_impl<metatype::associative, V>
{
  typedef typename V::target target;
  typedef typename V::valuetype valuetype;
  typedef typename target::value_type value_type;
  template<typename T, typename R>
  R operator()(T& t, target& v, R r)
  {
    typedef typename T::template build<valuetype, _deserializer_>::type deserializer;
    R income = r;
    r = parser::parse_space(r);
    // не понятно почему здаесь ключ не констнтный?? в value_type он костантный - поэтому не компилится
    typename valuetype::target vt;
    // std::pair<typename target::key_type, typename target::mapped_type > vt;
    deserializer d;
    r = d(t, vt, r);
    if ( r == income) return income;
    v.insert(vt);
    return r;
  }
};

template<typename V>
struct deserializer_impl<metatype::enumvalue, V>
{
  typedef typename V::target target;
  typedef typename V::name name;
  template<typename T, typename R>
  R operator()(T& t, target& v, R r)
  {
    R income = r;
    
    r = check(r, string_range<const char*>( name()() ) );
    if ( r == income) return income;
    v = V()();
    return r;
  }
private:
  template<typename R, typename RN>
  R check(R r, RN rn)
  {
    R income = r;
    for ( ;r && rn && *r!='\"'; ++r, ++rn )
      if ( *r!=*rn ) return income;
    if (rn || !r || *r!='\"') return income;
    return r;
  }
};

template<typename V>
struct deserializer_impl<metatype::enumerator, V>
{
  typedef typename V::target target;
  typedef typename V::enum_list enumlist;

  template<typename T, typename R>
  R operator()(T& t, target& v, R r)
  {
    R income = r;
    r = parser::parse_typename<'s'>(r);
    if (!r || *r!='\"') return income;
    ++r;
    R current = r;
    r = check(t, v, r, enumlist() );
    if (current == r) return income;
    if (!r || *r!='\"') return income;
    ++r;
    return r;
  }

private:

  template<typename T, typename R, typename L>
  R check(T& t, target& v, R r, L)
  {
    typedef typename L::left_type left_type;
    typedef typename L::right_type right_type;

    typedef typename T::template build<left_type, _deserializer_>::type deserializer;
    R income = r;
    r = deserializer()(t, v, r);
    if ( income != r)
      return r;
    return check(t, v, income, right_type());
  }

  template<typename T, typename R>
  R check(T&, target& v, R r, ap::empty_type)
  {
    return r;
  }
};




/// заглушка - неделает ничего
template<typename V>
struct deserializer_t
  : deserializer_impl< typename V::metatype, V>
{
};



struct deserializer_aspect
  : aa::aspect<
      ap::type_list_n<
        aa::advice< _deserializer_, tbox<deserializer_t> >,
        aa::advice< _type_deserializer_, tbox<type_deserializer> >,
        aa::advice< _setter_, tbox<setter> >
      >::type
    > {};

struct default_aspect
  : aa::aspect<
      aa::advice< _default_, box<deserializer_aspect> >
    >
{
};

template<typename V, typename A = aa::aspect<> >
struct deserializer
  : public ::fas::serialization::deserializer<V, typename aa::aspect_merge<default_aspect, A>::type >
{
};


}}}

#endif
