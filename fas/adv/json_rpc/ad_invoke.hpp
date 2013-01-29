//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_ADV_JSON_RPC_INVOKE_HPP
#define FAS_ADV_JSON_RPC_INVOKE_HPP

#include <fas/pattern/type_list.hpp>
#include <fas/misc/json.hpp>

namespace fas{ namespace adv{ namespace json_rpc{

namespace ap = ::fas::pattern;
namespace amj = ::fas::json;

class ad_invoke
{
  static bool _raw_equal( const char* name_nz, const char* raw_name)
  {
    for ( ; *name_nz!='\0' ; ++name_nz, ++raw_name )
      if (*name_nz!=*raw_name)
        return false;
    return true;
  }

public:

  template<typename T>
  void operator()(T& t, const char* d, typename T::size_type s)
  {


    const char *method =  _search_method(d, d + s);
    /*amj::json_object jo = amj::json_object(d, d + s);
    amj::json_object::iterator itr = jo["method"];*/
    if (method == 0)
    {
      const char *result =  _search_result(d, d + s);
      // itr = jo["result"];
      if (result==0)
        t.get_aspect().template get<_invalid_json_>()(t, d, s);
      else
        t.get_aspect().template get<_result_>()(t, d, s);
    }
    else
    {
      typedef typename T::aspect::advice_list advice_list;
      typedef typename ap::select<_ginvoke_, advice_list>::type invoke_list;
      const char* name = /*itr->value<amj::json_value>().raw_begin()*/ method + 1;
      _select(t, invoke_list(), name , d, s );
    }
  }

private:
  template<typename L, typename T>
  void _select(T& t, L, const char* name, const char* d, typename T::size_type s)
  {
    typedef typename L::left_type current_type;
    current_type& current = t.get_aspect().template get<typename L::left_type>();
    if ( _raw_equal( current.name(), name ) )
    {
      current(t, d, s);
    }
    else
      _select(t, typename L::right_type(), name, d, s);
  }

  template< typename T>
  void _select(T& t, ap::empty_type, const char* name, const char* d, typename T::size_type s)
  {
    t.get_aspect().template get<_unknown_method_>()(t, d, s);
  }

  const char* _search_method( const char *beg, const char *end)
  {
    if ( std::distance(beg, end) < 10)
      return 0;
    for (;beg!=end-10; ++beg)
    {
      if ( 0==strncmp(beg, "\"method\"", 8) && ( beg[8]<=32 || beg[8]==':') )
      {
        beg+=8;
        for (;beg!=end && *beg<=32; ++beg);
        if ( beg==end ) return 0;
        if (*beg==':') return beg+1;
        return 0;
      }
    }
    return 0;
  }

  const char* _search_result( const char *beg, const char *end)
  {
    if ( std::distance(beg, end) < 10)
      return 0;
    for (;beg!=end-10; ++beg)
    {
      if ( 0==strncmp(beg, "\"result\"", 8) && ( beg[8]<=32 || beg[8]==':') )
      {
        beg+=8;
        for (;beg!=end && *beg<=32; ++beg);
        if ( beg==end ) return 0;
        if (*beg==':') return beg+1;
        return 0;
      }
    }
    return 0;
  }

};


}}}

#endif
