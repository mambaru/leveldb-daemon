//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_UTIL_MEMCACHED_STATE_STORAGE_HPP
#define FAS_UTIL_MEMCACHED_STATE_STORAGE_HPP

#include <fas/pattern/state.hpp>
#include <fas/util/memcached/states.hpp>
#include <fas/util/memcached/istate.hpp>
#include <fas/util/memcached/data_type.hpp>
#include <fas/util/memcached/tags.hpp>
#include <vector>
#include <algorithm>

#include <iostream>
namespace fas{ namespace util { namespace memcached{

namespace ap = ::fas::pattern;

/**
<command name> <key> <flags> <exptime> <bytes>\r\n
struct storage
{
  std::string key;
  std::vector<char> data;
  unsigned short flags;
  int exptime;
  int bytes;
};

*/

template<state_mode s>
struct state_storage;

template<>
struct state_storage<mode_server>
 : ap::state<istate, MC_storage>
{
  typedef ap::state<istate, MC_storage> super;

  state_storage():_head_ready(false){}

  bool check(const char* d, size_t s)
  {
    if ( s > 3 && d[3]==' ' && ( (d[0]=='s' && d[1]=='e' && d[2]=='t') ||
                                 (d[0]=='a' && d[1]=='d' && d[2]=='d') ) )
      return  true;

    if ( s > 7 && d[7]==' ' && d[0]=='r' && d[1]=='e' && 
         d[2]=='p' && d[3]=='l' && d[4]=='a' && d[5]=='c' && d[6]=='e')
      return  true;
    return false;
  }
  
  /** @return колько байт отработано, остальное не для текущей комманды*/
  template<typename T>
  size_t operator()(T& t, const char* d, size_t s)
  {
    if (s == 0 ) return 0;
    const char* pbeg = d;
    const char* pend = d + s;
    if ( !_head_ready )
    {
      for (;pbeg!=pend && *pbeg!='\n'; ++pbeg);
      if ( pbeg!=pend ) ++pbeg;
      const char* phead = d;
      if (!_head.empty() || *(pbeg-1)!='\n')
      {
        std::copy(d, pbeg, std::back_inserter(_head));
        if ( _head.back()!='\n' )
          return s;
        phead = &_head[0];
      }

      char cmd[10];
      char key[256];
      int res = sscanf(phead, "%s %s %hu %d %d\r\n", 
                       cmd, key, &_storage.flags, &_storage.exptime, &_storage.bytes);
      bool ok = res == 5;
      if ( ok )
      {
        _storage.key = key;
        if ( cmd[0]=='s' && cmd[1]=='e' && cmd[2]=='t')
          _storage.command = storage::set;
        else if ( cmd[0]=='a' && cmd[1]=='d' && cmd[2]=='d')
          _storage.command = storage::add;
        else if ( cmd[0]=='r' && cmd[1]=='e' && cmd[2]=='p' && cmd[3]=='l' 
                  && cmd[4]=='a' && cmd[5]=='c' && cmd[6]=='e')
          _storage.command = storage::replace;
        else
          ok = false;
      }
      if (!ok)
        throw std::runtime_error("CLIENT_ERROR bad command line format\r\n");
      _storage.data.clear();
      _storage.bytes+=2;
      _head_ready = true;
      _head.clear();
    }

    if ( pend - pbeg == 0) return s;

    ptrdiff_t need = _storage.bytes - _storage.data.size();
    if ( need < std::distance(pbeg, pend) )
    {
      std::copy( pbeg, pbeg + need, std::back_inserter(_storage.data) );
      pbeg += need;
      need = 0;
    }
    else
    {
      std::copy( pbeg, pend, std::back_inserter(_storage.data) );;
      need -= pend - pbeg;
      pbeg = pend;
    }

    if (need == 0)
    {
      _head_ready = false;
      if ( _storage.data.size() < 2 ||
           _storage.data[_storage.data.size()-2]!='\r' || 
           _storage.data[_storage.data.size()-1]!='\n')
             throw std::runtime_error("CLIENT_ERROR bad data chunk\r\n");

      _storage.bytes-=2;
      _storage.data.resize(_storage.bytes);
      if (t.get_aspect().template get<_storage_>()(t, _storage))
        t.get_aspect().template get<_writer_>()(t, "STORED\r\n", 8);
      else
        t.get_aspect().template get<_writer_>()(t, "NOT_STORED\r\n", 12);
      t.get_aspect().template get<_parser_>().ready();
    }

    return s - ( pend -pbeg );
  }

private:
  bool _head_ready;
  storage _storage;
  std::vector<char> _head;
};


template<>
struct state_storage<mode_client>
 : ap::state<istate, MC_storage>
{
  typedef ap::state<istate, MC_storage> super;

  state_storage(){}

  template<typename T>
  void operator()(T& t, const storage& stg)
  {
//    stg.bytes = std.data.size();
    const char* cmd = "set";
    std::vector<char> data;
    data.resize(1024);
    int s = snprintf(&data[0], 1024, "%s %s %hu %d %zu\r\n",
                        cmd, stg.key.c_str(), stg.flags, stg.exptime, stg.data.size());
    data.resize(s);
    if (!stg.data.empty())
      std::copy( stg.data.begin(), stg.data.end(), std::back_inserter(data) );
    data.push_back('\r');
    data.push_back('\n');
    t.get_aspect().template get<_writer_>()(t, &data[0], data.size());
  }

  /** @return колько байт отработано, остальное не для текущей комманды*/
  template<typename T>
  size_t operator()(T& t, const char* d, size_t s)
  {
    
    const char* pbeg = d;
    const char* pend = d + s;
    const char* rbeg = d;
    const char* rend = d;
 
    for (;pbeg!=pend && *pbeg!='\n'; ++pbeg);
    if ( pbeg!=pend ) ++pbeg;
    if (!_data.empty() || *(pbeg-1)!='\n')
    {
      std::copy(d, pbeg, std::back_inserter(_data));
      if (_data.back()!='\n')
        return s;
      rbeg = &_data[0];
      rend = rbeg + _data.size();
    }
    else
      rend = pbeg;

    if ( rend - rbeg == 8 &&  0 == std::strncmp(rbeg, "STORED\r\n", rend - rbeg)  )
      t.get_aspect().template get<_storage_>()(t, true);
    else if ( rend - rbeg == 12 &&  0 == std::strncmp(rbeg, "NOT_STORED\r\n", rend - rbeg)  )
      t.get_aspect().template get<_storage_>()(t, false);
    else
      throw std::runtime_error( std::string(rbeg, rend) );

    _data.clear();
     t.get_aspect().template get<_parser_>().ready();
    return s - ( pend -pbeg );
  }

private:
  std::vector<char> _data;
};

}}}

#endif
