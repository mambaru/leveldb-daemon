//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_ADV_AMF_SRV_STATE2_HPP
#define FAS_ADV_AMF_SRV_STATE2_HPP

#include <vector>

#include <fas/adv/amf/srv/tags.hpp>
//#include <fas/amf/types.hpp>

namespace fas{ namespace  adv{ namespace amf{ namespace srv{

  namespace adas = ::fas::adv::amf;

class ad_state2
{
  typedef std::vector<char> data_type;
public:

  template<typename T>
  void activate(T&)
  {
    _count = 0;
    _head_size = 1;
  }

  template<typename T>
  void operator()(T& t, const char* d, size_t s)
  {
    if (_count == 0 && s > 0)
    {
     // const adas::amf_head_1* h1 = reinterpret_cast<const adas::amf_head_1*>(d);
      _head_size = 0; //h1->head_size();
    }

    size_t need = 1536 + _head_size - _count;
    if ( s >= need )
    {
      if (s >= need)
        t.get_aspect().template get<_state_>().template state<_state3_>(t)(t, d + need, s - need);
      else
        t.get_aspect().template get<_state_>().template state<_state3_>(t);
    }
    else
      _count += s;

  } 
private:

  size_t _head_size;
  size_t _count;
};


}}}}

#endif //TR_AMF_SPLITTER_H
