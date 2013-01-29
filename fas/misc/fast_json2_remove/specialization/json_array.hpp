#ifndef FAS_MISC_FAST_JSON2_SPECIALIZATION_JSON_ARRAY_HPP
#define FAS_MISC_FAST_JSON2_SPECIALIZATION_JSON_ARRAY__HPP

#include "json_parser.hpp"

// #define USIND_STDLIB

namespace fas{ namespace misc{ namespace fast_json2{


/// ////////////////////////////////////////////////////////////////

template< typename T >
class serializerT< array<T> >
{
  typedef array<T> array_type;
  typedef typename array_type::target_container target_container;
  typedef typename array_type::json_value json_value;
  typedef typename json_value::serializer serializer;
  typedef typename json_value::target target;


public:
  template<typename P>
  P operator()( target_container& t,  P beg, P end)
  {
    if ( parser::is_null(beg, end) )
    {
      t = target_container();
      return parser::parse_null(beg, end);
    }

    // std::back_insert_iterator<C> bitr =  std::back_inserter(t);
    // std::insert_iterator<C> bitr = std::inserter(t, t.end());
    typename array_type::inserter_iterator bitr = array_type::inserter(t);

    if (beg==end) throw;
    if (*beg!='[') throw;
    ++beg;
    for (;beg!=end;)
    {
      beg = parser::parse_space(beg, end);
      if (beg==end) throw;
      if (*beg==']') break;
      target tg;
      beg = serializer()( tg, beg, end);
      *(bitr++) = tg;
      beg = parser::parse_space(beg, end);
      if (beg==end) throw;
      if (*beg==']') break;
      if (*beg!=',') throw;
      ++beg;
    }
    if (beg==end) throw;
    if (*beg!=']') throw;
    ++beg;
    return beg;
  }


  template<typename P>
  P operator()( const target_container& t, P end)
  {
    *(end++)='[';
    typename target_container::const_iterator itr = t.begin();
    for (;itr!=t.end();)
    {
      end = serializer()(*itr, end);
      ++itr;
      if (itr!=t.end()) *(end++)=',';
    }
    *(end++)=']';
    return end;
  }
};

}}}

#endif
