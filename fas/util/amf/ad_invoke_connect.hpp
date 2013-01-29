//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_UTIL_AMF_AD_INVOKE_CONNECT_HPP
#define FAS_UTIL_AMF_AD_INVOKE_CONNECT_HPP

#include <fas/util/amf/tags.hpp>
#include <fas/util/amf/unserialize_invoke.hpp>
#include <fas/util/amf/serialize_invoke.hpp>

#include <string>

namespace fas{ namespace  util{ namespace amf{

class ad_invoke_connect
{
  std::string _name;
  bool _is_connect;
  double _call_id;
  unsigned int _object_id;

public:

  ad_invoke_connect()
    : _name("connect")
    , _is_connect(false)
    , _call_id(0)
    , _object_id(0xFF)
  {}

  const std::string& name() const { return _name;}

  template<typename T>
  void clear(T&)
  {
    _object_id = 0xFF;
    _call_id = 0;
    _is_connect = false;
  }

  bool is_connect() const 
  {
    return _is_connect;
  }

  double call_id() const
  {
    return _call_id;
  }

  unsigned int object_id() const
  {
    return _object_id;
  }

  template<typename T>
  bool operator()(T& t, const unserialize_invoke& ausi,
                        serialize_invoke& assr)
  {
    _object_id = ausi.head()->object_id();
    _call_id = ausi.call_id();
    _is_connect = true;
    multi_type mtr;
    mtr["level"] = "status";
    mtr["code"] = "NetConnection.Connect.Success";
    mtr["description"] = "Connection succeeded.";
    assr<<mtr;
    return true;
  }
};

}}}

#endif
