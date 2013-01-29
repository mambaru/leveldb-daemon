//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_UTIL_MEMCACHED_STATE_READY_HPP
#define FAS_UTIL_MEMCACHED_STATE_READY_HPP

#include <fas/pattern/state.hpp>
#include <fas/util/memcached/states.hpp>
#include <fas/util/memcached/istate.hpp>
#include <fas/util/memcached/tags.hpp>
#include <vector>

#include <iostream>
namespace fas{ namespace util { namespace memcached{

namespace ap = ::fas::pattern;

template<state_mode s>
struct state_ready;

template<>
struct state_ready<mode_server>
 : ap::state<istate, MC_ready>
{
  typedef ap::state<istate, MC_ready> super;
  
  bool check(const char* , size_t ) { return false;}
  
  /** @return сколько байт отработанно */
  template<typename T>
  size_t operator()(T& t, const char* d, size_t s)
  {
    throw std::runtime_error("state error"); 
    return 0;
  }
};

template<>
struct state_ready<mode_client>
 : ap::state<istate, MC_ready>
{
  typedef ap::state<istate, MC_ready> super;
  
  bool check(const char* , size_t ) { return false;}
  
  /** @return сколько байт отработанно */
  template<typename T>
  size_t operator()(T& t, const char* d, size_t s)
  {
    throw std::runtime_error("state error"); 
    return 0;
  }
};

}}}

#endif
