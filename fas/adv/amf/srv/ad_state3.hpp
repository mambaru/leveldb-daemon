//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_ADV_AMF_SRV_STATE3_HPP
#define FAS_ADV_AMF_SRV_STATE3_HPP

#include <vector>
#include <iostream>

#include <fas/adv/amf/srv/tags.hpp>
//#include <fas/amf/types.hpp>

namespace fas{ namespace  adv{ namespace amf{ namespace srv{

class ad_state3
{
  typedef std::vector<char> data_type;
public:

  template<typename T>
  void activate(T&){}

  template<typename T>
  void operator()(T& t, const char* d, size_t s)
  {
    if ( s==0 )
      return;
    t.get_aspect().template get<_chunk_splitter_>()(t, d, s);
  } 
private:

  data_type _handshake;
};


}}}}

#endif //TR_AMF_SPLITTER_H
