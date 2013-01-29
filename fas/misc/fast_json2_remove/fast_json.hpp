#ifndef FAS_MISC_FAST_JSON2_HPP
#define FAS_MISC_FAST_JSON2_HPP

#include <stdexcept>
#include <vector>
#include <set>
#include <map>

#include <fas/pattern/type_list.hpp>


namespace fas{ namespace misc{ namespace fast_json2{

namespace ap = ::fas::pattern;

class json_error: public std::runtime_error
{
  std::ptrdiff_t _tail_of;
public:
  json_error(const std::string& msg, size_t tail_of = 0 ): std::runtime_error(msg), _tail_of(tail_of) {}
  size_t tail_of() const { return _tail_of; }

  template<typename P>
  std::string message( P beg, P end ) const
  {
    if (std::distance(beg, end) < _tail_of )
      return this->what();
    std::string msg; 
    msg = this->what();
    msg += std::string(": ") + std::string(beg, end - _tail_of ) + ">>>" + std::string(end - _tail_of, end);
    return msg;
  };
};

class serialize_error: public json_error
{ 
public:
  serialize_error(size_t tail_of = 0)
    : json_error("serialize error", tail_of) {}
};

class deserialize_error: public json_error
{ 
public:
  deserialize_error(size_t tail_of = 0)
    : json_error("deserialize error", tail_of)  {}
};

class not_supported: public json_error
{ 
public:
  not_supported(size_t tail_of = 0)
    : json_error("not supported", tail_of)  {}
};

class invalid_json: public json_error
{ 
public:
  invalid_json(size_t tail_of = 0)
    : json_error("invalid json", tail_of)  {}
};

class invalid_json_number: public json_error
{ 
public:
  invalid_json_number(size_t tail_of = 0)
    : json_error("invalid json number", tail_of)  {}
};

class invalid_json_bool: public json_error
{ 
public:
  invalid_json_bool(size_t tail_of = 0)
    : json_error("invalid json bool", tail_of)  {}
};

class invalid_json_string: public json_error
{ 
public:
  invalid_json_string(size_t tail_of = 0)
    : json_error("invalid json string", tail_of)  {}
};

class invalid_json_member: public json_error
{ 
public:
  invalid_json_member(size_t tail_of = 0)
    : json_error("invalid json member", tail_of)  {}
};

class invalid_conversion
  : public json_error
{
public:
  invalid_conversion(size_t tail_of = 0)
    : json_error( "invalid conversion", tail_of) {}
  invalid_conversion( const std::string& from, const std::string& to, size_t tail_of = 0 )
    : json_error( std::string("invalid conversion from '") + from + std::string("' to '") + to, tail_of ) {}
};

class unexpected_end_fragment
  : public json_error
{
public:
  unexpected_end_fragment(size_t tail_of = 0)
    : json_error( "unexpected end of ragment", tail_of) {}
  unexpected_end_fragment(const std::string& str, size_t tail_of = 0)
   : json_error( std::string("unexpected end of ragment: ") + str, tail_of ) {}
};

class expected_of
  : public json_error
{
public:
  expected_of(const std::string& str, size_t tail_of = 0)
    : json_error( std::string("expected of '") + str + std::string("'"), tail_of) {}
};

/// /////////////////////////////////////////////////////

template<typename T>
struct value;

template<typename T = std::string>
struct raw_value;

/*
template<typename L, typename R>
struct if_def;
*/

template<typename N, typename T, T v>
struct enum_value;

template< typename T, typename L>
struct enumerator;

template<typename T, typename L>
struct object;

template<typename C>
struct array;

template<typename N,
         typename T,
         typename M,
         M T::* m,
         typename W = value<M> >
struct member;

template<typename N,
         typename T,
         typename M,
         typename G, // getter
         typename W = value<M>
        >
struct member_p;

/** Ахтунг! замороченые правила:
  * L и M типа member с одинаковыми N (именами)
  * сериализация:
  *   если значение мембера L не равно значению по умолчанию, то сериализуется L, в противном случае R
  * десериализация:
  *   всегда десереализуется в R, а L игнорируется
  * Используется для чего-чего?:
  *   L мембер типа raw_value (т.е. строка голого json как есть) и если он не пустой, то просто копируем в поток эту строку,
  *   если L пустой, то применяем сериализацию R. Соответственно если попытатся десериализовать, то сразу десериализуется по правилам в R.
  *   но если RU задать как false, то копируем в raw_value значение мембера
  */
template<typename L, typename R, bool RU = true >
struct member_if;

template<typename J>
class serializerT;

/// //////////////////////////////////////////////////////// 

/// /////////////////////////////////////////////////////

template<>
struct value<char>
{
  typedef char target;
  typedef serializerT< value<char> > serializer;
};

template<>
struct value<unsigned char>
{
  typedef unsigned char target;
  typedef serializerT< value<unsigned char> > serializer;
};

template<>
struct value<short>
{
  typedef short target;
  typedef serializerT< value<short> > serializer;
};

template<>
struct value<unsigned short>
{
  typedef unsigned short target;
  typedef serializerT< value<unsigned short> > serializer;
};


template<>
struct value<int>
{
  typedef int target;
  typedef serializerT< value<int> > serializer;
};

template<>
struct value<unsigned int>
{
  typedef unsigned int target;
  typedef serializerT< value<unsigned int> > serializer;
};

template<>
struct value<long>
{
  typedef long target;
  typedef serializerT< value<long> > serializer;
};

template<>
struct value<unsigned long>
{
  typedef unsigned long target;
  typedef serializerT< value<unsigned long> > serializer;
};

template<>
struct value<long long>
{
  typedef long long target;
  typedef serializerT< value<long long> > serializer;
};

template<>
struct value<unsigned long long>
{
  typedef unsigned long long target;
  typedef serializerT< value<unsigned long long> > serializer;
};


template<>
struct value<bool>
{
  typedef bool target;
  typedef serializerT< value<bool> > serializer;
};


template<>
struct value<std::string>
{
  typedef std::string target;
  typedef serializerT< value<std::string> > serializer;
};

template<int N>
struct value< char[N] >
{
  typedef char target[N];
  typedef serializerT< value<char[N]>  > serializer;
};


template<>
struct raw_value<std::string>
{
  typedef std::string target;
  typedef serializerT< raw_value<std::string> > serializer;
};

/*
template<typename T>
struct if_def
{
  typedef typename T::target target;
  typedef serializerT< if_def<T> > serializer;
};*/



/// //////////////////////////////////////////////////////////////////////////////

template<typename N, typename T, T v>
struct enum_value: N
{
  static const T value = v;
  const char* operator()() const { return N::operator()(); }
};

template< typename T, typename L>
struct enumerator
{
  typedef T target;
  typedef typename ap::type_list_traits<L>::type enum_list;
  typedef serializerT< enumerator<T, enum_list> > serializer;
};

/// //////////////////////////////////////////////////////////////////////////////

template<typename T, typename L>
struct object
{
  typedef T target;
  typedef serializerT< object<T, L> > serializer;
  typedef typename ap::type_list_traits<L>::type member_list;
};

/// //////////////////////////////////////////////////////////////////////////////

template<typename N, typename T, typename M, M T::* m, typename W >
struct member: N
{
  typedef T target;
  typedef M type;
  typedef W wrapper;
  typedef typename W::serializer serializer;
  typedef typename W::target wrapper_target;
  const char* operator()() const { return N::operator()(); }

  wrapper_target* ptr(T* t) { return static_cast<wrapper_target*>(t->*m);};
  const wrapper_target* ptr(const T* t) const { return static_cast< const wrapper_target*>(t->*m);};
  wrapper_target& ref(T& t) { return static_cast<wrapper_target&>(t.*m); };
  const wrapper_target& ref(const T& t) const { return static_cast< const wrapper_target&>(t.*m);};
};


template<typename T, typename M, M T::* m>
struct property
{
  void operator()(T& t, const M& value ) const
  {
    t.*m = value;
  }

  const M& operator()(const T& t) const
  {
    return t.*m;
  }
};

template<typename N,
         typename T,
         typename M,
         typename G, // getter
         typename W
        >
struct member_p: N
{
private:
  G _g;
public:
  typedef M type;
  typedef typename W::serializer serializer;
  typedef typename W::target wrapper_target;
  const char* operator()() const { return N::operator()(); }
  void set(T& t, const wrapper_target& v) const { _g(t, v); }
  wrapper_target get(const T& t) const { return _g(t); }
};


template<typename L, typename R, bool RU >
struct member_if
{
};


/// //////////////////////////////////////////////////////////////////////////////


/// //////////////////////////////////////////////////////////////////////////////

template<typename C>
struct array_base;

template<typename J>
struct array_base< std::vector<J> >
{
  typedef std::vector<J> json_container;
  typedef J json_value;
  typedef typename json_value::target target;
  typedef std::vector<target> target_container;

  typedef std::back_insert_iterator<target_container> inserter_iterator;
  static inserter_iterator inserter(target_container& t) { return std::back_inserter(t); }
};

/*
template<typename J, int N>
struct array_base< J[N] >
{
};*/


template<typename J>
struct array_base< std::set<J> >
{
  typedef std::set<J> json_container;
  typedef J json_value;
  typedef typename json_value::target target;
  typedef std::set<target> target_container;

  typedef std::insert_iterator<target_container> inserter_iterator;
  static inserter_iterator inserter(target_container& t) { return std::inserter(t, t.begin()); }
};

template<typename J>
struct array_base< std::multiset<J> >
{
  typedef std::multiset<J> json_container;
  typedef J json_value;
  typedef typename json_value::target target;
  typedef std::multiset<target> target_container;

  typedef std::insert_iterator<target_container> inserter_iterator;
  static inserter_iterator inserter(target_container& t) { return std::inserter(t, t.begin()); }
};

struct n_key { const char* operator()() const{ return "key";} };
struct n_value { const char* operator()() const{ return "value";} };

template<typename JK, typename JV>
struct array_base< std::map<JK, JV> >
{
  typedef std::map<JK, JV> json_container;
  typedef typename JK::target key;
  typedef typename JV::target value;
  typedef std::pair<key, value> pair_type;
  typedef object<
            pair_type,
            typename ap::type_list_n<
              member< n_key, pair_type, key, &pair_type::first, JK >,
              member< n_value, pair_type, value, &pair_type::second, JV >
            >::type
          > json_value;
  typedef typename json_value::target target;
  typedef std::map< key, value > target_container;

  typedef std::insert_iterator<target_container> inserter_iterator;
  static inserter_iterator inserter(target_container& t) { return std::inserter(t, t.begin()); }
};


template<typename C>
struct array: public array_base<C>
{
  typedef array_base<C> base;
  typedef typename base::target_container target;
  typedef serializerT< array<C> > serializer;
};

template< typename T, typename L>
struct array< object<T, L> >
  : array< std::vector< object<T, L> > >
{
};

/// //////////////////////////////////////////////////////////////////////////////

}}}

#include "specialization/json_parser.hpp"
#include "specialization/json_number.hpp"
#include "specialization/json_string.hpp"
#include "specialization/json_object.hpp"
#include "specialization/json_array.hpp"
#include "specialization/json_enum.hpp"

#endif
