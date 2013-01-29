//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_ADV_JSON_RPC_AD_CALL_REQUEST_HPP
#define FAS_ADV_JSON_RPC_AD_CALL_REQUEST_HPP


#include <fas/adv/json_rpc/ad_call.hpp>
#include <fas/adv/json_rpc/ad_request.hpp>

namespace fas{ namespace adv{ namespace json_rpc{

template<typename M, typename J, int S = 256>
class ad_call_request :
  ad_request<M, J, S>,
  ad_call<M, J, S>
{
  typedef ad_request<M, J, S> request_type;
  typedef ad_call<M, J, S> call_type;

public:
  template <typename T>
  void clear(T& t)
  {
    request_type::clear(t);
    call_type::clear(t);
  }
  const char* name() const
  {
    return request_type::name();
  }

  template<typename T>
  void operator()(T& t, const char* d, size_t s)
  {
    request_type::operator ( )(t, d, s);
  }

  template<typename T>
  void response(T& t, int rid)
  {
    request_type::response(t, rid);
  }

  template<typename T>
  void result(T& t, const char* d, size_t s)
  {
     call_type::result(t, d, s);
  }

  template<typename T>
  void call(T& t, const char* name, int rid)
  {
    call_type::call(t, name, rid);
  }


};

}}}

#endif
