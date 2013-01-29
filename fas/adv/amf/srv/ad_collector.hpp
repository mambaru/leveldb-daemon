//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_ADV_AMF_SRV_AD_COLLECTOR_HPP
#define FAS_ADV_AMF_SRV_AD_COLLECTOR_HPP

#include <vector>
#include <algorithm>

#include <fas/adv/amf/srv/tags.hpp>
//#include <fas/amf/types.hpp>

namespace fas{ namespace  adv{ namespace amf{ namespace srv{

namespace ada = ::fas::adv::amf;

template<typename N>
class ad_collector
{
  typedef std::vector<char> data_type;
  typedef std::vector<data_type> data_object;
public:

  template<typename T>
  void operator()(T& t, const char* d, size_t s)
  { 
    const ada::amf_head_8* h8 = reinterpret_cast<const ada::amf_head_8*>(d);
    unsigned char oid = h8->object_id();
    size_t body_size = h8->body_size();
    size_t head_size = h8->head_size();
    if (_objects.size() <= oid)
      _objects.resize(oid + 1);

    if ( _objects[oid].empty() )
      _objects[oid].assign(d, d + s);
    else
    {
      h8 = reinterpret_cast<const ada::amf_head_8*>(&(_objects[oid][0]));
      body_size = h8->body_size();
      std::copy( d + head_size, d + s , std::back_inserter(_objects[oid]));
    }

    if ( body_size + head_size == _objects[oid].size() )
    {
      t.get_aspect().template get<N>()(t, &(_objects[oid][0]), _objects[oid].size() );
      if (_objects[oid].size() > 4096)
        data_type().swap(_objects[oid]);
      else
        _objects[oid].clear();
    }
    else if ( body_size + head_size < _objects[oid].size() )
      throw;

    return;
  } 

  template<typename T>
  void clear(T&)
  {
    _objects.clear();
  }

private:
  data_object _objects;
};


}}}}

#endif //TR_AMF_SPLITTER_H
