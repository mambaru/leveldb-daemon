//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_UTIL_AMF_UNSERIALIZE_HPP
#define FAS_UTIL_AMF_UNSERIALIZE_HPP

#include <string>
#include <vector>

#include <fas/util/amf/types.hpp>
#include <fas/util/multi_type.hpp>

namespace fas{ namespace util{ namespace amf{

class unserialize_base
{
public:

  unserialize_base()
  {
  }

  unserialize_base(const char* d, size_t s)
    : _amf_pack(d)
    , _itr(d)
    , _end(d+s)
    , _size(s)
  {
  }

  void assign( const char* d, size_t s)
  {
    _amf_pack = d;
    _itr = _amf_pack;
    _end = d + s;
    _size = s;
  }


  unsigned char get_type() const
  {
    if ( _itr >= _end )
      throw std::out_of_range("amf_unserialize::get_type");
    return *(_itr++);
  }

  bool get_bool() const
  {
    if ( _itr >= _end )
      throw std::out_of_range("amf_unserialize::get_bool");
    return *(_itr++)!=0;
  }

  std::string get_string() const
  {
    if ( _itr + 2 > _end )
      throw std::out_of_range("amf_unserialize::get_string");

    uint16_t string_size = 0;
    reinterpret_cast<char*>(&string_size)[1] = *(_itr++);
    reinterpret_cast<char*>(&string_size)[0] = *(_itr++);

    if ( _itr + string_size > _end )
      throw std::out_of_range("amf_unserialize::get_string");
    const char* _str = _itr;
    _itr += string_size;
    return std::string(_str, _str + string_size);
  }

  double get_number() const
  {
    if ( _itr + 8 > _end )
      throw std::out_of_range("amf_unserialize::get_string");

    double result = 0;
    reinterpret_cast<char*>(&result)[7] = *(_itr++);
    reinterpret_cast<char*>(&result)[6] = *(_itr++);
    reinterpret_cast<char*>(&result)[5] = *(_itr++);
    reinterpret_cast<char*>(&result)[4] = *(_itr++);	
    reinterpret_cast<char*>(&result)[3] = *(_itr++);
    reinterpret_cast<char*>(&result)[2] = *(_itr++);
    reinterpret_cast<char*>(&result)[1] = *(_itr++);
    reinterpret_cast<char*>(&result)[0] = *(_itr++);
    return result;
  }

  operator bool () const
  {
    return _itr!=_end;
  }
private:
  const char* _amf_pack;
  mutable const char* _itr;
  const char* _end;
  size_t _size;
};

class unserialize
  : public unserialize_base
{
public:

  unserialize() {}

  unserialize(const char* d, size_t s)
    : unserialize_base(d, s)
  {
  }

  const unserialize& operator >> (bool& value) const
  {
    if ( this->get_type() != AMF_BOOL)
      throw std::runtime_error("invalid read data type. Must be bool");

    value = this->get_bool();
    return *this;
  }

  const unserialize& operator >> (double& value) const
  {
    if ( this->get_type() != AMF_NUMBER)
      throw std::runtime_error("invalid read data type. Must be number");

    value = this->get_number();
    return *this;
  }

  const unserialize& operator >> (std::string& value) const
  {
    if ( this->get_type() != AMF_STRING)
      throw std::runtime_error("invalid read data type. Must be string");

    value = this->get_string();
    return *this;
  }

  const unserialize& operator >> ( multi_type& mt) const
  {
    unsigned char type = this->get_type();
    return this->_get_mt(type, mt);
  }

  const unserialize& _get_hash(multi_type& mt) const
  {
    while (*this)
    {
      std::string key = this->get_string();
      unsigned char type = this->get_type();
      if ( type==AMF_END ) break;
      this->_get_mt(type, mt[key]);
    }
    return *this;
  }

  const unserialize& _get_mt(unsigned char type, multi_type& mt) const
  {
    if ( type == AMF_BOOL )
      mt = this->get_bool();
    else if ( type == AMF_NUMBER )
      mt = this->get_number();
    else if ( type == AMF_STRING )
      mt = this->get_string();
    else if ( type == AMF_HASH )
      this->_get_hash(mt);
    return *this;
  }
};

}}}

#endif
