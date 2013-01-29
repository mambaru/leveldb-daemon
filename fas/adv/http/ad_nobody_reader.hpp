//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_ADV_HTTP_NOBODY_READER_HPP
#define FAS_ADV_HTTP_NOBODY_READER_HPP

#include <fas/misc/http.hpp>
#include <fas/adv/http/tags.hpp>

namespace fas{ namespace adv{ namespace http{

namespace amh = ::fas::misc::http;

class ad_nobody_reader
{
public:
  template<typename T>
  void activate(T& )
  {
  }

  template<typename T>
  void operator ()(T& t, const char *, size_t )
  {
    t.get_aspect().template get<_reader_state_>().template state<_state_read_header_>(t)/*(t, d, s)*/; //! убрать? (t, d, s)
    t.get_aspect().template get<_body_readed_>()(t);
  }
};

}}}


#endif
