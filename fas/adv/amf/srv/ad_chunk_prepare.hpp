//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_UTIL_AMF_AD_CHUNK_PREPARE_HPP
#define FAS_UTIL_AMF_AD_CHUNK_PREPARE_HPP

#include <fas/adv/amf/srv/tags.hpp>
#include <fas/adv/amf/types.hpp>


namespace fas{ namespace  adv{ namespace amf{ namespace srv{

namespace ada = ::fas::adv::amf;


class ad_chunk_prepare
{
public:

  template<typename T>
  void operator() (T& t, const char* d, size_t s)
  {
    const char* beg = d;
    const char* end = d + s;
    const ada::amf_head_8* h8 = reinterpret_cast<const ada::amf_head_8*>(d);
    size_t head_size = h8->head_size();
    size_t body_size = h8->body_size();

    if (body_size <= CHUNK_SIZE)
    {
      std::cerr.write(d, std::streamsize(s) );
      t.get_aspect().template get<_writer_>()(t, d,
        static_cast<typename T::write_size_type>(s)
      );
    }
    else
    {
      std::vector<char> data;
      while (beg<end)
      {
        if (std::distance(beg, end) > CHUNK_SIZE )
        {
          std::copy(beg, beg + head_size + CHUNK_SIZE, std::back_inserter(data));
          beg += head_size + CHUNK_SIZE;
          data.push_back(*beg++);
        }
        else
        {
          std::copy(beg, end, std::back_inserter(data));
          beg = end;
        }
      }
      t.get_aspect().template get<_writer_>()(
        t, &data[0], 
        static_cast<typename T::write_size_type>(data.size()) );

    }



    /*
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
    */
  }
};

}}}}

#endif
