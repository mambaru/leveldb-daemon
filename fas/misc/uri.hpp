#ifndef FAS_MISC_URI_HPP
#define FAS_MISC_URI_HPP

#include <fas/misc/detail/fast_json.hpp>

namespace fas{ namespace misc{ 

namespace uri{

class domain
{
  inline static const char* begin(const char* beg, const char*)  { return beg; }
  inline static const char* end(const char* beg, const char* end)  { 
    for (;end-beg!=3 && beg[0]!=':' && beg[1]!='/' && beg[2]!='/';++beg);
    return beg;
  }
};

class source
{
  inline static const char* begin(const char* beg, const char* end)  
  { 
    const char* res = domain::end(beg, end);
    if (res + 3 <= end)
      return res + 3;
    return beg;
  }; 
  
  inline static const char* end(const char* beg, const char* end)  
  { 
  }
};

}

class uri
{
  const char* _beg;
  const char* _end;
  std::vector<char> _data;
public:

  void assign(const char* beg, const char* end)
  {
    _data.assign(beg, end);
    _beg = _data.begin();
    _end = _data.end();
  }

  void attach(const char* beg, const char* end)
  {
    _beg = beg;
    _end = end;
  }
};

}}



#endif
