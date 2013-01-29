//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_UTIL_AMF_AD_CHUNK_PREPARE_HPP
#define FAS_UTIL_AMF_AD_CHUNK_PREPARE_HPP

#include <fas/util/amf/tags.hpp>
#include <fas/util/amf/serialize.hpp>

namespace fas{ namespace  util{ namespace amf{


class ad_chunk_prepare
{
public:

  template<typename T>
  void operator() (T& t, const serialize& as)
  {
    const amf_head_1* head1 = as.head();
    if ( head1->head_size() >= 8 )
    {
      const amf_head_8* head8 = static_cast<const amf_head_8*>(head1);
      size_t body_size = head8->body_size();
      if ( body_size > CHUNK_SIZE )
      {
        amf_head_1 chunk_head = *head1;
        chunk_head.head_size(1);
        char ch_chunk_head = *reinterpret_cast<char*>(&chunk_head);

        std::vector<char> chunked;
        chunked.reserve(as.size() + as.size()/CHUNK_SIZE + 1 );
        chunked.assign(as.data(), as.body() + CHUNK_SIZE );
        size_t offset = as.body() - as.data() + CHUNK_SIZE ;
        while ( offset < as.size() )
        {
          chunked.push_back( ch_chunk_head );
          size_t offset_chunk = offset + CHUNK_SIZE;
          if (offset_chunk > as.size() ) 
            offset_chunk = as.size();
          std::copy( as.data() + offset,
                     as.data() + offset_chunk,
                     std::back_inserter(chunked) 
                   );
          offset += CHUNK_SIZE;
        }
        t.get_aspect().template get<_write_>()(t, &chunked[0], chunked.size());
        return;
      };
    }

    t.get_aspect().template get<_write_>()(t, as.data(), as.size());
  }
};

}}}

#endif
