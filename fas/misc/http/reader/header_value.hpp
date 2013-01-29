#ifndef FAS_MISC_HTTP_READER_HEADER_VALUE_HPP
#define FAS_MISC_HTTP_READER_HEADER_VALUE_HPP

#include <string>
namespace fas{ namespace http{ namespace reader{

struct header_value
{
private:
  const char *data_beg;
  const char *data_end;

public:

  header_value(): data_beg(), data_end() {};

  void clear() { data_beg=0; data_end=0; }

  bool has_value() const { return data_beg!=0 && data_end!=0;}

  void operator()( const char* beg, const char* end )
  {
    data_beg = beg;
    data_end = end;
  }

  std::string get_string() const
  {
    if ( data_beg == 0 || data_end == 0 ) return std::string();
    return std::string(data_beg, data_end);
  }

  const char *begin() const { return data_beg;};
  const char *end() const { return data_end;};
};


}}}

#endif
