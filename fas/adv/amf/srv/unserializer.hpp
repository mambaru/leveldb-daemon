//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_UTIL_AMF_UNSERIALIZE_HPP
#define FAS_UTIL_AMF_UNSERIALIZE_HPP

#include <cstddef>
#include <string>
#include <vector>

#include <fas/adv/amf/types.hpp>
#include <fas/adv/amf/array.hpp>

namespace fas{ namespace adv{ namespace amf{ namespace srv{

namespace ada = ::fas::adv::amf;

class unserializer
{
public:
  ptrdiff_t unserialize_double(const char* d, size_t s, double& result, bool check = true)
  {
    if ( s < 8U + check ) throw;

    const char* beg = d;
//    const char* end = d + s;

    if (check)
    {
      if ( *beg++ != AMF_NUMBER )
        throw;
    }

    reinterpret_cast<char*>(&result)[7] = *(beg++);
    reinterpret_cast<char*>(&result)[6] = *(beg++);
    reinterpret_cast<char*>(&result)[5] = *(beg++);
    reinterpret_cast<char*>(&result)[4] = *(beg++);	
    reinterpret_cast<char*>(&result)[3] = *(beg++);
    reinterpret_cast<char*>(&result)[2] = *(beg++);
    reinterpret_cast<char*>(&result)[1] = *(beg++);
    reinterpret_cast<char*>(&result)[0] = *(beg++); 
    return beg - d;  
  }

  ptrdiff_t unserialize_string(const char* d, size_t s, std::string& result, bool check = true)
  {
    if ( s < 2 ) throw;

    const char* beg = d;
    //const char* end = d + s;
    if (check)
    {
      if ( *beg++ != AMF_STRING )
        throw;
    }


    unsigned short string_size = 0;
    reinterpret_cast<char*>(&string_size)[1] = *(beg++);
    reinterpret_cast<char*>(&string_size)[0] = *(beg++);
    result.assign(beg, beg + string_size);
    beg+=string_size;
    return beg - d;  
  }

  ptrdiff_t unserialize(const char* d, size_t s, ada::array& arr)
  {
    
    const char* beg = d;
    const char* end = d + s;
    unsigned short string_size = 0;
    //while ( beg<end )
    //{
      unsigned char type = *(beg++);
      if (type==AMF_NIL)
        return beg - d;

      switch (type)
      {
      case AMF_BOOL: 
      arr = bool(*(beg++)!=0);
      return beg - d;
      break;
      case AMF_NUMBER: 
        {
        double result = 0;
        beg+=unserialize_double(beg, end - beg, result, false);
        /*reinterpret_cast<char*>(&result)[7] = *(beg++);
        reinterpret_cast<char*>(&result)[6] = *(beg++);
        reinterpret_cast<char*>(&result)[5] = *(beg++);
        reinterpret_cast<char*>(&result)[4] = *(beg++);	
        reinterpret_cast<char*>(&result)[3] = *(beg++);
        reinterpret_cast<char*>(&result)[2] = *(beg++);
        reinterpret_cast<char*>(&result)[1] = *(beg++);
        reinterpret_cast<char*>(&result)[0] = *(beg++); 
        */
        arr = result;
        return beg - d;
      }
      break;
      case AMF_STRING: 
      {
        std::string result;
        beg+=unserialize_string(beg, end - beg, result, false);
        arr = result;
        return beg - d;
/*
        reinterpret_cast<char*>(&string_size)[1] = *(beg++);
        reinterpret_cast<char*>(&string_size)[0] = *(beg++);
        arr = std::string(beg, beg + string_size);
        beg+=string_size;
        return beg - d;
        */
      }
      break;
      case AMF_HASH: 
        while (beg<end)
        {
          reinterpret_cast<char*>(&string_size)[1] = *(beg++);
          reinterpret_cast<char*>(&string_size)[0] = *(beg++);
          std::string key(beg, beg + string_size);
          beg+=string_size;
          if (*beg==AMF_END)
            return ++beg - d;
          if (beg > end) throw;
          beg += unserialize(beg, end - beg, arr[key]);
        }
        return beg - d;
        break;
      case AMF_UNDEF:
        // beg+=8;
//#pragma warning ("sdfsdfs1!")
        return end - d;
      default:throw;
      }
    }
  //}
};

class invoke_unserializer
  : unserializer
{
public:
  void unserialize(const char* d, size_t s, ada::invoke_info& arr)
  {
    std::cerr.write("\r\n++++++++++", 12);
    std::cerr.write(d, std::streamsize(s) );

    const char* beg = d;
    const char* end = d + s;

    const ada::amf_head_1* h1 = reinterpret_cast<const ada::amf_head_1*>(d);
    arr.set_object_id( h1->object_id() );
    beg+=h1->head_size();
    std::string name;
    double call_id;
    

    beg+=unserializer::unserialize_string(beg, end - beg, name);
    beg+=unserializer::unserialize_double(beg, end - beg, call_id);
    arr.set_name(name);
    arr.set_call_id(call_id);
    while (*beg==AMF_NIL && beg<end) ++beg;
    if (beg==end) return;

    if (beg > end) throw;
    for  ( int i =0; beg<end ; ++i )
    {
      beg+=unserializer::unserialize(beg, end - beg, arr[i]);
    }
  }
};

  

}}}}

#endif
