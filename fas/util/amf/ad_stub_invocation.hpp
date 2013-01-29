//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_UTIL_AMF_AD_STUB_INVOCATION_HPP
#define FAS_UTIL_AMF_AD_STUB_INVOCATION_HPP

#include <iostream>
#include <string>

#include <fas/util/amf/unserialize_invoke.hpp>
#include <fas/util/amf/serialize_invoke.hpp>
#include <fas/util/multi_type.hpp>
#include <fas/pattern/type_list.hpp>
#include <fas/pattern/ref.hpp>
#include <fas/util/amf/tags.hpp>

namespace fas{ namespace  util{ namespace amf{

namespace ap = ::fas::pattern;
template<typename IL>
class ad_stub_invocation
{
  typedef typename ap::type_list_traits<IL>::type invoke_list;
  double _counter;
  std::string _name;
  // size_t - номер в списке
  typedef std::map<double, size_t> call_map;
  typedef std::map<std::string, size_t> name_map;
  call_map _call_map;
  name_map _name_map;
public:

  ad_stub_invocation():_counter(10000), _name("_result"){}
  const std::string& name() const { return _name;}

  template<typename T>
  bool operator()(T& t, const unserialize_invoke& ui, const serialize_invoke& si)
  {
    call_map::iterator itr = _call_map.find(ui.call_id());
    if ( itr == _call_map.end() )
      return false;
    _call(t, ui, itr->second, invoke_list() );
    return true;
  }

  template<typename T, typename L, typename R>
  void _call(T& t, const unserialize_invoke& ui, size_t pos, ap::type_list<L, R>)
  {
    if (pos == 0)
      t.get_aspect().template get<L>()(t, ui);
    else
      _call(t, ui, pos - 1, R());
  }

  template<typename T>
  void _call(T& , const unserialize_invoke& , size_t , ap::empty_type)
  {
  }

  template<typename T>
  void operator()(T& t, const std::string& name, unsigned int obj_id)
  {
    _invoke(t, name, obj_id, ++_counter);
  }

  template<typename T>
  void operator()(T& t, double call_id, unsigned int obj_id)
  {
    _invoke(t, "_result", obj_id, call_id);
  }


  template<typename T>
  void operator()(T& t, const std::string& name, unsigned int obj_id, const multi_type& mtp)
  {
    _invoke(t, name, obj_id, ++_counter, ap::cref(mtp));
  }

  template<typename T>
  void operator()(T& t, double call_id, unsigned int obj_id, const multi_type& mtp)
  {
    _invoke(t, "_result", obj_id, call_id, ap::cref(mtp));
  }

  template<typename T, typename P0>
  void operator()(T& t, const std::string& name, unsigned int obj_id, P0 p0)
  {
    multi_type mt;
    mt[0] = p0;
    _invoke(t, name, obj_id, ++_counter, ap::cref(mt));
  }

  template<typename T, typename P0>
  void operator()(T& t, double call_id, unsigned int obj_id, P0 p0)
  {
    multi_type mt;
    mt = p0;
    _invoke(t, "_result", obj_id, call_id, ap::cref(mt));
  }

  template<typename T, typename P0, typename P1>
  void operator()(T& t, const std::string& name, unsigned int obj_id, P0 p0, P1 p1)
  {
    multi_type mt;
    mt[0] = p0; mt[1] = p1;
    _invoke(t, name, obj_id, ++_counter, ap::cref(mt));
  }

  template<typename T, typename P0, typename P1, typename P2 >
  void operator()(T& t, const std::string& name, unsigned int obj_id, P0 p0, P1 p1, P2 p2)
  {
    multi_type mt;
    mt[0] = p0; mt[1] = p1; mt[2] = p2;
    _invoke(t, name, obj_id, ++_counter, ap::cref(mt));
  }

  template<typename T, typename P0, typename P1, typename P2, typename P3 >
  void operator()(T& t, const std::string& name, unsigned int obj_id, P0 p0, P1 p1, P2 p2, P3 p3)
  {
    multi_type mt;
    mt[0] = p0; mt[1] = p1; mt[2] = p2; mt[3] = p3;
    _invoke(t, name, obj_id, ++_counter, ap::cref(mt));
  }

  template<typename T, typename P0, typename P1, typename P2, typename P3, typename P4 >
  void operator()(T& t, const std::string& name, unsigned int obj_id, P0 p0, P1 p1, P2 p2, P4 p4)
  {
    multi_type mt;
    mt[0] = p0; mt[1] = p1; mt[2] = p2; mt[4] = p4;
    _invoke(t, name, obj_id, ++_counter, ap::cref(mt));
  }

private:

  template<typename T>
  void _invoke(T& t, const std::string& name, unsigned int obj_id, double call_id, const multi_type& mt)
  {
    // ++_counter;
    size_t pos = _get_position(t, name);
    if ( pos != ap::length<invoke_list>::result && name!="_result")
      _call_map.insert( std::make_pair(call_id, pos) );
    serialize_invoke si( name, call_id);
    si<<mt;
    si.finalize(obj_id);
    t.get_aspect().template get<_chunk_prepare_>()( t, si );
  }

  template<typename T>
  void _invoke(T& t, const std::string& name, unsigned int obj_id, double call_id)
  {
    // ++_counter;
    size_t pos = _get_position(t, name);
    if ( pos != ap::length<invoke_list>::result && name!="_result" )
      _call_map.insert( std::make_pair(call_id, pos) );
    serialize_invoke si( name, call_id);
    si.finalize(obj_id);
    t.get_aspect().template get<_chunk_prepare_>()( t, si );
  }

  template<typename T>
  size_t _get_position( T& t, const std::string& name )
  {
    return _check_position(t, name, invoke_list());
  }

  template<typename T, typename R, typename L>
  size_t _check_position( T& t, const std::string& name, ap::type_list<L, R>)
  {
    if ( name == t.get_aspect().template get<L>().name() )
      return 0;
    return 1 + _check_position( t, name, R() );
  }

  template<typename T>
  size_t _check_position( T& , const std::string&, ap::empty_type)
  {
    return 0;
  }
};

}}}

#endif
