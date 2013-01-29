//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_UTIL_MEMCACHED_STATE_COMMAND_HPP
#define FAS_UTIL_MEMCACHED_STATE_COMMAND_HPP

#include <fas/pattern/state.hpp>
#include <fas/util/memcached/states.hpp>
#include <fas/util/memcached/istate.hpp>

#include <iostream>
namespace fas{ namespace util { namespace memcached{

namespace ap = ::fas::pattern;

struct state_command
 : ap::state<istate, MC_command>
{
  typedef ap::state<istate, MC_command> super;
  /** @return сколько байт отработанно */
  template<typename T>
  void operator()(T& t, const char* d, size_t s)
  {
    int id = MC_ready;

    if ( s > 3 && d[0]=='g' && d[1]=='e' && d[2]=='t' && d[3]==' ')
    {
      id = MC_get;
    }
    else if ( s > 3 && ( d[0]=='s' && d[1]=='e' && d[2]=='t' && d[3]==' '
                         || s > 3 && d[0]=='a' && d[1]=='d' && d[2]=='d' && d[3]==' ')
         )
    {
      // storage
      id = MC_storage;
    }
    else if ( s > 3 && ( d[0]=='d' && d[1]=='e' && d[2]=='c' && d[3]==' '
              || d[0]=='i' && d[1]=='n' && d[2]=='c' && d[3]==' ')
            )
    {
      // inc
    }
    else if (s > 3 && d[0]=='s' && d[1]=='t' && d[2]=='a' && d[3]=='t' && d[4]==' ' )
    {
      // stat
    }
    else if ( strcmp(d, "delete " )==0 )
    {
      // delete
    }
    else if ( strcmp(d, "replace " )==0 )
      id = MC_storage;

    super::cast_event(id);

    if (id!=MC_ready)
      t.get_aspect().template get<_parser_>()(t, d, s);
    else
      t.get_aspect().template get<_parser_>().error(t);

  }
private:
};

}}}

#endif
