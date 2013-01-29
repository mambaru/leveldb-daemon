//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_UTIL_AMF_UNSERIALIZE_INVOKE_HPP
#define FAS_UTIL_AMF_UNSERIALIZE_INVOKE_HPP

#include <string>
#include <vector>

#include <fas/util/amf/types.hpp>
#include <fas/util/amf/unserialize.hpp>
#include <fas/util/multi_type.hpp>

namespace fas{ namespace util{ namespace amf{

class unserialize_invoke
  : public unserialize
{
public:

  unserialize_invoke(){}

  unserialize_invoke( const char* d, size_t s)
  {
    this->assign(d, s);
  }

  void assign( const char* d, size_t s)
  {
      _h1 = reinterpret_cast<const amf_head_1*>(d);
      if ( _h1->head_size() >= 8)
      {
        unserialize::assign( d + _h1->head_size() , s - _h1->head_size() );
        unserialize::operator >>( _function );
        unserialize::operator >>( _call_id );
        if ( *this && _function!="connect" && unserialize::get_type() != AMF_NIL)
          throw std::runtime_error("must be AMF_NIL");
      }
  }

  const unserialize_invoke& operator >> ( multi_type& mt) const
  {
    for ( int i = 0; *this; ++i)
    {
      unsigned char type = unserialize::get_type();
      if (type!=AMF_NIL)
        unserialize::_get_mt(type, mt[i]);
    }
    return *this;
  }


  const std::string& name() const
  {
    return _function;
  }

  double call_id() const 
  {
    return _call_id;
  }

  unsigned int object_id() const
  {
    return this->head()->object_id();
  }

  const amf_head_1* head() const
  {
    return _h1;
  }

private:
  const amf_head_1* _h1;
  std::string _function;
  double _call_id;
};


}}}

#endif
