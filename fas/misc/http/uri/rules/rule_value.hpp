#ifndef FAS_MISC_HTTP_URI_RULES_RULE_VALUE_HPP
#define FAS_MISC_HTTP_URI_RULES_RULE_VALUE_HPP

namespace fas{ namespace http{ namespace uri{ 

/** Полное совпадение */
template<typename V>
struct rule_value: V
{
  bool operator()(const char* beg, const char* end)
  {
    if ( const char *v = V::value() )
    {
      for ( ;beg!=end && *v!='\0'; ++beg, ++v)
      {
	if ( *v != *beg ) return false;
      }
      return beg==end && *v=='\0';
    }
    return false;
  }
};

}}}

#endif
