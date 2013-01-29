#ifndef FAS_MISC_JSON_HPP
#define FAS_MISC_JSON_HPP

#include <fas/misc/variant.hpp>
#include <stdio.h>

namespace fas{ namespace misc{ namespace json{

/*namespace aa = ::fas::aop;
namespace ap = ::fas::pattern*/;
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

typedef amsc::variant_map<variant_aspect> json;

class json_rpc
{
public:
  void set_name(const std::string& name) { _json_rpc["name"] = name;}
  const std::string& get_name() const 
  { 
    const std::string& name = _json_rpc["name"];
    return name;
    // return _json_rpc["name"];
  }

  void set_object_id(const std::string& oid) { _json_rpc["object_id"] = oid;}
  const std::string& get_object_id() const 
  { 
    const std::string& object_id = _json_rpc["object_id"];
    return object_id;
  }

  void set_call_id(const std::string& cid) { _json_rpc["call_id"] = cid;}
  const std::string& get_call_id() const 
  {
    const std::string& call_id= _json_rpc["call_id"];
    return call_id;
  }

  json& params() { return _json_rpc["params"];}
  const json& params() const { return _json_rpc["params"];}

  json& result() { return _json_rpc["result"];}
  const json& result() const { return _json_rpc["result"];}

  json& status() { return _json_rpc["result"];}
  const json& status() const { return _json_rpc["result"];}

  json::mapped<int>::type& 
  operator[] (int key)
  {
    return _json_rpc["params"][key];
  }

  const json::mapped<int>::type& 
  operator[] (int key) const
  {
    return _json_rpc["params"][key];
  }


  void serialize(std::string& result) const
  {
    std::back_insert_iterator< std::string > itr = std::back_inserter(result);
    _serialize(_json_rpc, itr);
  }

  template<typename I>
  static void _serialize(const json& j, I itr)
  {
    if (j.has_map<std::string>())
      _serialize_object(j.get_map<std::string>().begin(), j.get_map<std::string>().end(), itr);
    else if (j.has_map<int>())
      _serialize_array(j.get_map<int>().begin(), j.get_map<int>().end(), itr);
    else if ( j.is<std::string>() )
      _serialize_string(j, itr);
    else if ( j.is<int>() )
      _serialize_int(j, itr);
    else if ( j.is<bool>() )
      _serialize_bool(j, itr);
    else if ( j.is<double>() )
      _serialize_double(j, itr);
    else
    {
      *(itr++)='n';*(itr++)='u';*(itr++)='l';*(itr++)='l';
    }
  }

  template<typename ItrMap, typename I>
  static void _serialize_object(ItrMap beg, ItrMap end, I itr)
  {
    *(itr++) = '{';
    for (;beg!=end;)
    {
      //std::copy(beg->first.begin(), beg->first.end(), itr);
      _serialize_string(beg->first, itr);
      *(itr++) = ':';
      _serialize(beg->second, itr);
      if (++beg == end)
        break;
      *(itr++) = ',';
    }
    *(itr++) = '}';
  }

  template<typename ItrMap, typename I>
  static void _serialize_array(ItrMap beg, ItrMap end, I itr)
  {
    *(itr++) = '[';
    for ( int i = 0; beg!=end; )
    {
      while (i < beg->first)
      {
        _serialize_null(itr);
        *(itr++) = ',';
        ++i;
      }
      _serialize(beg->second, itr);
      if (++beg == end)
        break;
      *(itr++) = ',';
      ++i;
    }
    
    *(itr++) = ']';

  }

  template<typename I>
  static void _serialize_string(const std::string& j, I itr)
  {
    *(itr++) = '\"';
    std::string::const_iterator beg = j.begin();
    std::string::const_iterator end = j.end();
    for (;beg!=end; ++beg)
      *(itr++) = *beg;
    *(itr++) = '\"';
  }

  template<typename I>
  static void _serialize_int(int j, I itr)
  {
    char tmp[12];
    char *beg = tmp;
    char *cur = tmp;
    if (j==0)
      (*cur++) = '0';
    else
    {
      if (j<0)
      {
        *(cur++) = '-';
        j=-j;
      }
      for ( ; j!=0; j/=10 )
        *(cur++) = j%10 + '0';
    }

    if (beg[0]=='-') { ++beg; *(itr++)='-';}
    std::reverse_copy(beg, cur, itr);
    //std::reverse(beg.base(), itr.base());
    //std::itoa(
  }

  template<typename I>
  static void _serialize_double(double j, I itr)
  {
    char ch[32];
    size_t s = _snprintf(ch, 32, "%e", j);
    std::copy(ch, ch+s, itr);
  }

  template<typename I>
  static void _serialize_bool(bool j, I itr)
  {
    if (j) { *(itr++)='t'; *(itr++)='r';*(itr++)='u';*(itr++)='e';}
    else { *(itr++)='f'; *(itr++)='a';*(itr++)='l';*(itr++)='s';*(itr++)='e';}
  }

    template<typename I>
  static void _serialize_null(I itr)
  {
     *(itr++)='n'; *(itr++)='u';*(itr++)='l';*(itr++)='l';
    
  }

  void unserialize(const std::string& result)
  {
    std::string::const_iterator beg = result.begin();
    std::string::const_iterator end = result.end();
    _skip_space(beg,end);
    _unserialize(_json_rpc, beg,end);
  }

  template<typename T>
  static void _skip_space(T& beg, T end)
  {
    while (beg!=end && 
      (*beg==' ' || *beg=='\r' || *beg=='\n' || *beg=='\t') ) ++beg;
  }

  template<typename T>
  static void _unserialize(json& j, T& beg, T end)
  {
    if (*beg=='{')
      _unserialize_object(j, beg, end);
    else if (*beg=='[')
      _unserialize_array(j, beg, end);
    else if (*beg=='"')
      _unserialize_string(j, beg, end);
    else if (*beg=='t')
    {
      j=true;
      beg+=4;
    }
    else if (*beg=='f')
    {
      j=false;
      beg+=5;
    }
    else if (*beg=='n')
    {
      j.set_null();
      beg+=4;
    }
    else
      _unserialize_number(j, beg, end);
  }

  template<typename T>
  static void _unserialize_object(json& j, T& beg, T end)
  {
    if (*beg!='{')
      throw;
    ++beg;
    _skip_space(beg,end);
    if (*beg!='"')
      throw;
    
    while (beg!=end)
    {
      std::string name;
      _unserialize_string(name, beg, end);
      _skip_space(beg,end);
      if (*beg!=':')
        throw;
      _skip_space(++beg,end);
      _unserialize(j[name], beg,end); 
      _skip_space(beg,end);
      if (*beg!=',' && *beg!='}')
        throw;
      if (*(beg++)=='}')
        break;
    }
  }

  template<typename T>
  static void _unserialize_array(json& j, T& beg, T end)
  {
    if (*beg!='[')
      throw;
    ++beg;
    for (int i = 0; beg!=end; ++i)
    {
      _skip_space(beg,end);
      _unserialize(j[i], beg, end);
      std::string tmp;
      _serialize(j, std::back_inserter(tmp));
      _skip_space(beg,end);
      if (*beg!=',' && *beg!=']')
        throw;
      if (*(beg++)==']')
        break;
    }
  }

  template<typename S, typename T>
  static void _unserialize_string(S& s, T& beg, T end)
  {
    if (*beg!='"')
        throw;

    T cur = ++beg;
    while (beg!=end)
    {
      if (*beg=='"')
        break;
      ++beg;
    }
    s = std::string(cur, beg);
    if (*beg!='"')
        throw;
    ++beg;
  }

  template<typename T>
  static void _unserialize_number(json& s, T& beg, T end)
  {
    char ch[30]={0};//!!!
    T cur = beg;
    while ( beg!=end 
      && ((*beg>='0' && *beg<='9') 
      || *beg=='.' 
      || *beg=='e' 
      || *beg=='E' 
      || *beg=='+' 
      || *beg=='-'))
    {
      ++beg;
    }
    //while ( beg!=end && ::isalnum(*beg)) ++beg;
    std::copy(cur, beg, ch);
    double value = ::atof(ch);
    if ( value - long(value) != 0  )
      s = ::atof(ch);
    else
      s = int(value);
    /*T cur = beg;
    while (*beg>='0' && *beg<='9' *beg=='.' *beg=='e' *beg=='E' *beg=='+' *beg=='+')
    */
    
  }



private:
  json _json_rpc;  
};

}}}

#endif
