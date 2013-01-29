//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_ADV_JSON_RPC_AD_REQUEST_HPP
#define FAS_ADV_JSON_RPC_AD_REQUEST_HPP

#include <map>
#include <fas/pattern/type_list.hpp>
#include <fas/misc/json.hpp>
#include <fas/adv/json_rpc/tags.hpp>

namespace fas{ namespace adv{ namespace json_rpc{

namespace ap = ::fas::pattern;
namespace amj = ::fas::misc::json;

template<typename M, typename J, int S = 4096>
class ad_request : public M
{
public:
  enum {response_size = S};

  template<typename T>
  void operator()(T& t, const char* d, size_t s)
  {
    amj::json_object jo(d, d + s);
    const char* s_rid = _search_id( d, s );
    int rid = 0;
    if (s_rid!=0)
      rid = std::atoi(s_rid);


    const char* param = _search_param(d, d+s);
    if (param == 0 )
      t.get_aspect().template get<_invalid_json_>()(t, d, s);
    else
    if ( M::request( t, rid, param, d + s) )
    {
      this->response(t, rid);
    }
  }

  // Отложенный ответ
  template<typename T>
  void response(T& t, int rid)
  {
    t.get_aspect().template get<_outgoing_object_>().initialize(S);
    amj::json_pack& pack = t.get_aspect().template get<_outgoing_object_>().get_outgoing();
    M::response( t, pack );
    t.get_aspect().template get<_outgoing_object_>().finalize(rid);
    t.get_aspect().template get<_output_>()(t, pack.begin(), pack.size());
  }
 
  const char* _search_param( const char *beg, const char *end)
  {
    if ( std::distance(beg, end) < 10)
      return 0;
    for (;beg!=end-10; ++beg)
    {
      if ( 0==strncmp(beg, "\"params\"", 8) && ( beg[8]<=32 || beg[8]==':') )
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
