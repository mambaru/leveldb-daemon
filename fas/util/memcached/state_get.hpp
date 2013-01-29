//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_UTIL_MEMCACHED_STATE_GET_HPP
#define FAS_UTIL_MEMCACHED_STATE_GET_HPP

#include <fas/pattern/state.hpp>
#include <fas/util/memcached/states.hpp>
#include <fas/util/memcached/istate.hpp>
#include <fas/util/memcached/data_type.hpp>
#include <fas/util/memcached/tags.hpp>
#include <vector>

#include <iostream>
namespace fas{ namespace util { namespace memcached{

namespace ap = ::fas::pattern;

template<state_mode s>
struct state_get;

template<>
struct state_get<mode_server>
 : ap::state<istate, MC_get>
{
  typedef ap::state<istate, MC_get> super;

  bool check(const char* d, size_t s)
  {
    if ( s > 3 && d[3]==' ' && d[0]=='g' && d[1]=='e' && d[2]=='t' )
      return  true;
    return false;
  }

  /** Неад всегда целиком и без хвоста. Тело как придет
    * @return сколько байт отработанно */
  template<typename T>
  size_t operator()(T& t, const char* d, size_t s)
  {
    if (s == 0 ) return 0;
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

    if ( std::distance(rbeg, rend) < 4 || rbeg[0]!='g' && rbeg[1]!='e' && rbeg[2]!='t' )
      throw std::runtime_error("ERROR\r\n");

/*
VALUE <key> <flags> <bytes>\r\n
<data block>\r\n
*/

    std::vector<value> vv;
    rbeg+=4;
    while (rbeg<rend)
    {
      value v;
      for (;rbeg!=rend && (*rbeg==' ' || *rbeg=='\r' || *rbeg=='\n');++rbeg);
      const char* rcur = rbeg;
      for (;rcur!=rend && *rcur!=' ' && *rcur!='\r' ; ++rcur);
      if ( std::distance(rbeg, rcur) > 0 && *rbeg!='\r' &&
           t.get_aspect().template get<_get_>()(t, std::string(rbeg, rcur), v) )
            vv.push_back(v);
      rbeg = rcur;
    }

    std::vector<value>::iterator beg = vv.begin();
    std::vector<value>::iterator end = vv.end();
    std::vector<char> result;
    result.reserve(512);
    char buff[512];
    for (;beg!=end;++beg)
    {
      if ( int sp = snprintf(buff, 512, "VALUE %s %hu %zu\r\n",
                             beg->key.c_str(), beg->flags, beg->data.size())
         )
            std::copy(buff, buff+sp, std::back_inserter(result));
      if (!beg->data.empty())
        std::copy(beg->data.begin(), beg->data.end(), std::back_inserter(result));
      result.push_back('\r'); result.push_back('\n');
    }
    result.push_back('E'); result.push_back('N'); result.push_back('D');
    result.push_back('\r'); result.push_back('\n');
    t.get_aspect().template get<_writer_>()(t, &(result[0]), result.size());
    t.get_aspect().template get<_parser_>().ready();
    return s - std::distance(pbeg, pend);
  }

private:
  std::vector<char> _data;
};

/*
VALUE <key> <flags> <bytes>\r\n
<data block>\r\n
*/
template<>
struct state_get<mode_client>
 : ap::state<istate, MC_get>
{
  typedef ap::state<istate, MC_get> super;

  state_get(): _head_ready(false){}
  template<typename T>
  void operator()(T& t, const std::string& keys)
  {
    std::string cmd = "get ";
    cmd +=keys;
    cmd +="\r\n";
    t.get_aspect().template get<_writer_>()(t, cmd.c_str(), cmd.size());
  }

  /** Неад всегда целиком и без хвоста. Тело как придет
    * @return сколько байт отработанно */
  template<typename T>
  size_t operator()(T& t, const char* d, size_t s)
  {
/*
VALUE <key> <flags> <bytes>\r\n
<data block>\r\n
END\r\n
*/
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

      char key[256];
      int res = sscanf(phead, "VALUE %s %hu %d\r\n", 
                       key, &_value.flags, &_value.bytes);
      bool ok = res == 3;
      if ( ok )
        _value.key = key;
      if (!ok)
      {
        if ( std::strncmp(phead, "END\r\n", 5)==0 )
        {
          t.get_aspect().template get<_get_>()(t);
          t.get_aspect().template get<_parser_>().ready();
          return s - ( pend -pbeg );
        }
        else
          throw std::runtime_error("SERVER_ERROR bad command line format\r\n");
      }
        /*throw std::runtime_error("CLIENT_ERROR bad command line format\r\n");*/
      _value.data.clear();
      _value.bytes+=2;
      _head_ready = true;
      _head.clear();
    }

    if ( pend - pbeg == 0) return s;

    ptrdiff_t need = _value.bytes - _value.data.size();
    if ( need < std::distance(pbeg, pend) )
    {
      std::copy( pbeg, pbeg + need, std::back_inserter(_value.data) );
      pbeg += need;
      need = 0;
    }
    else
    {
      std::copy( pbeg, pend, std::back_inserter(_value.data) );
      need -= pend - pbeg;
      pbeg = pend;
    }

    if (need == 0)
    {
      _head_ready = false;
      if ( _value.data.size() < 2 ||
           _value.data[_value.data.size()-2]!='\r' || 
           _value.data[_value.data.size()-1]!='\n')
             throw std::runtime_error("CLIENT_ERROR bad data chunk\r\n");

      _value.bytes-=2;
      _value.data.resize(_value.bytes);
      t.get_aspect().template get<_get_>()(t, _value);
      // t.get_aspect().template get<_parser_>().ready();
    }

    return s - ( pend -pbeg );
  }

private:
  std::vector<char> _head;
  bool _head_ready;
  value _value;
};

}}}

#endif
