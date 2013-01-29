#ifndef FAS_ADV_AMF_ARRAY_HPP
#define FAS_ADV_AMF_ARRAY_HPP

#include <fas/misc/variant.hpp>
namespace fas{ namespace adv{ namespace amf{  

namespace aa = ::fas::aop;
namespace ap = ::fas::pattern;
namespace amsc = ::fas::misc;

struct bool_advice: aa::advice< ap::type2type<bool>, amsc::vt<bool>  > {};
struct int_advice: aa::advice< ap::type2type<int>, amsc::vt<int>  > {};
struct float_advice: aa::advice< ap::type2type<double>, amsc::vt<double>  > {};
struct string_advice: aa::advice< ap::type2type<std::string>, amsc::vt<std::string>  > {};

typedef ap::type_list_n<
    bool_advice,
    int_advice,
    float_advice,
    string_advice
>::type variant_types;

struct variant_aspect: aa::aspect< variant_types > {};

typedef amsc::variant_map<variant_aspect> array;

class invoke_info
  : public array
{
  typedef array super;
public:
  typedef invoke_info self;
  typedef super::map_aspect aspect;
  
  unsigned char get_object_id() const { return _object_id;}
  double get_call_id() const { return _call_id;}
  std::string get_name() const  { return _name;}

  template<typename T>
  self& operator = (const T& v)
  {
    super_variant::operator =(v);
    return *this;
  }

  self& operator = (const char* v)
  {
    super_variant::operator =( std::string(v) );
    return *this;
  }

  invoke_info()
    : _name()
    , _object_id()
    , _call_id()
  {}

  invoke_info(const std::string& name, unsigned char oid, double cid )
    : _name(name)
    , _object_id(oid)
    , _call_id(cid)
  {
  }


/*
  invoke_info(const std::string name, unsigned char object_id, double call_id)
    : _name(name)
    , _object_id(object_id)
    , _call_id(call_id){}
*/


  void set_name(const std::string& name ) { _name = name;}
  void set_object_id(unsigned char oid)  { _object_id = oid;}
  void set_call_id(double cid)  { _call_id = cid;}
/*
  aa::advice_cast<ap::type2type<int>, aspect>::type::type::mapped_type& operator[] (const int& key)
  {
    typedef aa::advice_cast<ap::type2type<int>, map_aspect>::type holder_type;
    return static_cast<holder_type&>(*this).get_ref()[key];
  }*/
private:
  std::string _name;
  unsigned char _object_id;
  double _call_id;
  
};

class invoke_result
  : public invoke_info
{
public:
  typedef invoke_result self;
  invoke_result(): invoke_info(){}
  invoke_result(unsigned char oid, double cid )
    : invoke_info("_result", oid, cid)
  {
  }

    template<typename T>
  self& operator = (const T& v)
  {
    super_variant::operator =(v);
    return *this;
  }

  self& operator = (const char* v)
  {
    super_variant::operator =( std::string(v) );
    return *this;
  }

};

class call_info
  : public invoke_info
{
public:
  typedef call_info self;
  call_info(): invoke_info(){}

  call_info(const std::string& name, unsigned char oid, double cid = 0 )
    : invoke_info(name, oid, cid)
  {
  }

    template<typename T>
  self& operator = (const T& v)
  {
    super_variant::operator =(v);
    return *this;
  }

  self& operator = (const char* v)
  {
    super_variant::operator =( std::string(v) );
    return *this;
  }

};

class call_result
  : public call_info
{
public:
  typedef call_result self;
  call_result(): call_info(){}
  call_result(unsigned char oid, double cid )
    : call_info("_result", oid, cid)
  {}

    template<typename T>
  self& operator = (const T& v)
  {
    super_variant::operator =(v);
    return *this;
  }

  self& operator = (const char* v)
  {
    super_variant::operator =( std::string(v) );
    return *this;
  }

};


}}}

#endif
