#ifndef FAS_MISC_HTTP_WRITER_AD_CHUNKED_WRITER_HPP
#define FAS_MISC_HTTP_WRITER_AD_CHUNKED_WRITER_HPP

#include "../names.hpp"
#include <cstdio>

namespace fas{ namespace http{ namespace writer{

class ad_chunked_writer
{
  bool _complete;
public:

  ad_chunked_writer(): _complete(false) {}
  operator bool () const { return _complete; }

  template<typename T, typename H, typename Itr>
  Itr operator() ( T& , H& h, const char *d, size_t s, Itr itr )
  {
    _complete = false;
    if ( !h )
    {
      h.template clear_header<Content_Length>();
      h.template set_header<Transfer_Encoding>(true);
    }

    char ch[32]={0};
    int len = std::sprintf(ch, "%x\r\n", static_cast<unsigned int>(s) );
    if (len!=-1)
      itr = std::copy(ch, ch + len, itr);
    if ( s != 0 )
       itr = std::copy(d, d + s, itr);
    else
       _complete = true;
    *(itr++) = '\r';
    *(itr++) = '\n';
    return itr;
  }
};

}}}

#endif
