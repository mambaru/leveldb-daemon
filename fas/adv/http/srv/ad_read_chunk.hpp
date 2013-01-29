//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_ADV_HTTP_SRV_AD_RECIVER_HPP
#define FAS_ADV_HTTP_SRV_AD_RECIVER_HPP

#include <vector>
#include <iostream>

#include <fas/adv/http/tags.hpp>

namespace fas{ namespace  adv{ namespace http{ namespace srv{

namespace adh = ::fas::adv::http;


template<typename N>
class ad_receiver
{
  typedef std::vector<char> data_type;
  typedef enum { ready, read_head, head_ready, read_body, body_ready} state;
public:
  ad_receiver(){}
  
  template<typename T>
  void operator()(T& t, const char* d, size_t s)
  {
    if ( _offset == 0  )
    {
      if ( _data.empty() )
      {
        if ( char *beg = find_rnrn(d, d + s) )
        {
          _offset = std::distance(d, beg);
        }
      }
    }
  } 

  const char* find_value(const char* beg, const char* end, const char* substr, size_t s)
  {
    for (;beg!=end; ++beg)
    {
      const char* cur = substr;
      for (size_t i=0; cur!='\0'; ++i, ++cur)
      {
        if (beg+i==end)
          return 0;

        if ( toupper(beg[i]) != toupper(*cur) )
          break;
      }

      if ( *cur=='\0' )
      {
        beg+=std::distance(substr, beg);
        if (beg==end) return 0;
        if (*beg!=':') return 0;
        ++beg;
        while (beg!=end && isspace(*beg)) ++beg;
        // if (beg==end) return 0;
      }
    }
    return 0;
  }

  int content_length(const char* beg, const char* end)
  {
    // content-length - 14
    if (std::distance(beg, end) < 14)
      return -1;
    while (beg!=end-14)
    {
      if ( 
           ( beg[0]=='c'  || beg[0]=='C')
        && ( beg[1]=='o'  || beg[1]=='O')
        && ( beg[2]=='n'  || beg[2]=='N')
        && ( beg[3]=='t'  || beg[3]=='T')
        && ( beg[4]=='e'  || beg[4]=='E')
        && ( beg[5]=='n'  || beg[5]=='N')
        && ( beg[6]=='t'  || beg[6]=='T')
        && ( beg[7]=='-'  || beg[7]=='-')
        && ( beg[8]=='l'  || beg[8]=='L')
        && ( beg[9]=='e'  || beg[9]=='E')
        && ( beg[10]=='n' || beg[10]=='N')
        && ( beg[11]=='g' || beg[11]=='G')
        && ( beg[12]=='t' || beg[12]=='T')
        && ( beg[13]=='h' || beg[13]=='H')
      )
      {
        beg+=14;
        while (beg!=end && beg!=':') ++beg;
        if (beg==end) return -1;
        ++beg;
        if (beg==end) return -1;
        while (beg!=end && isspace(*beg) ) ++beg;
        return std::atoi(beg);
      }
    }
    return -1;
    
  }

  // after rnrn
  const char* find_rnrn(const char* beg, const char* end)
  {
    if ( std::distance(beg, end) < 4 )
      return 0;

    while ( beg != end - 4 )
    {
      if ( *(beg+0)=='\r' &&
           *(beg+1)=='\n' &&
           *(beg+2)=='\r' &&
           *(beg+3)=='\n' &&
         )
      {
        return beg + 4;
      }
      ++beg;
    }
    return 0;

  }

private:
  data_type _head;
  data_type _body;
  size_t _body_offset;
  size_t _content_length;
  bool _chunk;
  size_t chunk_size;
  state _status;

};


}}}}

#endif //FAS_ADV_AMF_SRV_AD_INVOKE_UNSERIALIZER_HPP
