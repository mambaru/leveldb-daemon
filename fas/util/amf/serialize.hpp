//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_UTIL_AMF_SERIALIZE_HPP
#define FAS_UTIL_AMF_SERIALIZE_HPP

#include <string>
#include <vector>

#include <fas/util/amf/types.hpp>
#include <fas/util/multi_type.hpp>

namespace fas{ namespace util{ namespace amf{


class serialize_base
{
public:

  serialize_base(size_t reserve)
  {
    _amf_pack.reserve(reserve);
  }

  const char* data() const { return &_amf_pack[0]; }
  size_t size() const { return _amf_pack.size(); }

  void push_type(unsigned char type)
  {
    _amf_pack.push_back(type);
  }

  void push_bool( bool value )
  {
    _amf_pack.push_back(value);
  }


  void push_number( double value )
  {
    unsigned char* ptr_val = reinterpret_cast<unsigned char*>( &value );
    _amf_pack.push_back(ptr_val[7]);
    _amf_pack.push_back(ptr_val[6]);
    _amf_pack.push_back(ptr_val[5]);
    _amf_pack.push_back(ptr_val[4]);
    _amf_pack.push_back(ptr_val[3]);
    _amf_pack.push_back(ptr_val[2]);
    _amf_pack.push_back(ptr_val[1]);
    _amf_pack.push_back(ptr_val[0]);
  }


  void push_string(const std::string& value)
  {
    uint16_t size = static_cast<uint16_t>( value.size() );
    unsigned char* ptr_size = reinterpret_cast<unsigned char*>( &size );
    _amf_pack.push_back(ptr_size[1]);
    _amf_pack.push_back(ptr_size[0]);
    if ( !value.empty() )
      std::copy(value.begin(), value.end(), std::back_inserter(_amf_pack));
  }

  void push_hash_begin()
  {
    this->push_type(AMF_HASH);
  }

  void push_hash_end()
  {
    _amf_pack.push_back(0);
    _amf_pack.push_back(0);
    this->push_type(AMF_END);
  }

  /// Тип поля не кладем!!!
  void push_hash_key(const std::string& value)
  {
    uint16_t size = static_cast<uint16_t>( value.size() );
    unsigned char* ptr_size = reinterpret_cast<unsigned char*>( &size );
    _amf_pack.push_back(ptr_size[1]);
    _amf_pack.push_back(ptr_size[0]);
    if ( !value.empty() )
      std::copy(value.begin(), value.end(), std::back_inserter(_amf_pack));
  }

protected:
  std::vector<char> _amf_pack;
};


class serialize
  : public serialize_base
{
public:

  serialize(size_t reserve, size_t head_offset, size_t body_offset)
    : serialize_base(reserve)
    , _head_offset(head_offset)
    , _body_offset(body_offset)
  {}

  const char* body() const { return &_amf_pack[_body_offset]; }

  amf_head_1* head(){
    return reinterpret_cast<amf_head_1*>( &_amf_pack[_head_offset] );
  }

  const amf_head_1* head() const {
    return reinterpret_cast<const amf_head_1*>( &_amf_pack[_head_offset] );
  }

  serialize& operator << (bool value)
  {
    this->push_type(AMF_BOOL);
    this->push_bool(value);
    return *this;
  }

  serialize& operator << (double value)
  {
    this->push_type(AMF_NUMBER);
    this->push_number(value);
    return *this;
  }

  serialize& operator << (const std::string& value)
  {
    this->push_type(AMF_STRING);
    this->push_string(value);
    return *this;
  }

  serialize& operator << (const multi_type& mt)
  {
    mt_types type = mt.get_type();
    if ( type == mt_bool )
    {
      this->push_type(AMF_BOOL);
      this->push_bool(mt);
    }
    else if ( type == mt_char)
    {
      this->push_type(AMF_NUMBER);
      this->push_number( static_cast<double>( static_cast<char>(mt) ) );
    }
    else if ( type == mt_long)
    {
      this->push_type(AMF_NUMBER);
      this->push_number( static_cast<double>( static_cast<long>(mt) ) );
    }
    else if ( type == mt_double)
    {
      this->push_type(AMF_NUMBER);
      this->push_number( mt );
    }
    else if ( type == mt_string)
    {
      this->push_type(AMF_STRING);
      this->push_string( static_cast<const std::string&>(mt.operator const std::string &()) );
    }
    else if ( type == mt_shash )
    {
      this->push_hash_begin();
      mt_traits<mt_shash>::const_iterator beg = mt.begin<mt_shash>();
      mt_traits<mt_shash>::const_iterator end = mt.end<mt_shash>();
      for (;beg!=end;++beg)
      {
        this->push_hash_key( beg->first );
        this->operator << ( beg->second );
      }
      this->push_hash_end();

    }
    else if ( type == mt_nhash )
    {
      this->push_hash_begin();
      mt_traits<mt_nhash>::const_iterator beg = mt.begin<mt_nhash>();
      mt_traits<mt_nhash>::const_iterator end = mt.end<mt_nhash>();
      for (;beg!=end;++beg)
      {
        char ch[12];
        snprintf(ch, 12, "%d", beg->first);
        this->push_hash_key( ch );
        this->operator << ( beg->second );
      }
      this->push_hash_end();
    }
    return *this;
  }
private:
  size_t _head_offset;
  size_t _body_offset;

};

}}}

#endif
