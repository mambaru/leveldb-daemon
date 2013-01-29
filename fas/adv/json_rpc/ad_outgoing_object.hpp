//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_ADV_JSON_RPC_AD_OUTGOING_OBJECT_HPP
#define FAS_ADV_JSON_RPC_AD_OUTGOING_OBJECT_HPP

#include <vector>
#include <fas/pattern/type_list.hpp>
#include <fas/misc/json.hpp>

namespace fas{ namespace adv{ namespace json_rpc{

namespace ap = ::fas::pattern;
namespace amj = ::fas::json;

class ad_outgoing_object
{
  typedef std::vector<char> data_type;
  data_type _data;
  amj::json_pack _outgoing;
public:
  amj::json_pack& get_outgoing() { return _outgoing; };

  template<typename T>
  void clear(T&)
  {
    _data.clear();
  }

  const char* data() const
  {
    return _outgoing.begin();
  }

  size_t size() const
  {
    return _outgoing.size();
  }

  void initialize(size_t s)
  {
    _data.clear();
    this->reserve(s);
    _outgoing<<'{'<<"jsonrpc"<<':'<<"2.0"<<','<<"result"<<':';
  }

  void initialize(size_t s, const char* name)
  {
    _data.clear();
    this->reserve(s);
    _outgoing<<'{'<<"jsonrpc"<<':'<<"2.0"<<','<<"method"<<':'<<name<<','<<"params"<<':';
  }

  void finalize(int id)
  {
    _outgoing<<','<<"id"<<':'<<id<<'}';
  }

  void finalize()
  {
    _outgoing<<'}';
  }


  // Обязателен вызов перед формированием ответа адвайсом пользователя
  void reserve(size_t s) 
  {
    /*if ( _data.size() < s + 128 )
    {*/
      _data.resize(s + 16635, 0);
      char* beg = &(_data[0]);
      char* end = beg + _data.size();
      _outgoing = amj::json_pack( beg, end );
    // }
  }

  //TODO: сделать проверку да двойной вызов result
  // json_object и массивы больше 5 записываем вручную
  void result()
  {
    _outgoing<<'n'<<'u'<<'l'<<'l';
  }

  template<typename T0>
  void result(const T0& p0)
  {
    _outgoing<<p0;
  }

  template<typename T0, typename T1>
  void result(const T0& p0, const T1& p1)
  {
    _outgoing<<'['<<p0<<','<<p1<<']';
  }

  template<typename T0, typename T1, typename T2>
  void result(const T0& p0, const T1& p1, const T2& p2)
  {
    _outgoing<<'['<<p0<<','<<p1<<','<<p2<<']';
  }

  template<typename T0, typename T1, typename T2, typename T3>
  void result(const T0& p0, const T1& p1, const T2& p2, const T3& p3)
  {
    _outgoing<<'['<<p0<<','<<p1<<','<<p2<<','<<p3<<']';
  }

  template<typename T0, typename T1, typename T2, typename T3, typename T4>
  void result(const T0& p0, const T1& p1, const T2& p2, const T3& p3, const T4& p4)
  {
    _outgoing<<'['<<p0<<','<<p1<<','<<p2<<','<<p3<<']';
  }

};

}}}

#endif
