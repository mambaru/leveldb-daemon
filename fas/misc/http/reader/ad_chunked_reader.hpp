#ifndef FAS_MISC_HTTP_READER_AD_CHUNKED_READER_HPP
#define FAS_MISC_HTTP_READER_AD_CHUNKED_READER_HPP

#include "http_parser.hpp"

namespace fas{ namespace http{ namespace reader{

class ad_chunked_reader
{
public:
  template<typename T>
  const char* operator()(T& t, const char* beg, const char* end) 
  {
    const char* chunk = http_parser::parse_space(beg, end);
    if ( chunk != 0 )
    {
       const char *data = http_parser::parse_header(beg, end);
       if ( data != 0 )
       {
          int chunk_size = _chunk_size( chunk, data);
          if ( chunk_size + 2 <= std::distance( data, end ) )
          {
            t._content_ready( data, data + chunk_size, chunk_size == 0 );
            return data + chunk_size + 2;
          }
       }
    }
    t._content_no_ready();
    return 0;
  }

  int _chunk_size(const char* beg, const char* end) const
  {
    char hex[16]={0};
    for ( register size_t i = 0; i < 16 && beg!=end; ++i, ++beg )
      hex[i]= *beg;
    return std::strtol(hex, 0, 16);
  }
};

}}}

#endif
