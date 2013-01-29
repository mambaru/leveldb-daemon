// C++ Implementation: muti_type
//
// Description: 
//
//
// Author: Migashko Vladimir <vladimir@migashko.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

//#include <fas/util/multi_type.hpp>

#ifndef FAS_MISC_VARIANT_HPP
#define FAS_MISC_VARIANT_HPP

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>

#include <fas/aop/aspect.hpp>
#include <fas/aop/advice.hpp>

#include <stdlib.h>
#include <stdio.h>

namespace fas{ namespace misc{

namespace aa = ::fas::aop;
namespace ap = ::fas::pattern;

template<typename V>
struct vt
{
  template<typename T>
  void set(T& t, const V& v) 
  { 
    t._assign(v); 
  }

  template<typename T>
  void get(T& t, bool& u) const 
  { 
    u = t.template _get_data<bool>();
  }


  
  /*template<typename T>
  void get(T& t, V& v) const 
  {
    v = t.template _get_data<V>();
  }*/

  template<typename T>
  void get(T& t, std::string& s) const 
  { 
    V v = t.template _get_data<V>();

    do
    {
      s.push_back('0' + static_cast<char>(v%10) );
      v/=10;
    } 
    while (v != 0);

    std::reverse(s.begin(), s.end());
  }

  
  template<typename T, typename U>
  void get(T& t, U& u) const 
  { 
    V v = t.template _get_data<V>();
    u = static_cast<U>(v);
  }


/*
  template<typename T>
  void get(T&, ... ) const {};
*/
};

template<>
template<typename T>
void vt<double>::get(T& t, std::string& s) const 
{
  char tmp[32];
#ifdef WIN32
  _snprintf_s(tmp, 32, "%f", t.template _get_data<double>() );
#else
  snprintf(tmp, 32, "%f", t.template _get_data<double>() );
#endif
  s = tmp;
}


template<>
template<typename T>
void vt<bool>::get(T& t, std::string& s) const 
{
  if ( t.template _get_data<bool>() )
    s = "1";
  else
    s = "0";
}



template<>
struct vt<std::string>
{
  template<typename T>
  void set(T& t, const std::string& v) { t._assign(v); }

  template<typename T>
  void set(T& t, const char* v) { t._assign( std::string(v) ); }

  template<typename T>
  void get(T& t, std::string& v) const { v = t.template _get_data<std::string>();}

  template<typename T>
  void get(T& t, bool& u) const 
  { 
    std::string v = t.template _get_data<std::string>();
    u = (v!="0");
  }


  template<typename T>
  void get(T& t, float& u) const 
  { 
    std::string v = t.template _get_data<std::string>();
    u = std::atof( v.c_str() );
  }

  template<typename T>
  void get(T& t, double& u) const 
  { 
    std::string v = t.template _get_data<std::string>();
    u = std::atof( v.c_str() );
  }

  template<typename T, typename U>
  void get(T& t, U& u) const 
  { 
    std::string v = t.template _get_data<std::string>();
    u = std::atoi( v.c_str() );
  }

  /*template<typename T>
  void get(T&, ... ) const {};*/
};



template<typename A>
class variant
  : aa::aspect_class<A>
{
  typedef variant<A> self;
  typedef aa::aspect_class<A> super;

  std::vector<char> _data;
  int _marker;

public:
  variant():_marker(-1){}

  template<typename T>
  void _assign( const T& v )
  {
    _data.resize(sizeof(T));
    new (&_data[0])T(v);
  }
  
  template<typename T>
  const T& _get_data( ) const {  return *reinterpret_cast<const T*>(&_data[0]);  }

  /*template<typename T>
  T& _get_data( ) {  return *reinterpret_cast<const T*>(&_data[0]);  }*/

public:
  bool is_null() const
  {
    return _marker==-1;
  }

  void set_null()
  {
    _data.clear();
    _marker  = -1;
  }

  template<typename T>
  bool is() const
  {
    return _marker == ap::type_position< ap::type2type<T>, typename super::type_list_type >::result;
  }

  template<typename T>
  variant& operator = (const T& v)
  {
    super::template get< ap::type2type<T> >().set(*this, v);
    _marker = ap::type_position< ap::type2type<T>, typename super::type_list_type >::result;
    return *this;
  }

  template<typename T>
  operator T () const 
  {
    typedef typename super::advice_list advice_list;
    return this->template _select<T>( advice_list() );
  }

  template<typename T>
  T _select(ap::empty_type) const { return T(); }
  
  template<typename T, typename U>
  T _select(U ) const
  {
    if ( ap::type_position<typename U::left_type, typename super::type_list_type>::result == _marker)
    {
      T res;
      super::template get<typename U::left_type>().get(*this, res );
      return res;
    }
    return _select<T>( typename U::right_type() );
  }
//***********************
  /*template<typename T>
  operator T& () { return _select<T>( super::advice_list() ); }

  template<typename T>
  T& _select(ap::empty_type) { throw; }
  
  template<typename T, typename U>
  T& _select(U& t)
  {
    if ( ap::type_position<U::left_type, super::type_list_type>::result == _marker)
    {
      return *reinterpret_cast<const T*>(&_data[0]); 
 
    }
    return _select<T>( typename U::right_type() );
  }*/

};


template< typename T >
class ptr_holder
{
public:
  typedef T type;
  ~ptr_holder(){delete _value;}
  ptr_holder():_value(0){};
  ptr_holder(const ptr_holder<T>& ph)
  {
    if (ph._value==0)
      _value = 0;
    else
    {
      _value = new T( *ph._value) ;
    }
  }
  type* get_ptr()
  {
    if (_value==0) _value = new T;
    return _value;
  }
  const type* get_ptr() const
  {
    /*if (_value==0)
      throw;*/
    return _value;

    /*
    if (_value==0) _value = new T;
    return *_value;
    */
  }
  type& operator* () { return *this->get_ptr();}
  const type& operator* () const { return *this->get_ptr();}
  type* operator -> () { return this->get_ptr();}
  const type* operator -> () const { return this->get_ptr();}
private:
  type* _value;
};


template<
  typename T,
  typename V,
  template<typename> class L,
  template<typename> class A,
  template <typename,typename,typename,typename> class M = std::map
>
struct make_variant_map_advice_list
{
  /// 1. 
  typedef typename T::left_type key_type;
  typedef V mapped_type;
  typedef L< key_type > less_type;
  typedef A< std::pair<key_type, mapped_type> > allocator_type;
  typedef M<key_type, mapped_type, less_type,  allocator_type> map_type;
  typedef aa::advice< ap::type2type<key_type>, ptr_holder< map_type > > advice_type;

  /// 2
  typedef typename T::right_type right_list;
  typedef typename make_variant_map_advice_list<right_list, V, L, A, M>::type right_advice_list;

  /// 3
  typedef ap::type_list<
    advice_type,
    right_advice_list
  > type;
};

template<
  typename V,
  template<typename> class L,
  template<typename> class A,
  template <typename,typename,typename,typename> class M
>
struct make_variant_map_advice_list<ap::empty_type, V, L, A, M>
{
  typedef ap::empty_type type;
};

template<typename T>
struct key_traits
{
  typedef ap::type2type<T> key_type;
  typedef T type;
};

template<int C>
struct key_traits<const char[C]>
{
  typedef ap::type2type<std::string> key_type;
  typedef const char type[C];
};

template<>
struct key_traits<const char*>
{
  typedef ap::type2type<std::string> key_type;
  typedef const char* type;
};

template<
  typename A,  
  typename KL = ap::type_list_n<int, std::string>::type, 
  template <typename,typename,typename,typename> class M = std::map,
  template <typename> class L = std::less,
  template <typename> class AL = std::allocator
  >
class variant_map
  : public variant<A>
  , public aa::aspect_class< 
      aa::aspect<
        typename make_variant_map_advice_list<
          KL,
          variant_map<A, KL, M, L, AL>,
          L, AL, M
        >::type 
      > 
    >
{

public:
  typedef variant_map<A, KL, M, L, AL> self;

  typedef variant<A> super_variant;
  typedef aa::aspect_class< 
      aa::aspect<
        typename make_variant_map_advice_list<
          KL,
          variant_map<A, KL, M, L, AL>,
          L, AL, M
        >::type 
      > 
    > super_map;

  typedef typename super_map::aspect map_aspect;

  template<typename K>
  struct mapped
  {
    typedef typename aa::advice_cast< typename key_traits<K>::key_type, map_aspect>::type::type::mapped_type type;
  };

  template<typename K>
  typename aa::advice_cast< typename key_traits<K>::key_type, map_aspect>::type::type::mapped_type& 
  operator[] (K key)
  {
    typedef typename aa::advice_cast<typename key_traits<K>::key_type, map_aspect>::type holder_type;
    return static_cast<holder_type*>(this)->get_ptr()->operator[](key);
  }

  template<typename K>
  const typename aa::advice_cast< typename key_traits<K>::key_type, map_aspect>::type::type::mapped_type& 
  operator[] (K key) const 
  {
    typedef typename aa::advice_cast<typename key_traits<K>::key_type, map_aspect>::type holder_type;
    return static_cast<const holder_type*>(this)->get_ptr()->find(key)->second;
  }

  /*
  template<typename T>
  operator T () const { return super_variant::operator T();_select<T>( super::advice_list() ); }
*/

/*
  template<typename K>
  typename aa::advice_cast<ap::type2type<K>, map_aspect>::type::type::mapped_type& operator[] (const K& key)
  {
    typedef typename aa::advice_cast<ap::type2type<K>, map_aspect>::type holder_type;
    return static_cast<holder_type&>(*this).get_ref()[key];
  }
*/

  /*
  template<typename K>
  typename aa::advice_cast<ap::type2type<K>, map_aspect>::type::type::mapped_type& get(const K& key) const
  {
    typedef typename aa::advice_cast<ap::type2type<K>, map_aspect>::type holder_type;
    return static_cast<const holder_type&>(*this).get_ref()[key];
  }*/

  /*
  typename aa::advice_cast<ap::type2type<std::string>, map_aspect>::type::type::mapped_type& 
  operator[] (const char* key)
  {
    typedef typename aa::advice_cast<ap::type2type<std::string>, map_aspect>::type holder_type;
    return static_cast<holder_type&>(*this).get_ref()[key];
  }*/

  template<typename K>
  bool has_map() const
  {
    typedef typename aa::advice_cast<ap::type2type<K>, map_aspect>::type holder_type;
    return static_cast<const holder_type&>(*this).get_ptr()!=0;
  }

  template<typename K>
  typename aa::advice_cast<ap::type2type<K>, map_aspect>::type::type& get_map()
  {
    typedef typename aa::advice_cast<ap::type2type<K>, map_aspect>::type holder_type;
    return *static_cast<holder_type&>(*this).get_ptr();
  }

  template<typename K>
  const typename aa::advice_cast<ap::type2type<K>, map_aspect>::type::type& get_map() const
  {
    typedef typename aa::advice_cast<ap::type2type<K>, map_aspect>::type holder_type;
    const typename aa::advice_cast<ap::type2type<K>, map_aspect>::type::type*
      ptr = static_cast<const holder_type&>(*this).get_ptr();
    return *ptr;
  }

  template<typename T>
  self& operator = (const T& v)
  {
    super_variant::operator =(v);
    return *this;
  }

  self& operator = (const char* v)
  {
    super_variant::operator =( std::string(v) );
    return *this;
  }

};

}}

#endif
