//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_UTIL_AMF_SERIALIZER_HPP
#define FAS_UTIL_AMF_SERIALIZER_HPP

#include <string>
#include <vector>

//#include <fas/util/amf/types.hpp>
#include <fas/adv/amf/array.hpp>

namespace fas{ namespace adv{ namespace amf{ namespace srv{

namespace ada = ::fas::adv::amf;


class serializer
{
public:
  typedef std::vector<char> data_type;

    struct f_ser_int
  {
    serializer* _ser;
    serializer::data_type* _d;
    f_ser_int(serializer* ser, serializer::data_type* d):_ser(ser), _d(d){}
    template<typename T>
    void operator()(T& p)
    {
      if (p.first >= 0)
        _ser->serialize(p.second, *_d);
    }
  };

  struct f_ser_string
  {
    serializer* _ser;
    serializer::data_type* _d;
    f_ser_string(serializer* ser, serializer::data_type* d):_ser(ser), _d(d){}
    template<typename T>
    void operator()(T& p)
    {
      std::string str( p.first );
      unsigned short string_size = static_cast<unsigned short>(str.size());
      _d->push_back(reinterpret_cast<char*>(&string_size)[1]);
      _d->push_back(reinterpret_cast<char*>(&string_size)[0]);
      std::copy( str.begin(), str.end(), std::back_inserter(*_d) );

      _ser->serialize(p.second, *_d);
    }
  };


  void serialize_string(const std::string& str, data_type& d)
  {
      d.push_back(AMF_STRING);
      unsigned short string_size = static_cast<unsigned short>( str.size() );
      d.push_back(reinterpret_cast<char*>(&string_size)[1]);
      d.push_back(reinterpret_cast<char*>(&string_size)[0]);
      std::copy( str.begin(), str.end(), std::back_inserter(d) );
  }

  void serialize_number(double num, data_type& d)
  {
      d.push_back(AMF_NUMBER);
      const char *pv = reinterpret_cast<const char*>(&num);
      d.push_back(pv[7]);
      d.push_back(pv[6]);
      d.push_back(pv[5]);
      d.push_back(pv[4]);
      d.push_back(pv[3]);
      d.push_back(pv[2]);
      d.push_back(pv[1]);
      d.push_back(pv[0]);
  }

  void serialize(ada::array& arr, data_type& d)
  {
    if (arr.is<bool>())
    {
      d.push_back(AMF_BOOL);
      d.push_back(char(bool(arr)));
    }
    else if (arr.is<double>() )
    {
      double v = arr;
      d.push_back(AMF_NUMBER);
      const char *pv = reinterpret_cast<const char*>(&v);
      d.push_back(pv[7]);
      d.push_back(pv[6]);
      d.push_back(pv[5]);
      d.push_back(pv[4]);
      d.push_back(pv[3]);
      d.push_back(pv[2]);
      d.push_back(pv[1]);
      d.push_back(pv[0]);
    }
    else if ( arr.is<std::string>() )
    {
      std::string str = arr;
      d.push_back(AMF_STRING);
      unsigned short string_size = static_cast<unsigned short>(str.size());
      d.push_back(reinterpret_cast<char*>(&string_size)[1]);
      d.push_back(reinterpret_cast<char*>(&string_size)[0]);
      std::copy( str.begin(), str.end(), std::back_inserter(d) );
    }
    if (!arr.get_map<int>().empty())
    {
    }
    if (!arr.get_map< std::string >().empty())
    {
      d.push_back(AMF_HASH);
      std::for_each(
        arr.get_map< std::string >().begin(),
        arr.get_map< std::string >().end(),
        f_ser_string(this, &d)
      );

      d.push_back(0);
      d.push_back(0);
      d.push_back(AMF_END);
    }

    /*else if ( arr.is<std::string>() )
    {

    }
    */
    /*
    const char* beg = d;
    const char* end = d + s;
    unsigned short string_size = 0;
    //while ( beg<end )
    {
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
        reinterpret_cast<char*>(&result)[7] = *(beg++);
        reinterpret_cast<char*>(&result)[6] = *(beg++);
        reinterpret_cast<char*>(&result)[5] = *(beg++);
        reinterpret_cast<char*>(&result)[4] = *(beg++);	
        reinterpret_cast<char*>(&result)[3] = *(beg++);
        reinterpret_cast<char*>(&result)[2] = *(beg++);
        reinterpret_cast<char*>(&result)[1] = *(beg++);
        reinterpret_cast<char*>(&result)[0] = *(beg++); 
        arr = result;
        return beg - d;
      }
      break;
      case AMF_STRING: 
        reinterpret_cast<char*>(&string_size)[1] = *(beg++);
        reinterpret_cast<char*>(&string_size)[0] = *(beg++);
        arr = std::string(beg, beg + string_size);
        beg+=string_size;
        return beg - d;
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
      }
    }*/
  }
};

static const unsigned char invoke_head[]=
{
  0x03,
  0x00, 0x00, 0x00,
  0x00, 0x00, 0x00,
  0x14,
  0x00, 0x00, 0x00, 0x00
}; 

class invoke_serializer
  : serializer
{

public:
  typedef serializer::data_type data_type;

  void serialize(ada::invoke_info& arr, data_type& d)
  {
    d.assign(invoke_head, invoke_head + sizeof(invoke_head) );
    ada::amf_head_1* h1 = reinterpret_cast<ada::amf_head_1*>(&d[0]);
    h1->object_id(arr.get_object_id());
    serializer::serialize_string(arr.get_name(), d);
    serializer::serialize_number(arr.get_call_id(), d);
    d.push_back(AMF_NIL);
    std::for_each(arr.get_map<int>().begin(), arr.get_map<int>().end(), serializer::f_ser_int(this, &d)); 
    ada::amf_head_8* h8 = reinterpret_cast<ada::amf_head_8*>(&d[0]);
    h8->body_size( d.size() - 12);
  }
};

}}}}

#endif
