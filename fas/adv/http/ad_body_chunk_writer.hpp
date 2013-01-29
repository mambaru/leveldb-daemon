//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_ADV_HTTP_BODY_CHUNK_WRITER_HPP
#define FAS_ADV_HTTP_BODY_CHUNK_WRITER_HPP

#include <vector>
#include <cstdlib>
#include <cstdio>
#include <fas/misc/http.hpp>
#include <fas/adv/http/tags.hpp>

namespace fas{ namespace adv{ namespace http{

namespace amh = ::fas::misc::http;

class ad_body_chunk_writer
{
  bool _keep_alive;
public:

  template<typename T>
  void activate(T& )
  {
    
    //_header_writed = false;
    //_buffer.clear();
    // _keep_alive = t.get_aspect().template get<_request_header_>().is_keep_alive();

  }

  template<typename T>
  void operator ()(T& t, const char *d, size_t s)
  {
    
    char ch[32];
    int len = std::sprintf(ch, "%x\r\n", static_cast<unsigned int>(s) );
    if (len==-1)
      throw;

    t.get_aspect().template get<_writer_>()(t, ch, len);
    if ( s > 0 )
    {
      t.get_aspect().template get<_writer_>()(t, d, s);
      t.get_aspect().template get<_writer_>()(t, "\r\n", 2);
      t.get_aspect().template get<_writer_>().flush(t);
    }

    if (s == 0)
    {
      t.get_aspect().template get<_writer_>()(t, "\r\n", 2);
      t.get_aspect().template get<_writer_>().flush(t);
      t.get_aspect().template get<_writer_state_>().template state<_state_write_initial_>(t);
      t.get_aspect().template get<_body_writed_>()(t);
    }
  }

private:

};

}}}


#endif
