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

class ad_receiver
{
  typedef std::string data_type;

public:
  ad_receiver(){}
  
  template<typename T>
  void operator()(T& t, const char* d, size_t s)
  {
    _data.append(d, d + s);
    std::string::size_type body_offset = _data.find("\r\n\r\n");
    if (body_offset == std::string::npos)
      throw std::logic_error("TODO: head receive");
    body_offset += 4;

    t.get_aspect().template get<_after_receiver_>()(t, _data, body_offset);
    

/*
    std::copy(d, d + s, std::back_inserter(_data) );


    if (_offset == 0)
    {
      data_type::const_iterator itr = find_rnrn( _data.begin(), _data.end() );
      if (itr!=_data.end())
      {
        _offset = std::distance<data_type::const_iterator>(_data.begin(), itr);
        std::string encoding = find_value( _data.begin(), _data.begin() + _offset, "Transfer-encoding");
        _chunked = false;
        // TODO: �������! chunked
        std::string content_length = find_value( _data.begin(), _data.begin() + _offset, "Content-length");
        _content_length = atoi(content_length.c_str());
      }
    }

    if (_data.size() - _offset >= _content_length)
    {

      t.release();
      // t.get_aspect().template get<N>()(t);
    }
*/
    /*
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
    */
    /*std::copy(d, d + s, std::back_inserter(_data));

    if ( _offset == 0  )
    {
    }
    */
    /*
    if (_chunk == ready)
    {
    }
    */
    /*
    t.release();*/
    /*if (_status == ready)
    {
    }*/

  } 

  std::string find_value(data_type::const_iterator beg, data_type::const_iterator end, const char* substr)
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
        beg+=std::distance(substr, cur);
        if (beg==end) return std::string();
        if (*beg!=':') return std::string();
        ++beg;
        while (beg!=end && isspace(*beg)) ++beg;
        if (beg==end) return std::string();
        data_type::const_iterator vend = beg;
        while (vend!=end-1 && *vend!='\r' && *(vend+1)!='\n' ) ++vend;
        return std::string(beg, vend);
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
        while (beg!=end && *beg!=':') ++beg;
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
  data_type::const_iterator find_rnrn(data_type::const_iterator beg, data_type::const_iterator end)
  {
    if ( std::distance(beg, end) < 4 )
      return end;

    const char *tmp = &*beg;
    while ( beg != (end - 3) )
    {
      if ( *(beg+0)=='\r' &&
           *(beg+1)=='\n' &&
           *(beg+2)=='\r' &&
           *(beg+3)=='\n' 
         )
      {
        return beg + 4;
      }
      ++beg;
      tmp = &*beg;
    }
    return end;

  }

private:
  data_type _data;
  size_t _offset;
  size_t _content_length;
//  bool _chunked;
};


}}}}

#endif //FAS_ADV_AMF_SRV_AD_INVOKE_UNSERIALIZER_HPP
