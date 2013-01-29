//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_UTIL_MEMCACHED_AD_SERVER_PARSER_HPP
#define FAS_UTIL_MEMCACHED_AD_SERVER_PARSER_HPP

#include <fas/pattern/state.hpp>
#include <fas/util/memcached/istate.hpp>
#include <fas/util/memcached/data_type.hpp>
#include <fas/util/memcached/tags.hpp>
#include <fas/util/memcached/states.hpp>

namespace fas{ namespace util { namespace memcached{

namespace ap = ::fas::pattern;

/** @param L - state list*/
template<typename L>
class ad_server_parser
 : public ap::state_context< istate,  L  >
{
  typedef ap::state_context< istate,  L  > super;
  typedef ad_server_parser<  L  > self;

public:

  template<typename T>
  void operator()(T& t, const char* d, size_t s)
  {
    size_t cs = 0;
    while (cs < s)
    {
      if (super::get_state_id() == MC_ready)
        self::_do_check(L(), t, d+ cs, s - cs);
      if (super::get_state_id() != MC_ready)
      {
        if ( size_t ss = self::_doit(L(), t, d + cs, s - cs)) 
          cs += ss;
        else
           break;
      }
      else
        break
        ;
    }
      //error
    // self::_doit(L(), t, d, s);
  }

  /*template<typename T>
  void client_error(T& t, const std::string& message)
  {
    std::string err("CLIENT_ERROR ");
    err+=message;
    err+="\r\n";
    t.get_aspect().template get<_writer_>()(t, err.c_str(), err.size());
  }

  template<typename T>
  void server_error(T& t, const std::string& message)
  {
    std::string err("SERVER_ERROR ");
    err+=message;
    err+="\r\n";
    t.get_aspect().template get<_writer_>()(t, err.c_str(), err.size());
  }

  template<typename T>
  void error(T& t)
  {
    t.get_aspect().template get<_writer_>()(t, "ERROR\r\n", 7);
  }

  template<typename T>
  void stored(T& t, bool result)
  {
    t.get_aspect().template get<_writer_>()(
      t,
      result ? "STORED\r\n": "NOT_STORED\r\n",
      result ? 8 : 12
    );
  }

  template<typename T>
  void responce_get(T& t, const std::vector<value>& v)
  {
    std::vector<char> data;
    data.reserve(v.size()*128 + 512);
    std::vector<value>::const_iterator itr = v.begin();
    for (;itr!=v.end();++itr)
      _make_value(*itr, data );
    data.push_back('E'); data.push_back('N'); data.push_back('D');
    data.push_back('\r');data.push_back('\n');
    t.get_aspect().template get<_writer_>()(t, &data[0], data.size());
  }*/
  
  void ready()
  {
    super::cast_event(MC_ready);
  }

private:

  /*void _make_value(const value& v, std::vector<char>& data )
  {
    char h[512];
    int s = snprintf(h, 512, "VALUE %s %hu %d\r\n", v.key.c_str(), v.flags, v.bytes );
    if (s>0)
      std::copy(h, h + s, std::back_inserter(data));
    std::copy(v.data.begin(), v.data.end(), std::back_inserter(data));
    data.push_back('\r');data.push_back('\n');
  }*/

  template<typename LL, typename RL, typename T >
  void _do_check( ap::type_list<LL, RL> , T& t, const char* d, size_t s)
  {
    if ( super::template get<LL::state_id>().check(d, s) )
      super::cast_event(LL::state_id);
    else
      _do_check( RL(), t, d, s);
  }


  template< typename LL, typename T >
  void _do_check( ap::type_list<LL, ap::empty_type> , T& t, const char* d, size_t s)
  {
    if ( super::template get<LL::state_id>().check(d, s) )
      super::cast_event(LL::state_id);
  }


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
    //else
      // SERVER_ERROR;
    return s;
  }
  
};

}}}

#endif
