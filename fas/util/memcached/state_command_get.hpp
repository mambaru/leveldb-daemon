//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_UTIL_MEMCACHED_STATE_COMMAND_GET_HPP
#define FAS_UTIL_MEMCACHED_STATE_COMMAND_GET_HPP

#include <fas/pattern/state.hpp>
#include <fas/util/memcached/states.hpp>
#include <fas/util/memcached/istate.hpp>
#include <fas/util/memcached/data_type.hpp>
#include <fas/util/memcached/tags.hpp>
#include <vector>

#include <iostream>
namespace fas{ namespace util { namespace memcached{

namespace ap = ::fas::pattern;


struct state_command_get
 : ap::state<istate, MC_get>
{
  typedef ap::state<istate, MC_get> super;

  state_command_get(): _head_ready(false) {}
  template<typename T>
  void operator()(T& t, const std::string& key)
  {
    std::string query = "get ";
    query += key;
    query += "\r\n";
    t.get_aspect().template get<_writer_>()(t, query.c_str(), query.size());
  }

  /** Неад всегда целиком и без хвоста. Тело как придет
    * @return сколько байт отработанно */
  template<typename T>
  void operator()(T& t, const char* d, size_t s)
  {

    if (s==0) return ;

    while (s!=0)
    {
      if (!_head_ready)
      {
        _value.data.clear();
        _value.key.clear();
        _value.flags = 0;
        _value.bytes = 0;
        char key[256];
        int res = sscanf(d, "VALUE %s %hu %d\r\n", 
                           key, &_value.flags, &_value.bytes);
        if (res == 3)
        {
          _value.key = key;
          _head_ready = true;
          for (size_t i = 0; i < s && d[i]!='\n'; ++i, --s, ++d );
          ++d; --s;
        // if (i < s) ++i;
        }
        else
        {
          // Error
          t.get_aspect().template get<_get_>()(t);
          for (size_t i = 0; i < s && *d!='\n'; ++i, --s, ++d );
          if (s > 0)
            ++d; --s;
          continue;
        }
      }

      size_t need = _value.bytes - _value.data.size();

      if ( need > s )
      {
        std::copy(d, d + s, std::back_inserter(_value.data));
        need -= s;
        d+=s;
        s = 0;
      }
      else if ( need > 0 )
      {
        std::copy(d, d + need, std::back_inserter(_value.data));
        d+=need;
        s-=need;
        need = 0;
      }

      if (need == 0)
      {
        t.get_aspect().template get<_get_>()(t, _value);
        _head_ready = false;
      }
    }
  }

private:
  bool _head_ready;
  value _value;
};

}}}

#endif
