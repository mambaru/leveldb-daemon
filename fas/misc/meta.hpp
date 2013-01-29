#ifndef FAS_MISC_META_HPP
#define FAS_MISC_META_HPP

#include <stdexcept>
#include <vector>
#include <string>
#include <set>
#include <map>


#include <fas/pattern/type_list.hpp>


namespace fas{ namespace meta{

namespace ap = ::fas::pattern;

/** Для каждого метатипа создается свой сериализатор */
struct metatype
{
  /// Простые типы 

  /// Все целочисленные от char до long long 
  struct numeric{};
  /// строки STL 
  struct string;
  /// bool
  struct boolean;

  /// Контейнеры
  /// map, multimap, hash_map и т.д
  struct map;
  /// set, multiset, hash_set и т.д
  struct set;
  /// vector, list и т.д
  struct seqence;

  /// Формирующие типы
  struct hash;
  struct array;

  /// Описывает объект
  struct object;

  /// атрибуты объекта
  struct attribute;
  /// свойства объекта
  struct property;

/// Перобразователи
  struct number_string;
  struct time_string;

/// Олд
  struct key;
  struct memberlist;
};

/// Для сериализаторов содержимого 
/// например да a:3:"123" на вход подается 123
struct contenttype
{
  struct boolean;
  struct charaster;
  struct numeric;
  struct string;
};

/// /////////////////////////////////////////////////////

/// Приведен для примера
struct getter;
/// Приведен для примера
struct setter;

template<typename T, typename S>
inline void apply_getset(T&, S);

template<typename T>
inline void apply_getset(T&, ap::empty_type);

template<typename C, typename T, typename S>
inline void apply_getset(C&, T&, S);

template<typename C, typename T>
inline void apply_getset(C&, T&, ap::empty_type);

/// /////////////////////////////////////////////////////

/** @param T тип значения (для каждого типа своя специализация)
    @param G геттер, по умолчанию empty_type (для тестирования можно использовать getter)
    @param S cеттер, по умолчанию empty_type (для тестирования можно использовать setter)
  */
template<typename T, typename G = ap::empty_type, typename S = ap::empty_type>
struct value;

template<typename T, typename G = ap::empty_type, typename S = ap::empty_type>
struct number_string;

template<typename T, typename G = ap::empty_type, typename S = ap::empty_type>
struct time_string;


/// /////////////////////////////////////////////////////

template<typename T>
struct has_metatype;

/** @param T проверяемый тип
  * @param L ментатип или список типов метатипов
  */
template<typename T, typename L>
struct check_meta;

/** @param L - список типов объектов
  * @param T - тип соответсвия metatype или список типов
 */
template<typename L, typename T = ap::empty_type >
struct select_meta;


/// /////////////////////////////////////////////////////
/// /////////////////////////////////////////////////////
/// /////////////////////////////////////////////////////

struct setter
{
  template<typename T> T operator () ( T value ) const  { return value; };
  template<typename C, typename T> T operator () ( C& c, T value ) const { return value; }
};

struct getter
{
  template<typename T> T operator () ( T value ) const { return value; };
  template<typename C, typename T> T operator () ( C&, T value ) const { return value; }
};

template<typename T, typename S>
inline void apply_getset(T& v, S s) { v = s()(v); }

template<typename T>
inline void apply_getset(T&, ap::empty_type){ }

template<typename C, typename T, typename S>
inline void apply_getset(C& c, T& v, S s) { v = s()(c, v); }

template<typename C, typename T>
inline void apply_getset(C&, T&, ap::empty_type) {}


/// /////////////////////////////////////////////////////
/// value специализации

template<typename T, typename G, typename S>
struct value_t
{
  typedef T target;
  typedef G getter;
  typedef S setter;
};

template<typename T, typename G, typename S>
struct value_tn
  : value_t<T, G, S>
{
  typedef metatype::numeric metatype;
};

template<typename G, typename S>
struct value<char, G, S>
  : value_tn<char, G, S> {};

template<typename G, typename S>
struct value<unsigned char, G, S>
  : value_tn<unsigned char, G, S> {};

template<typename G, typename S>
struct value<short, G, S>
  : value_tn<short, G, S> {};

template<typename G, typename S>
struct value<unsigned short, G, S>
  : value_tn<unsigned short, G, S> {};

template<typename G, typename S>
struct value<int, G, S>
  : value_tn<int, G, S> {};

template<typename G, typename S>
struct value<unsigned int, G, S>
  : value_tn<unsigned int, G, S> {};

template<typename G, typename S>
struct value<long, G, S>
  : value_tn<long, G, S> {};

template<typename G, typename S>
struct value<unsigned long, G, S>
  : value_tn<unsigned long, G, S> {};

template<typename G, typename S>
struct value<long long, G, S>
  : value_tn<long long, G, S> {};

template<typename G, typename S>
struct value<unsigned long long, G, S>
  : value_tn<unsigned long long, G, S> {};

template<typename G, typename S>
struct value<bool, G, S>
  : value_t<bool, G, S> 
{
  typedef metatype::boolean metatype;
};

template<typename G, typename S>
struct value<std::string, G, S>
  : value_t<std::string, G, S> 
{
  typedef metatype::string metatype;
};

template<int N, typename G, typename S>
struct value< char[N], G, S >
  : value_t< char[N], G, S> 
{
  typedef metatype::string metatype;
};

template<typename K, typename V, typename A, typename G, typename S>
struct value< std::map<K, V, A>, G, S >
  : value_t< std::map<K, V, A>, G, S>
{
  typedef value_t< std::map<K, V, A>, G, S> super;
  typedef typename super::target target;
  typedef metatype::hash metatype;

  typedef typename target::key_type key_type;
  typedef typename target::mapped_type mapped_type;
  typedef typename target::value_type value_type;

  typedef value<key_type> key_value_type;
  typedef value<mapped_type> mapped_value_type;
};


/// //////////////////////////////////////////////////////////
template<typename T, typename G, typename S>
struct number_string
{
  enum { is_number = ap::static_check< check_meta< value<T>, ap::type_list<metatype::numeric> >::result >::result };

  typedef metatype::number_string metatype;
  typedef T target;
  typedef G getter;
  typedef S setter;
};

template<typename T, typename G, typename S>
struct time_string
{
  enum { is_number = ap::static_check< check_meta< metatype::numeric, value<T> >::result >::result };

  typedef metatype::time_string metatype;
  typedef T target;
  typedef G getter;
  typedef S setter;
};



/// /////////////////////////////////////////////////////
/// вспомогательные инструменты 

template<typename T>
struct has_metatype
{
  struct _big_type_{ char r[2]; };
  struct _small_type_{ char r[1]; };

  template<typename P>
  static _big_type_ test(typename P::metatype const * );

  template<typename P>
  static _small_type_ test( ... );

  enum { result = sizeof( test<T>(0) ) == sizeof(_big_type_) };
};

namespace detail
{
  template<typename T, typename L, bool>
  struct check_metatype;

  template<typename T, typename L>
  struct check_metatype_helper;


  template<typename T, typename L>
  struct check_metatype_helper
  {
    enum {
      /*
      result = ap::conversion<T, typename L::left_type::metatype>::some_type
               || check_metatype_helper< T, typename L::left_type >::result
      */
      result = ap::conversion< typename T::metatype, typename L::left_type >::some_type
               || check_metatype_helper< T, typename L::right_type >::result
    };
  };

  template<typename T>
  struct check_metatype_helper<T, ap::empty_type>
  {
    enum {result = 0};
  };

  template<typename T, typename L>
  struct check_metatype<T, L, true>
  {
    enum { result = check_metatype_helper<T, L>::result };
  };


  template<typename T, typename L>
  struct check_metatype<T, L, false>
  {
    enum { result = 0 };
  };

};

/** @param T проверяемый тип
  * @param L ментатип или список типов метатипов
  */
template<typename T, typename L>
struct check_meta
{
  enum { 
    result = detail::check_metatype< 
      T,
      typename ap::type_list_traits<L>::type, 
      has_metatype<T>::result 
    >::result
  };
};

namespace detail
{
  template<typename T, typename L>
  struct select_meta;

  /// Определяет наличее metatype
  template<typename T, typename L, bool>
  struct select_meta_helper1;

  /// Определяет соответствие metatype
  template<typename T, typename L, bool>
  struct select_meta_helper2;


  template<typename T, typename L>
  struct select_meta_helper2<T, L, true>
  {
    typedef ap::type_list<
       typename L::left_type,
       typename select_meta<T, typename L::right_type>::type
    > type;
  };

  template<typename T, typename L>
  struct select_meta_helper2<T, L, false>
  {
    typedef typename select_meta<T, typename L::right_type>::type type;
  };

  template<typename T, typename L>
  struct select_meta_helper1<T, L, true>
  {
    typedef typename select_meta_helper2<
      T,
      L,
      /*ap::conversion<T, typename L::left_type::metatype>::some_type*/
      check_meta<T, L>::result
      || ap::conversion<T, ap::empty_type>::some_type
    >::type type;
  };

  template<typename T, typename L>
  struct select_meta_helper1<T, L, false>
  {
    typedef typename select_meta<T, typename L::right_type>::type type;
  };

  template<typename T, typename L>
  struct select_meta
  {
     typedef typename select_meta_helper1<T, L, has_metatype< typename L::left_type>::result >::type type;
  };

  template<typename T>
  struct select_meta<T, ap::empty_type>
  {
    typedef ap::empty_type type;
  };
}

template<typename L, typename T  >
struct select_meta
{
  typedef typename ap::type_list_traits<L>::type incoming_list;
  // Исходный список без типов не содержащих metatype
  typedef typename detail::select_meta<ap::empty_type, typename ap::type_list_traits<L>::type >::type common_list;

  // typedef detail::select_meta<ap::empty_type, 
  // Ахтунг! L и T поменяны для detail TODO: исправить в detail
  typedef typename detail::select_meta<T, common_list >::type type;

  // Остаток - что осталось после извлечения типов
  typedef typename ap::erase_from_list<common_list, T, 0>::type margin;
};

/*
namespace detail
{
  template<typename L, typename LL >
  struct select_meta_from_list;

/// ////////////////////////////////////////

  template<typename L, typename LL >
  struct select_meta_from_list
  {
    // формируем список типов используя select_meta для каждого искомого типа
    // полученный список будет иметь нарушенный порядок
  };

};



template<typename L, typename LL >
struct select_meta_from_list
{
  typedef detail::select_meta_from_list<T, typename ap::type_list_traits<L>::type > type;
};
*/























/// /////////////////////////////////////////////////////

/*
struct setter 
{
  template<typename T> T operator () ( T value ) const 
  {
    std::cout<<"set:"<<value<< std::endl; 
    return value;
  };

  template<typename C, typename T, T C::* m> T operator () ( C& c, T value ) const
  {
    return value;
  }

  template<typename C, typename T, T C::* m> T operator () ( C* c, T value ) const
  {
    return value;
  }
};

struct getter 
{
  template<typename T> T operator () ( T value ) const 
  {
     std::cout<<"get:"<<value<< std::endl; return value;
  };

  
  template<typename C, typename T, T C::* m> T operator () ( C& c, T value ) const
  {
    return value;
  }

  template<typename C, typename T, T C::* m> T operator () ( C* c, T value ) const
  {
    return value;
  }
  
};
*/

template<typename N, int n = 1>
struct key
{
  // количество вхождений для данного имени (далее игнорируются)
  enum { limit=n};
  typedef metatype::key metatype;
  typedef N type;
};

struct anykey
{
  enum { limit=-1};
  typedef metatype::key metatype;
  typedef anykey type;
};

struct anyindex
{
  enum { limit=-1};
  typedef metatype::key metatype;
  typedef anykey type;
};

// Любой ключ или индех
struct any
{
  enum { limit=-1};
  typedef metatype::key metatype;
  typedef anykey type;
};

struct ignore
{
  typedef metatype::key metatype;
  enum { limit=0};
  typedef anykey type;
};

template<int i, int n = 1>
struct index
{
  typedef metatype::key metatype;
  enum {value=i, limit=n};
  typedef index<i, n> type;
};


/*

object<
  attribute< Name, member<> >,
  attribute< Name, member<> >,
  property< Name, member<> >
>
aj::member<n_login, authorize, std::string, &authorize::login>,

template<typename N,
         typename T,
         typename M,
         M T::* m,
         typename W = value<M> >
struct member;
*/

/*
  typename С, // класс
  typename T, // тип мембера
  T С::* m    // указатель на мембер, 
  getter, 
  setter,
*/

template<typename C, typename T, T C::* m, typename G = getter, typename S = setter>
struct member
{
  typedef C classtype;
  typedef T target;
  typedef G getter;
  typedef S setter;

  // операторы доступа
  target& operator()(classtype* c) const { return c->*m;}
  const target& operator()(const classtype* c) const  { return c->*m;}

  target& operator()(classtype& c) const { return c.*m;}
  const target& operator()(const classtype& c) const  { return c.*m;}
};

/*
template<
  typename N,
  typename M,
  typename V = value< typename M::value_type >
>
struct attr
{
};*/

/*
template<
  typename N, // либо name<>, либо index<>, иначе name<N>, если в Name не определен специальный тип тип 
  typename V  // value
  typename getter
  typename setter
>
struct attr в общем случае синонимы, но могут интерпретироваться по разному разными сериализаторами 
struct prop



*/



/// /////////////////////////////////////////////////////

///
template<
  typename N,
  typename M,
  typename V = value< typename M::target >
>
struct attr
{
  typedef metatype::attribute metatype;
  typedef N nametype;
  typedef M membertype;
  typedef V valuetype;
  typedef typename membertype::classtype target;
};


template<int i, typename L>
struct select_attrs_by_limit1;

template<int i, typename L, bool>
struct select_attrs_helper;

template<int i>
struct select_attrs_helper<i, ap::empty_type, true> { typedef ap::empty_type type; };

template<int i>
struct select_attrs_helper<i, ap::empty_type, false> { typedef ap::empty_type type; };


template<int i, typename L>
struct select_attrs_helper<i, L, true>
{
  typedef ap::type_list<
    typename L::left_type,
    typename select_attrs_by_limit1<i, typename L::right_type >::type
  > type;
};

template<int i, typename L>
struct select_attrs_helper<i, L, false>
{
  typedef typename select_attrs_by_limit1<i, typename L::right_type >::type type;
};


template<int i, typename L>
struct select_attrs_by_limit1
{
private:
  typedef typename L::left_type left_type;
  typedef typename L::right_type right_type;
public:
  typedef typename select_attrs_helper< i, L, left_type::nametype::limit==i >::type type;
};

template<int i>
struct select_attrs_by_limit1<i, ap::empty_type>
{
  typedef ap::empty_type type;
};

template<int i, typename L>
struct select_attrs_by_limit
{
  typedef typename ap::type_list_traits<L>::type common_list;
  typedef typename select_attrs_by_limit1<i, common_list>::type type;
};




/*

template<typename K, typename T, typename G, typename S>
struct value< std::pair<K,T>, G, S>
{
};

template<typename K, typename T, typename A, typename G, typename S>
struct value< std::map<K,T,A>, G, S>
{
  typedef std::map<K,T,A> target;
};
*/

/**
   value_type
   key_type
   mapped_type
*/
template<typename C /*= std::map<K, T>*/, typename K = value<typename C::key_type>, typename V = value<typename C::mapped_type> >
struct hash
{
  typedef metatype::hash metatype;
  typedef C target;
  typedef K key_value;
  typedef V mapped_value;
  /*
  typedef metatype::hash metatype;
  typedef C target;
  typedef K target_key;
  typedef T target_value;
  typedef typename C::item_type target_item;
  */
};


template<typename T, typename L>
struct fieldlist
{
  typedef T target;
  typedef metatype::memberlist metatype;
  typedef typename ap::type_list_traits<L>::type memberlist;
};

template<typename T, typename L = ap::empty_type/*, typename L2 = ap::empty_type*/ >
struct object
{
  typedef metatype::object metatype;
  typedef T target;
  typedef L memberlist;
  // typedef fieldlist<T, L> memberlist;
};



/// //////////////////////////////////////////////////////// 
/// value
/// /////////////////////////////////////////////////////



}}

#endif
