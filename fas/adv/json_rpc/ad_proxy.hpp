//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_ADV_JSON_RPC_AD_PROXY_HPP
#define FAS_ADV_JSON_RPC_AD_PROXY_HPP

  #include <iostream>
#include <map>
#include <fas/aop/advice.hpp>
#include <fas/pattern/type_list.hpp>
#include <fas/misc/json.hpp>

namespace fas{ namespace adv{ namespace json_rpc{

namespace ap = ::fas::pattern;
namespace aa = ::fas::aop;
namespace amj = ::fas::misc::json;

namespace detail
{
  template<typename L>
  struct result_helper
  {
    template<typename T>
    void operator()(T& t, int pos, const char* d, size_t s)
    {
      typedef typename L::left_type current_advice;
      if ( pos == 0 )
        static_cast<current_advice&>(t.get_aspect()).result(t, d, s);
      else
        result_helper<typename L::right_type>()(t, pos - 1, d, s);
    }
  };

  template<>
  struct result_helper<ap::empty_type>
  {
    template<typename T>
    void operator()(T& t, int pos, const char* d, size_t s) 
    {
      t.get_aspect().template get<_lost_call_>()(t, d, s);
    }
  };
  /*template<typename L, typename R>
  struct result_helper
    : result_helper<L, typename R::right_type>
  {
    template<typename T>
    void operator()(T& t, int pos, const char* d, size_t s)
    {
      typedef typename L::left_type current_advice;
      if ( ap::type_position<current_advice, L>::result == pos )
        static_cast<current_advice&>(t.get_aspect()).result(t, d, s);
      else
        result_helper<L, typename R::right_type>::operator ( )(t, pos, d, s);
    }
  };

  template<typename L>
  struct result_helper<L, ap::empty_type>
  {
    template<typename T>
    void operator()(T& t, int pos, const char* d, size_t s) 
    {
      t.get_aspect().template get<_lost_call_>()(t, d, s);
    }
  };*/
}

class ad_proxy
{
  int _id_count;
  typedef std::map<int, int> waiting_list;
  waiting_list _waiting_list;
public:
  ad_proxy(): _id_count(1000000) {}

  template<typename T>
  void clear(T&)
  {
    _id_count = 1000000;
    _waiting_list.clear();
  }

  template<typename CallTag, typename T>
  void call(T& t)
  {
    typedef typename T::aspect::advice_list advice_list;
    typedef typename ap::select<_gcall_, advice_list>::type call_list;

    const char* name = t.get_aspect().template get<CallTag>().name();
    t.get_aspect().template get<CallTag>().call(t, name, ++_id_count);
    _waiting_list[_id_count] = ap::type_position< CallTag, call_list>::result;
  }

  template<typename T>
  void operator()(T& t, const char *d, size_t s)
  {
    const char* s_id = _search_id(d, s);
    /*amj::json_object jo(d, d + s);
    amj::json_object::iterator itr = jo["id"];
    if (!itr)
      return;*/
    if (s_id==0) return;

    // int rid = itr->value<amj::json_value>();
    int rid = std::atoi(s_id);
    waiting_list::iterator witr = _waiting_list.find(rid);
    if (witr!=_waiting_list.end())
    {
      typedef typename T::aspect::advice_list advice_list;
      typedef typename ap::select<_gcall_, advice_list>::type call_list;

      int pos = witr->second;
      _waiting_list.erase(witr);
      // detail::result_helper< call_list, call_list >()(t, pos, d, s);
      detail::result_helper< call_list >()(t, pos, d, s);
    }
    else
      t.get_aspect().template get<_lost_call_>()(t, d, s);
  }

private:
  const char* _search_id( const char *d, size_t s )
  {
    const char *end = d + s;
    const char *rbeg = d + s - 2;
    const char *rend = d /* - 1 */;
    for ( ; rbeg!=rend; --rbeg)
      if ( rbeg[0] == 'i' && rbeg[1] == 'd')
        break;
    if ( rbeg == rend ) return 0;

    for ( ; rbeg != end && *rbeg!=':'; ++rbeg);
    if ( rbeg == rend ) return 0;
    ++rbeg;
    for ( ; rbeg != end && *rbeg<=' '; ++rbeg);
    if ( rbeg == rend ) return 0;
    return rbeg;

  };

};


}}}

#endif
