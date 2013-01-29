//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_ADV_JSON_RPC2_HTTP_CHUNKED_AD_CHUNK_WRITER_HPP
#define FAS_ADV_JSON_RPC2_HTTP_CHUNKED_AD_CHUNK_WRITER_HPP

#include <vector>
#include <cstdlib>
#include <cstdio>
#include <fas/adv/json_rpc2/http_chunked/tags.hpp>

namespace fas{ namespace adv{ namespace json_rpc2{ namespace http_chunked{


class ad_chunk_writer
{
public:

  template<typename T>
  void clear(T& t)
  {
  }

  template<typename T>
  void operator ()(T& t, const char *d, size_t s)
  {
    char ch[32];
    int len = std::sprintf(ch, "%x\r\n", s);
    if (len==-1)
      throw;

    t.get_aspect().template get<_writer_>()(t, ch, len);
    if ( s > 0 )
      t.get_aspect().template get<_writer_>()(t, d, s);

  }

private:

};

}}}}


#endif
