#ifndef FAS_MISC_PHPSER_PHPSER_VALUE_HPP
#define FAS_MISC_PHPSER_PHPSER_VALUE_HPP

#include <fas/misc/meta.hpp>
#include "phpser_parser.hpp"

namespace fas{ namespace phpser{

namespace detail
{
   // ff 255 3
   // ffff 65535 5
   // ffffffff 4294967295 10
   // ffffffffffffffff 18446744073709551615 20
   // 1 - 3  -> 1*2 + 1/2
   // 2 - 5  -> 2*2 + 2/1
   // 4 - 10 -> 4*2 + 4/2
   // 8 - 20 -> 8*2 + 8/2

  // S==true - signed, false - unsigned
  /*template< typename T, bool S >
  class serializer_integer
  {
  public:
  };*/
  template<typename T>
  struct is_signed_integer
  {
    enum { result = T(-1) < T(1) };

    static bool is_less_zero(T v)
    {
      return result && (v < 0);
    }
  };

  template<typename T>
  struct integer_buffer_size
  {
    enum { result = sizeof(T)*2 + sizeof(T)/2 + sizeof(T)%2 + is_signed_integer<T>::result };
  };

  template<typename T>
  class integer_serializer
  {
    char _buf[integer_buffer_size<T>::result];
  public:

    integer_serializer() {}

    template<typename P>
    P serialize(T v, P itr)
    {
      register char *beg = _buf;
      register char *end = _buf;
      if (v==0)
        *(end++) = '0';
      else
      {
        if ( is_signed_integer<T>::is_less_zero(v) ) 
        {
          *(end++)='-';
          ++beg;
          for( ; v!=0 ; ++end, v/=10) 
            *end = '0' - v%10;
        }
        else
          for( ; v!=0 ; ++end, v/=10) 
            *end = '0' + v%10;
      }
      for ( register char* cur = end ; cur-beg > 1;--cur, ++beg) 
      { *beg ^= *(cur-1); *(cur-1)^=*beg; *beg^=*(cur-1); }

      for (beg = _buf; beg!=end; ++beg)
        *(itr++)=*beg;

      return itr;
    }

    template<typename P>
    P unserialize ( T& v, P beg, P end )
    {
       return parser::unserialize_integer(v, beg, end);
    };
  };


  template<typename T, typename G, typename S>
  class deserializerN: integer_serializer<T>
  {
  public:

    template<typename P>
    P operator() ( T& v, P beg, P end )
    {
      P start = beg;
      beg = parser::parse_space(beg, end);
      if ( parser::is_null(beg, end) )
      {
        v = T();
        beg = parser::parse_null(beg, end);
      }
      else if ( parser::is_number(beg, end) )
      {
        beg = parser::parse_typename<'i'>(beg, end);
        beg =  unserialize(v, beg, end);
      }
      else
        return start;

      meta::apply_getset( v, S() );
      beg = parser::parse_space(beg, end);
      if (beg!=end && *beg==';') ++beg;
      return beg;
    };
  };
}

template<typename V, /*template<typename> class S*/ typename A>
class deserializer_t< meta::metatype::numeric, V, /*S*/ A >
  : public detail::deserializerN< typename V::target, typename V::getter, typename V::setter>
{
public:
};

template<typename V, /*template<typename> class S*/ typename A>
class deserializer_t< meta::metatype::number_string, V, /*S*/A >
{
  
public:

  template<typename T, typename P>
  P operator() ( T& v, P beg, P end )
  {
      P start = beg;
      beg = parser::parse_space(beg, end);
      if ( parser::is_null(beg, end) )
      {
        v = T();
        beg = parser::parse_null(beg, end);
      }
      else if ( parser::is_number(beg, end) )
      {
         typedef meta::value<T, typename V::getter, typename V::setter> newvalue;
         typedef typename A::template deserializer<newvalue, A>::type deserealizer;
         /*
         typedef typename A::template builder<>::type deserialize_builder;
         typedef typename deserialize_builder::template build< meta::value<T, typename V::getter, typename V::setter> >::type deserealizer;
         */
         deserealizer()(v, beg, end);
         // return S< meta::value<T, typename V::getter, typename V::setter> >()(v, beg, end);
      }
      else if ( parser::is_string(beg, end) )
      {
        ptrdiff_t size;
        beg = parser::parse_typename<'s'>(beg, end, &size);
        if ( beg == end || *(beg++) != '"' )
          return start;
        P sbeg = beg;
        for ( ; size!=0 && beg!=end; --size, ++beg);
        P send = beg;
        if ( beg == end || *beg != '"' )
          return start;
        ++beg;
        parser::unserialize_integer(v, sbeg, send);
      }
      else
        return start;

      beg = parser::parse_space(beg, end);
      if (beg!=end && *beg==';') ++beg;
      return beg;
  }
};


/*
template<typename G, typename S>
class serializerT< meta::value<char, G, S> >
  : public detail::serializerN<char, G, S>
{
};

template<typename G, typename S>
class serializerT< meta::value<unsigned char, G, S> >
  : public detail::serializerN<unsigned char, G, S>
{
};

template<typename G, typename S>
class serializerT< meta::value<short, G, S> >
  : public detail::serializerN<short, G, S>
{
};

template<typename G, typename S>
class serializerT< meta::value<unsigned short, G, S> >
  : public detail::serializerN<unsigned short, G, S>
{
};

template<typename G, typename S>
class serializerT< meta::value<int, G, S> >
  : public detail::serializerN<int, G, S>
{
};

template<typename G, typename S>
class serializerT< meta::value<unsigned int, G, S> >
  : public detail::serializerN<unsigned int, G, S>
{
};

template<typename G, typename S>
class serializerT< meta::value<long, G, S> >
  : public detail::serializerN<long, G, S>
{
};

template<typename G, typename S>
class serializerT< meta::value<unsigned long, G, S> >
  : public detail::serializerN<unsigned long, G, S>
{
};

*/


}}


#endif

