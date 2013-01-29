//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_ADV_HTTP_AD_WRITER_HPP
#define FAS_ADV_HTTP_AD_WRITER_HPP

#include <fas/misc/http.hpp>
#include <fas/adv/http/tags.hpp>


namespace fas{ namespace adv{ namespace http{

namespace amh = ::fas::misc::http;

class ad_writer
{
  typedef std::vector<char> data_type;
  data_type _buffer;
public:

  size_t capacity() const { return _buffer.capacity() ; }

  template<typename T>
  void activate(T& )
  {
    // _buffer.clear();
    data_type().swap(_buffer);
  }

  template<typename T>
  void operator ()(T& , const char *d, size_t s)
  {
    if (s!=0)
    {
      _buffer.reserve(s);
      std::copy(d, d + s, std::back_inserter(_buffer));
    }
  }

  template<typename T>
  void flush(T& t)
  {
    if (!_buffer.empty())
      t.get_aspect().template get<_output_>()(t, &(_buffer[0]), _buffer.size());
    data_type().swap(_buffer);

    // _buffer.clear();
  }
};

}}}


#endif
