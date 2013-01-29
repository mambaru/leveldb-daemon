//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_ADV_HTTP_NOBODY_WRITER_HPP
#define FAS_ADV_HTTP_NOBODY_WRITER_HPP

#include <vector>
#include <cstdlib>
#include <fas/misc/http.hpp>
#include <fas/adv/http/tags.hpp>

namespace fas{ namespace adv{ namespace http{

namespace amh = ::fas::misc::http;

class ad_nobody_writer
{

public:

  template<typename T>
  void activate(T& )
  {
  }

  template<typename T>
  void operator ()(T& t, const char * /*d*/, size_t /*s*/ )
  {
    //t.get_aspect().template get<_writer_>()(t, d, s);

    
    fas::adv::state<_writer_state_, _state_write_initial_>(t)/*(t, d, s)*/; //! убрать? (t, d, s)
    t.get_aspect().template get<_body_writed_>()(t);
  }

private:

};

}}}


#endif
