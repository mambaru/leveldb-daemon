#ifndef FAS_MISC_PHPSER_PHPSER_FIELD_HPP
#define FAS_MISC_PHPSER_PHPSER_FIELD_HPP

#include <fas/misc/meta.hpp>
#include "phpser_parser.hpp"

namespace fas{ namespace phpser{

namespace ap = ::fas::pattern;

template<typename V, /*template<typename> class S*/ typename A>
class deserializer_t< meta::metatype::key, V, A >
{
  enum { limit = V::limit};

public:
  deserializer_t():_result(false), _count(limit) {}

  // Проверка имени  
  template<typename P>
  P operator() ( P beg, P end )
  {
     return unserialize(beg, end, V() );
    /*
    if ( !parser::is_string(beg, end) )
      return beg;

    P result = beg;
    ptrdiff_t size = 0;
    beg = parser::parse_typename<'s'>(beg, end, &size);
    if ( *(beg++)!='"')
      throw "espect_of"; // TODO:

    if ( !check_name(beg, end, typename V::type()() ) )
      return result;
    std::advance(beg, size);
    ++beg;
    return beg;
    */
  }

  template<typename P, typename N >
  P unserialize( P beg, P end, meta::key<N>  )
  {
    
    _result=false;
    if ( !parser::is_string(beg, end) )
      return beg;

    P result = beg;
    ptrdiff_t size = 0;
    beg = parser::parse_typename<'s'>(beg, end, &size);
    if ( *(beg++)!='"')
      throw std::runtime_error("espect_of"); // TODO:

    if ( !check_name(beg, end, typename V::type()() ) )
      return result;
    std::advance(beg, size);
    ++beg;
    beg = parser::parse_space(beg, end);
    if ( beg!=end && *beg==';') ++beg;
    _result=true;
    return beg;
  }

  template<typename P, typename N >
  P unserialize( P beg, P end, N  )
  {
    
    _result = true;
    return beg;
  }


  void clear() { _count = limit; _result = false; }
  operator bool () const { return _result; };


private:
  bool _result;
  int _count;
  template<typename P, typename R>
  bool check_name( P beg, P end, R* name )
  {
    for ( ;beg!=end && *name!=R() && *beg!='"' && *beg==*name; ++beg, ++name);
    return *name==R() && beg!=end && *beg=='"';
  }
};


template<typename V/*, template<typename> class S*/, typename A>
class deserializer_t< meta::metatype::attribute, V, A >
{
public:
  typedef typename V::target target; // Тип исходного класса
  typedef typename V::nametype nametype;
  typedef typename V::valuetype valuetype;
  typedef typename V::membertype membertype;

  template<typename P>
  P operator() ( target& t, P beg, P end )
  {
    _result = false;
//    P result = beg;


    beg = parser::parse_space(beg, end);

    

    // Здесь может быть как имя, индекс, или пустое имя (для unnamed)
    beg = _name_serializer(beg, end);

    
    // Имя не подошло
    if ( !_name_serializer /*result == beg*/)
      return beg;

     

    beg = parser::parse_space(beg, end);

    
    if (beg==end)
      throw std::runtime_error("beg==end");;
    /*if (*beg!=';')
      throw std::runtime_error(std::string(beg, end));*/
    
    /*if (++beg==end)
      hrow std::runtime_error("beg==end");;*/
   
    beg = parser::parse_space(beg, end);
//    bool is_object = parser::is_object(beg, end);
    
    beg = _value_serializer( membertype()(t), beg, end );
    
    beg = parser::parse_space(beg, end);
    _result = true;
    /*if (beg==end) 
      throw std::runtime_error("beg==end");;
    if ( !is_object && *(beg++)!=';') 
      throw std::runtime_error("beg==end");;
     */
    return beg;
  }

  void clear() { _result = false; _name_serializer.clear(); /*_value_serializer.clear();*/ }
  operator bool () const { return _result; };

  

private:
/*
  deserializer<nametype> _name_serializer;
  deserializer<valuetype> _value_serializer;
*/

  typedef typename A::template deserializer<nametype, A>::type name_deserializer;
  typedef typename A::template deserializer<valuetype, A>::type value_deserializer;
  name_deserializer _name_serializer;
  value_deserializer _value_serializer;

  bool _result;
};

template<typename T, typename L/*, template<typename> class S*/, typename A>
class serializerTMember
  : serializerTMember<T, typename L::right_type, A>
{
protected:
  typedef serializerTMember<T, typename L::right_type, A> super;
  typedef typename L::left_type field_type;
  typedef  T target;

  template<typename P>
  P operator() ( target& t, P beg, P end )
  {
    // std::cout << "-1->" << std::string(beg, end) << std::endl;
    // здесь подумать как вести ограниечение не явно (универсально)
    if ( beg==end || *beg=='}' ) return beg;
    //beg = _serializer(t, beg, end);
    // std::cout << "-2->" << std::string(beg, end) << std::endl;
    return super::operator()(t, beg, end);
  }

  void clear() { /*_serializer.clear();*/ super::clear(); }
  operator bool () const { return true; };

  // S<field_type> _serializer;

};

template<typename T/*, template<typename> class S*/, typename A>
class serializerTMember<T, ap::empty_type, A>
{
protected:
  typedef  T target;
  template<typename P>
  P operator() ( target& , P beg, P ) { return beg; }
  void clear() { }
  operator bool () const { return true; };
};

/// это должен быть универсальный ыериализатор 
template<typename V/*, template<typename> class S*/, typename A>
class deserializer_t< meta::metatype::memberlist, V, A >
  : serializerTMember< typename V::target, typename V::memberlist, A>
{
  typedef serializerTMember< typename V::target, typename V::memberlist, A> super;
  typedef typename V::target target; // Тип исходного класса
  typedef typename V::memberlist memberlist;

public:

  template<typename P>
  P operator() ( target& t, P beg, P end )
  {
    // Один проход по списку
    // но может отработать несколько классов
    return super::operator()(t, beg, end);

    /*
    for ( ;beg!=end && *beg!='}'; )
    {
      P res = super::operator()(t, beg, end);
      if (res==beg) 
      {
        beg = parser::parse_member(beg, end);
        if (res==beg) 
          break;
        continue;
      }
      beg = res;
    }
    return beg;
    */
  }

  void clear() { super::clear(); }
  operator bool () const { return true; };

};

template<typename V/*, template<typename> class S*/, typename A>
class deserializer_t< meta::metatype::/*object*/boolean, V, A >
{
public:
  typedef typename V::target target; // Тип исходного класса
  typedef typename V::memberlist memberlist;

  // Десериализация в два прохода
  // 1. Пытаемся в обычные атрибуты (именованные или индексирование) memberlist1
  // 2. в противном случае прогоняем по второму списку (используется для map vector и т.д.)

  // Извлекаем аттрибуты, которые пеместим во второй список
  typedef typename meta::select_attrs_by_limit< -1, memberlist>::type memberlist2;
  typedef typename ap::erase_from_list<memberlist, memberlist2, 0 >::type memberlist1;

  template<typename P>
  P unserialize( target& t, P beg, P end )
  {
    /*
    for ( ;beg!=end && *beg!='}'; )
    {
      P res = _serializer1(t, beg, end);
      if (res==beg) 
      {
        res = _serializer2(t, beg, end);
        if (res==beg)
        {
          beg = parser::parse_member(beg, end);
          if (res==beg) break;
          continue; 
        }
      }
      beg = res;
    }
    */
    return beg;
  }


  template<typename P>
  P operator() ( target& t, P beg, P end )
  {
     
    beg = parser::parse_space(beg, end);
    if ( !parser::is_object(beg, end) )
      return beg;
    // size_t size = 0;
    beg = parser::parse_typename<'a'>(beg, end/*, size*/);
    beg = parser::parse_space(beg, end);
    if (beg!=end && *beg=='{') ++beg;
    beg = parser::parse_space(beg, end);
    beg = unserialize(t, beg, end);
    // beg = _serializer(t, beg, end);
    if (beg!=end && *beg=='}') ++beg;
    beg = parser::parse_space(beg, end);
    return beg;
  }

  void clear() { /*_serializer1.clear(); _serializer2.clear();**/}
  operator bool () const { return true; };

private:
/*  S< meta::fieldlist<target, memberlist1> > _serializer1;
  S< meta::fieldlist<target, memberlist2> > _serializer2;*/
};

/** Используется вне объекта*/
/** За одну итерацию одно поле... подумать*/

template<typename V/*, template<typename> class S*/, typename A>
class deserializer_t< meta::metatype::hash, V, A >
{
public:
  typedef typename V::target target;
  typedef typename V::key_value_type key_value;
  typedef typename V::mapped_value_type mapped_value;
  // typedef typename target::value_type value_type;

  template<typename P>
  P operator() ( target& t, P beg, P end )
  {
    beg = parser::parse_space(beg, end);
    std::pair<typename key_value::target, typename mapped_value::target>  vt;
    beg = _key_serialize(vt.first, beg, end);
    beg = parser::parse_space(beg, end);
    if (beg!=end && *beg==';') ++beg;
    beg = parser::parse_space(beg, end);
    beg = _mapped_serialize(vt.second, beg, end);
    beg = parser::parse_space(beg, end);
    if (beg!=end && *beg==';') ++beg;
    t.insert(vt);
    
    //beg = parser::parse_value(beg, end);
    return beg;
  }

  void clear() { _key_serialize.clear(); _mapped_serialize.clear(); }
  operator bool () const { return true; };

private:
  typedef typename A::template deserializer<key_value, A>::type key_deserializer;
  typedef typename A::template deserializer<mapped_value, A>::type mapped_deserializer;
  key_deserializer _key_serialize;
  mapped_deserializer _mapped_serialize;
  /*S<key_value> _key_serialize; 
  S<mapped_value> _mapped_serialize;*/
};

/*
template<typename G, typename S>
class serializerT< meta::value<std::string, G, S> >
*/
/*
template<typename V>
class serializerT< meta::metatype::string, V >
{
public:
  typedef typename V::target target;
  typedef typename V::getter getter;
  typedef typename V::setter setter;

  template<typename P>
  P operator()( const target& v, P end)
  {
    throw;
  }

  template<typename P>
  P operator() ( target& v, P beg, P end )
  {
    ptrdiff_t size;
    beg = parser::parse_typename<'s'>(beg, end, size);
    if ( ++beg == end )
      throw json::unexpected_end_fragment();
    P send = beg;
    std::advance(send, size);
    v.assign(beg, send);
    beg = send;
    ++beg;
    v = setter()(v);
    return beg;
  }
};
*/

}}


#endif

