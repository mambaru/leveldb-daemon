//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_UTIL_MEMCACHED_AD_CLIENT_PARSER_HPP
#define FAS_UTIL_MEMCACHED_AD_CLIENT_PARSER_HPP

#include <fas/pattern/state.hpp>
#include <fas/util/memcached/istate.hpp>
#include <fas/util/memcached/data_type.hpp>
#include <fas/util/memcached/tags.hpp>
#include <fas/util/memcached/states.hpp>

#include <queue>
namespace fas{ namespace util { namespace memcached{

namespace ap = ::fas::pattern;

template<typename L>
class ad_client_parser
  : public ap::state_context< istate,  L  >
{
  typedef ad_client_parser<L> self;
  typedef ap::state_context< istate,  L  > super;
  typedef std::queue<states> states_queue;
public:

  ad_client_parser(): _ready(true){}

  /*
  template<typename T>
  void command_get(T& t, const std::string& key)
  {
    super::template get<MC_get>()(t, key);
    _sq.push(MC_get);
  }
  */

  template<typename T>
  void command_storage(T& t, const storage& stg)
  {
    super::template get<MC_storage>()(t, stg);
    _sq.push(MC_storage);
  }

  template<typename T>
  void command_get(T& t, const std::string& keys)
  {
    super::template get<MC_get>()(t, keys);
    _sq.push(MC_get);
  }
  
  template<typename T>
  void operator()(T& t, const char* d, size_t s)
  {
    size_t cs = 0;
    while (cs < s)
    {
      if (_ready)
      {
        if (_sq.empty())
          return;

        _ready = false;
        super::cast_event(_sq.front());
      }

      if ( size_t ss = self::_doit(L(), t, d + cs, s - cs)) 
          cs += ss;
        else
           break;
    }

/*    if (_ready)
    {
      if (_sq.empty())
        return;

      _ready = false;
      super::cast_event(_sq.front());
      
    }
    self::_doit(L(), t, d, s);
*/
  }

  void ready()
  {
    _sq.pop();
    _ready = true;
  }
  
private:
  bool _ready;
  states_queue _sq;

  template<typename LL, typename RL, typename T >
  size_t _doit( ap::type_list<LL, RL> , T& t, const char* d, size_t s)
  {
    if (LL::state_id == super::get_state_id())
      return super::template get<LL::state_id>()(t, d, s);
    else
      return _doit( RL(), t, d, s);
  }


  template< typename LL, typename T >
  size_t _doit( ap::type_list<LL, ap::empty_type> , T& t, const char* d, size_t s)
  {
    if (LL::state_id == super::get_state_id())
      return super::template get<LL::state_id>()(t, d, s);
    return s;
    /*else
      // SERVER_ERROR;
      ;*/
  }

};

}}}

#endif
