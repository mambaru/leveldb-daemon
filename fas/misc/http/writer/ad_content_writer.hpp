#ifndef FAS_MISC_HTTP_WRITER_AD_CONTENT_WRITER_HPP
#define FAS_MISC_HTTP_WRITER_AD_CONTENT_WRITER_HPP

#include "../names.hpp"

namespace fas{ namespace http{ namespace writer{

class ad_content_writer
{
public:

  operator bool () const { return true; }

  template<typename T, typename H, typename Itr>
  Itr operator() ( T& , H& h, const char *d, size_t s, Itr itr )
  {
    if ( !h )
      h.template set_header<Content_Length>(s);
    std::copy(d, d + s, itr);
    return itr;
  }
};

}}}

#endif
