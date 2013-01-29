#ifndef FAS_MISC_HTTP_AD_URI_HPP
#define FAS_MISC_HTTP_AD_URI_HPP

namespace fas{ namespace http {

template<typename P /* = uri_processor*/ >
struct ad_uri
{
  template<typename T, typename H, typename B>
  void operator()(T& t, H& h, B& b)
  {
    _uri_processor(t, h, b);
  }
private:
  P _uri_processor;
};

}}

#endif
