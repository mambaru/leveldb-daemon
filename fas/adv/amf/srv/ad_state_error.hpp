//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_ADV_AMF_SRV_STATE_ERROR_HPP
#define FAS_ADV_AMF_SRV_STATE_ERROR_HPP

#include <vector>

#include <fas/adv/amf/srv/tags.hpp>
//#include <fas/amf/types.hpp>

namespace fas{ namespace  adv{ namespace amf{ namespace srv{

class ad_state_error
{
  typedef std::vector<char> data_type;
public:

  template<typename T>
  void activate(T&)
  {
    // ������� ����������
  }

  template<typename T>
  void operator()(T& t, const char* d, size_t s)
  {
    if ( s==0 )
      return;

  } 
private:

  data_type _handshake;
};


}}}}

#endif //TR_AMF_SPLITTER_H
