//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_ADV_HTTP_HEADER_READER_HPP
#define FAS_ADV_HTTP_HEADER_READER_HPP

#include <vector>
#include <fas/misc/http.hpp>
#include <fas/adv/http/tags.hpp>

namespace fas{ namespace adv{ namespace http{

namespace amh = ::fas::misc::http;

class ad_header_reader
{
  typedef std::vector<char> data_type;
  data_type buffer;

  const char* _rn(const char* beg, const char* end)
  {
    if (beg==end) return 0;
    if (*beg=='\n') return beg+1;
    for (;beg!=end && *beg!='\r';++beg);
    if (beg==end) return 0;
    ++beg;
    if (beg==end || *beg!='\n') return 0;
    return beg+1;
  }

public:

  size_t capacity() const { return buffer.capacity(); }

  template<typename T>
  void activate(T& t)
  {
    // buffer.clear();
    data_type().swap(buffer);
    t.get_aspect().template get<_request_header_>().clear(t);
  }

  template<typename T>
  void operator ()(T& t, const char *d, size_t s)
  {
    if (s == 0) return;

    const char* e = d + s;
    const char* n = _rn(d, e);
    if (n==0) 
    {
      if (buffer.empty()) buffer.assign(d, e);
      else std::copy(d, e, std::back_inserter(buffer));
    }
    else
    {
      if (!buffer.empty())
        std::copy(d, n, std::back_inserter(buffer));

      bool iscomplete = false;
      if (!buffer.empty())
        iscomplete = t.get_aspect().template get<_request_header_>()(t, &(buffer[0]), buffer.size());
      else
        iscomplete = t.get_aspect().template get<_request_header_>()(t, d, std::distance(d, n) );
      buffer.clear();

      for (;n!=0 && !iscomplete;)
      {
        d = n;
        n = _rn(d, e);
        if ( n==0 )
          std::copy(d, e, std::back_inserter(buffer));
        else
          iscomplete = t.get_aspect().template get<_request_header_>()(t, d, std::distance(d, n) );
      }

      if (iscomplete)
      {

        const amh::header& h = t.get_aspect().template get<_request_header_>().header();
        bool ischunked = h.is_chunked();
        int content_length = h.get_content_length();
        t.get_aspect().template get<_head_readed_>()(t);
        size_t tail = std::distance(n, e);
        if (ischunked)
        {
            t.get_aspect().template get<_reader_state_>()
                          .template state<_state_read_chunk_>(t)(t, n, tail );
        }
        else if (content_length > 0)
        {
            t.get_aspect().template get<_reader_state_>()
                          .template state<_state_read_body_>(t)(t, n, tail );
        }
        else
        {
            t.get_aspect().template get<_reader_state_>()
                          .template state<_state_read_nobody_>(t)(t, n, tail );
        }
      }
    }
  }
};

}}}


#endif
