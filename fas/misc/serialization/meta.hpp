#ifndef FAS_MISC_SERIALIZATION_META_HPP
#define FAS_MISC_SERIALIZATION_META_HPP

#include <fas/pattern/type_list.hpp>
#include <fas/aop/tag.hpp>
#include "tags.hpp"
#include "future.hpp"

#define FASLIB_NAME(X) struct n_##X { const char* operator()() const{ return #X;} };

namespace fas{ namespace serialization{

namespace ap = ::fas::pattern;
namespace aa = ::fas::aop;

struct metatype
{
  struct numeric;
  struct boolean;
  struct real;
  struct string;
  struct symbol;
  struct enumerator;
  struct enumvalue;

  struct object;
  struct array;

	struct fieldlist;
	struct key;
  struct attribute;
  struct property;

  struct comment;
	struct pair;
	struct associative;
	struct sequence;
};

FASLIB_HAS_TYPENAME(has_metatype, metatype);

/** @param T тип значения (для каждого типа своя специализация)
    @param G геттер, по умолчанию empty_type (для тестирования можно использовать getter)
    @param S cеттер, по умолчанию empty_type (для тестирования можно использовать setter)
  */
template<typename T, typename Tg = _default_ >
struct value;


/// ////////////////////////////////////////////////////////////////////
/// Специализации 
/// ////////////////////////////////////////////////////////////////////


/// ////////////////////////////////////////////////////////////////////
/// Специализации для value

// Tg - тег или список тегов
template<typename T, typename Tg>
struct value_t
{
  typedef T target;
  typedef typename aa::tag_list_traits<Tg>::type tag_list;
  typedef typename tag_list::left_type tag;

  // перестраиваем элемент с новым списком тегов
  template<typename TL>
  struct rebind
  {
		typedef value<T, typename ap::merge<tag_list, TL>::type > type;
  };
};

template<typename T, typename Tg>
struct value_tn
  : value_t<T, Tg>
{
  typedef metatype::numeric metatype;
};

template<typename Tg>
struct value<char,Tg>
  : value_tn<char, Tg> {};

template<typename Tg>
struct value<unsigned char, Tg>
  : value_tn<unsigned char,Tg> {};

template<typename Tg>
struct value<short, Tg>
  : value_tn<short, Tg> {};

template<typename Tg>
struct value<unsigned short, Tg>
  : value_tn<unsigned short, Tg> {};

template<typename Tg>
struct value<int, Tg>
  : value_tn<int, Tg> {};

template<typename Tg>
struct value<unsigned int, Tg>
  : value_tn<unsigned int, Tg> {};

template<typename Tg>
struct value<long, Tg>
  : value_tn<long, Tg> {};

template<typename Tg>
struct value<unsigned long, Tg>
  : value_tn<unsigned long, Tg> {};

template<typename Tg>
struct value<long long, Tg>
  : value_tn<long long, Tg> {};

template<typename Tg>
struct value<unsigned long long, Tg>
  : value_tn<unsigned long long, Tg> {};

template<typename Tg>
struct value<bool, Tg>
  : value_t<bool, Tg> 
{
  typedef metatype::boolean metatype;
};

template<typename Tg>
struct value<std::string, Tg>
  : value_t<std::string, Tg> 
{
  typedef metatype::string metatype;
};

template<int N, typename Tg>
struct value< char[N], Tg >
  : value_t< char[N], Tg> 
{
  typedef metatype::string metatype;
};


template<typename C, typename T, T C::* m>
struct member
{
  typedef C classtype;
  typedef T target;

  target& operator()(classtype& c) const { return c.*m;}
  const target& operator()(const classtype& c) const  { return c.*m;}
};

// ML - member_list
template<typename ML>
struct aggregation
{
	typedef typename ap::type_list_traits<ML>::type aggr_path;

	typedef typename ap::type_list_nth_cast<0, aggr_path>::type::left_type first_member;
	typedef typename ap::type_list_nth_cast< ap::length<aggr_path>::result-1, aggr_path>::type::left_type last_member;

	typedef typename first_member::classtype classtype;
	typedef typename last_member::target target;

  target& operator()(classtype& c) const 
	{
		 return _get< typename aggr_path::left_type>(c, typename  aggr_path::right_type() );
	}

  const target& operator()(const classtype& c) const 
	{
		 return _get< typename aggr_path::left_type>(c, typename  aggr_path::right_type() );
	}

private:

	template<typename T, typename C, typename L>
  target& _get( C& c, L ) const
  {
		return _get<typename L::left_type>( T()(c), typename L::right_type() );
  }

	template<typename T, typename C>
  target& _get( C& c, ap::empty_type ) const
  {
		return T()(c);
  }

	template<typename T, typename C, typename L>
  const target& _get( const C& c, L ) const
  {
		return _get<typename L::left_type>( T()(c), typename L::right_type() );
  }

	template<typename T, typename C>
  const target& _get( const C& c, ap::empty_type ) const
  {
		return T()(c);
  }
};


/// //////////////////////////////////////////////

template<typename N>
struct key: N
{
  typedef typename aa::tag_list_traits<_default_>::type tag_list;
  typedef typename tag_list::left_type tag;

	typedef metatype::key metatype;
  // TODO: тип получить из возвращаемого значения K::operator ()
  typedef const char* target;
};

template<typename K>
struct key_traits
{
	typedef key<K> type;
};

template<
  typename N,
  typename M,
  typename V = value< typename M::target >,
  typename Tg = _default_
>
struct attr
{
  typedef metatype::attribute metatype;
	typedef typename key_traits<N>::type keytype;
  typedef M membertype;
  typedef V valuetype;
  typedef typename aa::tag_list_traits<Tg>::type tag_list;
  typedef typename tag_list::left_type tag;
	typedef typename membertype::classtype target;
};

template<
  typename N,
  typename M,
  typename V = value< typename M::target >,
	typename Tg = _default_
>
struct property
{
  typedef metatype::property metatype;
  typedef N nametype;
  typedef M membertype;
  typedef V valuetype;
  typedef typename aa::tag_list_traits<Tg>::type tag_list;
  typedef typename tag_list::left_type tag;
};

template<typename T, typename L = ap::empty_type, typename Tg = _default_ >
struct fieldlist
{
	typedef metatype::fieldlist metatype;
	typedef T target;
	typedef L field_type_list;
  typedef typename aa::tag_list_traits<Tg>::type tag_list;
  typedef typename tag_list::left_type tag;
};


template<typename FL, typename Tg = _default_ >
struct object
{
  typedef metatype::object metatype;
  typedef typename FL::target target;
  typedef FL fieldlist;
  typedef typename aa::tag_list_traits<Tg>::type tag_list;
  typedef typename tag_list::left_type tag;
};


template<typename T, typename VF = value<typename T::first_type>, typename VS = value<typename T::second_type>,  typename Tg = _default_>
struct pair
{
  typedef metatype::pair metatype;
  typedef T target;
  typedef VF first_valuetype;
  typedef VS second_valuetype;
  typedef typename aa::tag_list_traits<Tg>::type tag_list;
  typedef typename tag_list::left_type tag;
};


template<typename T, typename V = pair<typename T::value_type>, typename Tg = _default_>
struct hash
{
  typedef V valuetype;
  typedef metatype::associative metatype;
  typedef T target;
  typedef typename aa::tag_list_traits<Tg>::type tag_list;
  typedef typename tag_list::left_type tag;
};

template<typename N, typename T, T v, typename Tg = _default_ >
struct enum_value
{
  typedef metatype::enumvalue metatype;
  typedef T target;
  T operator()() const { return v;}
  typedef N name;
  // ТОDO: получить возвращаемое значение
  // const char* operator()() const { return N::operator()(); }
  typedef typename aa::tag_list_traits<Tg>::type tag_list;
  typedef typename tag_list::left_type tag;
};

template< typename T, typename L, typename Tg = _default_ >
struct enumerator
{
  typedef metatype::enumerator metatype;
  typedef T target;
  typedef typename ap::type_list_traits<L>::type enum_list;

  typedef typename aa::tag_list_traits<Tg>::type tag_list;
  typedef typename tag_list::left_type tag;
};


}}

#endif