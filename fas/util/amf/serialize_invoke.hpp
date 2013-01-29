//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_UTIL_AMF_SERIALIZE_INVOKE_HPP
#define FAS_UTIL_AMF_SERIALIZE_INVOKE_HPP

#include <string>
#include <vector>

#include <fas/util/amf/tags.hpp>
#include <fas/util/amf/types.hpp>
#include <fas/util/amf/serialize.hpp>
#include <fas/util/multi_type.hpp>

namespace fas{ namespace util{ namespace amf{

static const unsigned char invoke_head[]=
{
  0x03,
  0x00, 0x00, 0x00,
  0x00, 0x00, 0x00,
  0x14,
  0x00, 0x00, 0x00, 0x00
};


class serialize_invoke
  : public serialize
{
  std::string _name;
public:

  const std::string& name() const {return _name;};

  serialize_invoke(const std::string& function, double number, size_t reserve = 512)
    : serialize( reserve, 0, sizeof(invoke_head) ), _name(function)
  {
    std::copy( invoke_head,
               invoke_head + sizeof(invoke_head),
               std::back_inserter( _amf_pack ) );

    (*this)<<function<<number;
    _amf_pack.push_back(AMF_NIL);
  }

  void finalize(unsigned int object_id)
  {
    amf_head_8* h8 = static_cast<amf_head_8*>(this->head());
    h8->body_size( _amf_pack.size() - sizeof(invoke_head) );
    h8->object_id( object_id );
  }
};

}}}

#endif
