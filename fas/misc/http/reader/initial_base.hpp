#ifndef FAS_MISC_HTTP_READER_INITIAL_BASE_HPP
#define FAS_MISC_HTTP_READER_INITIAL_BASE_HPP

#include <string>

namespace fas{ namespace http{ namespace reader{

struct initial_base
{
  const char *data_beg;
  const char *data_end;

  typedef std::pair<const char *, const char *> initial_field;
  initial_field first;
  initial_field second;
  initial_field thrid;

  initial_base(): data_beg(0), data_end(0), first(), second(), thrid() {}

  void clear() 
  {
    data_beg = 0;
    data_end = 0;
    first  = initial_field();
    second = initial_field();
    thrid  = initial_field();
  }

  void operator()( const char* beg, const char* end )
  {
    data_beg = beg;
    data_end = end;

    for ( ;beg!=end && *beg==' '; ++beg);
    first.first = beg;
    for ( ;beg!=end && *beg!=' '; ++beg);
    first.second = beg;

    for ( ;beg!=end && *beg==' '; ++beg);
    second.first = beg;
    for ( ;beg!=end && *beg!=' '; ++beg);
    second.second = beg;

    for ( ;beg!=end && *beg==' '; ++beg);
    thrid.first = beg;
    for ( ;beg!=end && ( *beg!=' ' || *beg!='\r'); ++beg);
    thrid.second = beg;
  }

  std::string get_string() const
  {
    if ( data_beg == 0 || data_end == 0 ) return std::string();
    return std::string(data_beg, data_end);
  }

  std::string get_first() const  {  return _get(first); }

  std::string get_second() const {  return _get(second); }

  std::string get_thrid() const  {  return _get(thrid); }

  bool check_first(const char* value) const { return _check(value, first);}
  bool check_second(const char* value) const { return _check(value, second);}
  bool check_thrid(const char* value) const { return _check(value, thrid);}

private:
  std::string _get(const initial_field& f ) const
  {
    if ( f.first > f.second || f.first < data_beg || f.second > data_end ) return std::string();
    return std::string(f.first, f.second);
  }

  bool _check(const char* value, initial_field f ) const
  {
    for (;value!='\0' && f.first!=f.second; ++f.first , ++value )
      if ( *value != *f.first )
        return false;

   return *value=='\0' && f.first==f.second;
  }
};

}}}

#endif
