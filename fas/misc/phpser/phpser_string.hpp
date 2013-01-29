#ifndef FAS_MISC_PHPSER_PHPSER_STRING_HPP
#define FAS_MISC_PHPSER_PHPSER_STRING_HPP

#include <fas/misc/meta.hpp>
#include "phpser_parser.hpp"

namespace fas{ namespace phpser{


/*
template<typename G, typename S>
class serializerT< meta::value<std::string, G, S> >
*/
template<typename V, /*template<typename> class S*/ typename A>
class deserializer_t< meta::metatype::string, V, A >
{
public:
  typedef typename A::template deserializer<V, A>::type deserializer;
  typedef typename V::target target;
  typedef typename V::getter getter;
  typedef typename V::setter setter;

  template<typename P>
  P operator() ( target& v, P beg, P end )
  {
    P start = beg;
    beg = parser::parse_space(beg, end);
    if ( beg == end )
      return start;

    if ( parser::is_null(beg, end) )
    {
      v = target();
      beg =  parser::parse_null(beg, end);
    }
    else if ( parser::is_string(beg,  end) )
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
      v.assign(sbeg, send);
    }
    else
      return start;
    meta::apply_getset( v, setter() );
    beg = parser::parse_space(beg, end);
    if (beg!=end && *beg==';') ++beg;
    return beg;
  }
};

}}


#endif

