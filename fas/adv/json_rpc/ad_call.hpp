//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_ADV_JSON_RPC_AD_CALL_HPP
#define FAS_ADV_JSON_RPC_AD_CALL_HPP

#include <map>
#include <fas/aop/advice.hpp>
#include <fas/pattern/type_list.hpp>
#include <fas/misc/json.hpp>

namespace fas{ namespace adv{ namespace json_rpc{

namespace ap = ::fas::pattern;
namespace aa = ::fas::aop;
namespace amj = ::fas::misc::json;

// Удаленный вызов сервер->клиент
template<typename M, typename J, int S = 4096>
class ad_call : public M
{
public:
  enum {call_size = S};

  template<typename T>
  void result(T& t, const char* d, size_t s)
  {
    /*amj::json_object jo(d, d + s);
    amj::json_object::iterator itr = jo["id"];
    if (!itr)
      t.get_aspect().template get<_invalid_json_>()(t, d, s);*/
    const char* s_id = _search_id(d, s);
    /*amj::json_object jo(d, d + s);
    amj::json_object::iterator itr = jo["id"];
    if (!itr)
      return;*/
    if (s_id==0) return;

    int rid = std::atoi(s_id);// itr->value<amj::json_value>();

    const char *result =  _search_result(d, d + s);
    /*itr = jo["result"];*/
    if (/*!itr*/result==0)
      t.get_aspect().template get<_invalid_json_>()(t, d, s);

    // amj::json_x jx = itr->value<amj::json_x>();
    M::result( t,  rid, /*J(jx.buffer_begin(), jx.buffer_end())*/result, d + s);
  }

  // Отложенный ответ (вызывается только через stub)
  template<typename T>
  void call(T& t, const char* name, int rid)
  {
    t.get_aspect().template get<_outgoing_object_>().initialize(S, name);
    amj::json_pack& pack = t.get_aspect().template get<_outgoing_object_>().get_outgoing();
    M::call(t, pack );
    t.get_aspect().template get<_outgoing_object_>().finalize(rid);
    // _request_map.remove(rid);
    t.get_aspect().template get<_output_>()(t, pack.begin(), pack.size());
  }

  M& get_method() { return static_cast<M&>(*this); }
  const M& get_method() const { return static_cast<const M&>(*this); }

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
/*
class ad_response
{
public:
  template<typename T, typename RTag>
  void operator()(T& t, int rid, RTag)
  {
    t.get_aspect().template get<_outgoing_>().initialize( R::response_size );

    amj::json_pack& pack = t.get_aspect().template get<_outgoing_>().get_outgoing();
    request[rid].response( pack );
    t.get_aspect().template get<_outgoing_>().finalize(rid);
  }

  
};
*/
}}}

#endif
