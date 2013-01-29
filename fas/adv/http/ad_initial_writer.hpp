//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_ADV_HTTP_AD_INITIAL_WRITER_HPP
#define FAS_ADV_HTTP_AD_INITIAL_WRITER_HPP

#include <vector>
#include <fas/misc/http.hpp>
#include <fas/adv/http/tags.hpp>

namespace fas{ namespace adv{ namespace http{

namespace amh = ::fas::misc::http;

class ad_initial_writer
{
public:

  template<typename T>
  void activate(T& t)
  {
    t.get_aspect().template get<_writer_>().flush(t);
  }

  template<typename T>
  void operator ()(T& t, const char *d, size_t s)
  {
    
    // if (s == 0) return;
    t.get_aspect().template get<_writer_state_>().template state<_state_write_header_>(t)(t, d, s);
  }

  template<typename T>
  void operator ()(T& )
  {
    
    // t.get_aspect().template get<_writer_state_>().template state<_state_write_header_>(t);
    // t.get_aspect().template get<_writer_state_>().template state<_state_write_header_>(t)(t, d, s);
  }
};

}}}


#endif
