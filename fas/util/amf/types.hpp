//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_UTIL_AMF_TYPES_H
#define FAS_UTIL_AMF_TYPES_H

#include <string>
#include <stdexcept>


namespace fas{ namespace util{ namespace amf{

#ifdef WIN32
typedef unsigned __int8  uint8_t;
typedef unsigned __int16 uint16_t;
typedef unsigned __int32 uint32_t;
#endif
const size_t BYTE_SIZE       =   0x01;
const size_t SHORT_SIZE      =   0x02;
const size_t MEDIUM_INT_SIZE =   0x03;
const size_t LONG_SIZE       =   0x04;
const size_t DOUBLE_SIZE     =   0x08;

const size_t HANDSHAKE_SIZE  =  1536;
const size_t CHUNK_SIZE      =  128;

const int AMF_NUMBER      =  0x00;
const int AMF_BOOL        =  0x01;
const int AMF_STRING      =  0x02;
const int AMF_HASH        =  0x03;
const int AMF_NIL         =  0x05;
const int AMF_UNDEF       =  0x06;
const int AMF_LINK        =  0x07;
const int AMF_ASSOC_ARRAY =  0x08;
const int AMF_END         =  0x09;
const int AMF_ARRAY       =  0x0A;
const int AMF_DATE        =  0x0B;
const int AMF_LONG_STRING =  0x0C;
const int AMF_UNDEF_OBJ   =  0x0D;
const int AMF_XML         =  0x0E;
const int AMF_CUSTOM      =  0x0F;
const int AMF_MAX         =  0x10;

class amf_head_1
{
public:
  size_t head_size() const
  {
    switch(_head_size)
    {
      case 0: return 12;
      case 1: return 8;
      case 2: return 4;
      case 3: return 1;
    }
    return 0;
  }

  unsigned char object_id() const
  {
    return _object_id;
  }

  void head_size(size_t s) 
  {
    switch(s)
    {
      case 12: _head_size = 0; break;
      case 8: _head_size = 1; break;
      case 4: _head_size = 2; break;
      case 1: _head_size = 3; break;
      default: 
        throw std::runtime_error("amf_head_1: unsupported size");
    }
  }

  void object_id(unsigned char id) 
  {
    if (id > 64)
      throw std::out_of_range("amf_head_1: unsupported number");
    _object_id = id;
  }

private:
  unsigned char _object_id : 6;
  unsigned char _head_size : 2;
}
#ifndef WIN32
__attribute__ ((packed));;
#endif
;

class amf_head_4: public amf_head_1
{
  unsigned char unknown[3];
public:
  void data_type(unsigned int value )
  {
    unsigned char* pv = reinterpret_cast<unsigned char*>(&value);
    unknown[0] = pv[2];
    unknown[1] = pv[1];
    unknown[2] = pv[0];
  }
}
#ifndef WIN32
__attribute__ ((packed));;
#endif
;

class amf_head_8: public amf_head_4
{
public:
  unsigned char content_type() const
  {
    return _contetnt_type;
  }

  void content_type(unsigned char value) 
  {
    _contetnt_type = value;
  }

  size_t body_size() const
  {
    return (int)((_body_size_1<<16) | (_body_size_2<<8)) | _body_size_3;
  }

  void body_size(size_t value) 
  {
    unsigned char *val_ptr = reinterpret_cast<unsigned char *>(&value);
    _body_size_1 = val_ptr[2];
    _body_size_2 = val_ptr[1];
    _body_size_3 = val_ptr[0];
  }

private:

  uint32_t _body_size_1:8;
  uint32_t _body_size_2:8;
  uint32_t _body_size_3:8;
  uint32_t _contetnt_type:8;
}
#ifndef WIN32
__attribute__ ((packed));;
#endif
;

class amf_head_12: public amf_head_8
{
public:
  unsigned int destination() const { return _destination;}
  void destination(unsigned int value) 
  {
    (&_destination)[0] = (value >> 24) & 0xFF;
    (&_destination)[1] = (value >> 16) & 0xFF; 
    (&_destination)[2] = (value >> 8) & 0xFF;
    (&_destination)[3] = value & 0xFF;
  }
private:
  uint32_t _destination;
};


}}}

#endif
