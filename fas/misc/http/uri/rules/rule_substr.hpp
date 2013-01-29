#ifndef FAS_MISC_HTTP_URI_RULES_RULE_SUBSTR_HPP
#define FAS_MISC_HTTP_URI_RULES_RULE_SUBSTR_HPP

namespace fas{ namespace http{ namespace uri{ 

/** если значение совпало с первой частю uri */
template<typename V>
struct rule_substr: V
{
  bool operator()(const char* beg, const char* end)
  {
    if ( const char *v = V::value() )
    {
      for ( ;beg!=end && *v!='\0'; ++beg, ++v)
      {
	if ( *v != *beg ) 
	  return false;
      }
      return *v=='\0';
    }
    return false;
  }
};

}}}

#endif
