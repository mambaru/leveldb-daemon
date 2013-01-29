#ifndef FAS_MISC_PHPSER_PHPSER_OBJECT_HPP
#define FAS_MISC_PHPSER_PHPSER_OBJECT_HPP

#include <fas/misc/meta.hpp>
#include "phpser_parser.hpp"

namespace fas{ namespace phpser{

namespace ap = ::fas::pattern;

namespace detail
{

  template<typename L,/*, template<typename> class S*/ typename A, bool B>
  class deserializer_list;

  /** B продолжать процесс в случае успешной десериализации поля  
      B = true - десер. один раз, передача на верх в люблм случае
      B = true - многократная, передача на верх в случае неуспеха
    */

/// /////////////////////////////////////////

  template<typename L /*, template<typename> class S*/, typename A>
  class deserializer_list<L, A, true>
    : public deserializer_list<typename L::right_type, A, true>
  {
    typedef deserializer_list<typename L::right_type, A, true> super;
    typedef typename L::left_type item_type;
    // typedef S<item_type> deserializer;
    typedef typename A::template deserializer<item_type, A>::type deserializer;

    public:
      deserializer_list(): _flag(false) {}

      template<typename T, typename P>
      P operator() ( T& t, P beg, P end );

    private:
      bool _flag;
  }; 

  template<typename L/*, template<typename> class S*//*, template<typename> class S*/, typename A>
  class deserializer_list<L, A, false>
    : public deserializer_list<typename L::right_type, A, false>
  {
    typedef deserializer_list<typename L::right_type, A, false> super;
    typedef typename L::left_type item_type;
    // typedef S<item_type> deserializer;
    typedef typename A::template deserializer<item_type, A>::type deserializer;

    public:
      template<typename T, typename P>
      P operator() ( T& t, P beg, P end );
  };

/// /////////////////////////////////////////

  template</*, template<typename> class S*/ typename A>
  class deserializer_list<ap::empty_type, A, true>
  {
  public:
    template<typename T, typename P>
    P operator() ( T& t, P beg, P end ) { return beg; };
  };

  template</*template<typename> class S*//*, template<typename> class S*/ typename A>
  class deserializer_list<ap::empty_type, A, false>
  {
  public:
    template<typename T, typename P>
    P operator() ( T& t, P beg, P end ) { return beg; };
  };

/// /////////////////////////////////////////

  template<typename L/*, template<typename> class S*/, typename A>
  template<typename T, typename P>
  P deserializer_list<L, A, true>::operator()(T& t, P beg, P end)
  {
    if (!_flag)
    {
      P itr = deserializer()(t, beg, end);
      _flag = itr!=beg;
      beg = itr;
    }
    return super::operator()(t, beg, end);
  }

  template<typename L /*, template<typename> class S*/, typename A>
  template<typename T, typename P>
  P deserializer_list<L, A, false>::operator()(T& t, P beg, P end)
  {
    P itr = deserializer()(t, beg, end);
    return itr != beg ? itr : super::operator()(t, beg, end);
  }
}

template<typename V/*, template<typename> class S*/, typename A>
class deserializer_t< meta::metatype::object, V, A >
{
  typedef ap::type_list_n< meta::metatype::attribute, meta::metatype::property >::type metalist;
  typedef typename V::memberlist memberlist;
  typedef typename V::target target; // Тип исходного класса

  // property и аттрибуты
  typedef typename meta::select_meta<memberlist, metalist>::type first_list;
  // все остальное
  typedef typename meta::select_meta<memberlist, metalist>::margin second_list;

  typedef detail::deserializer_list<first_list, A, true> first_deserializer;
  typedef detail::deserializer_list<second_list, A, false> second_deserializer;


public:
  deserializer_t()/*: _result(false)*/ {};

  template<typename P>
  P operator() ( target& t, P beg, P end )
  {
    

    P start = beg;
    /*_result = false;
    _deserializer1.clear();
    _deserializer2.clear();
    */

     beg = parser::parse_space(beg, end);
     if ( !parser::is_object(beg, end) )
       return start;

    beg = parser::parse_typename<'a'>(beg, end);
    beg = parser::parse_space(beg, end);
    if ( beg==end || *beg!='{')
      return start;
    ++beg;
    beg = parser::parse_space(beg, end);
    beg = unserialize(t, beg, end);
    beg = parser::parse_space(beg, end);
    if ( beg==end || *beg!='}')
      return start;
    ++beg;
    // _result = true;
    return beg;
  }

  /*
  operator bool () const { return _result; };
  void clear() { _result = false;};
  */

private:

  template<typename P>
  P unserialize( target& t, P beg, P end )
  {
    first_deserializer deserializer1;
    second_deserializer deserializer2;
    for ( ;beg!=end && *beg!='}'; )
    {
      //std::cout << "deserializer_object:" << std::string(beg, end) << std::endl;
      beg = parser::parse_space(beg, end);
      P itr = deserializer1(t, beg, end);
      if ( itr == beg )
      {
        itr = deserializer2(t, beg, end);
        if ( itr == beg )
        {
          itr = parser::parse_member(beg, end);
          if (itr==beg) return beg;
        }
      }
      beg = itr;
      /*
      _deserializer1.clear();
      _deserializer2.clear();

      beg = _deserializer1(t, beg, end);
      if ( !_deserializer1 )
      {
        beg = _deserializer2(t, beg, end);
        if ( !_deserializer2 )
        {
          P res = parser::parse_member(beg, end);
          if (res==beg) return beg;
          beg = res;
        }
      }
      */
    }
    return beg;
  }

  /*
  bool _result;
  detail::deserializer_list<first_list, S, true> _deserializer1;
  detail::deserializer_list<second_list, S, false> _deserializer2;
  */
};


/*
template<typename V, template<typename> class S>
class deserializer_t< meta::metatype::object, V, S >
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
    for ( ;beg!=end && *beg!='}'; )
    {
      std::cout<<"---------------------- serializer1 ---------------------- " << ap::length<memberlist1>::result << ", "<< ap::length<memberlist2>::result << std::endl;
      P res = _serializer1(t, beg, end);
      if (res==beg) 
      {
        std::cout<<"---------------------- serializer2 ---------------------- "<< std::endl;
        res = _serializer2(t, beg, end);
        if (res==beg)
        {
          std::cout<<"---------------------- parse_member ---------------------- "<< std::endl;
          beg = parser::parse_member(beg, end);
          if (res==beg) break;
          continue; 
        }
      }
      beg = res;
    }
    return beg;
  }


  template<typename P>
  P operator() ( target& t, P beg, P end )
  {
    std::cout << "object 1" << std::endl;
    beg = parser::parse_space(beg, end);
    if ( !parser::is_object(beg, end) )
      return beg;
    std::cout << "object 2" << std::endl;
    // size_t size = 0;
    beg = parser::parse_typename<'a'>(beg, end);
    beg = parser::parse_space(beg, end);
    if (beg!=end && *beg=='{') ++beg;
    beg = parser::parse_space(beg, end);
    beg = unserialize(t, beg, end);
    // beg = _serializer(t, beg, end);
    if (beg!=end && *beg=='}') ++beg;
    beg = parser::parse_space(beg, end);
    std::cout << "object 3" << std::endl;
    return beg;
  }

  void clear() { _serializer1.clear(); _serializer2.clear();}
  operator bool () const { return true; };

private:
  S< meta::fieldlist<target, memberlist1> > _serializer1;
  S< meta::fieldlist<target, memberlist2> > _serializer2;
};

*/
}}


#endif

