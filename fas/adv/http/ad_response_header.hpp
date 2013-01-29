//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_ADV_HTTP_RESPONSE_HEADER_HPP
#define FAS_ADV_HTTP_RESPONSE_HEADER_HPP

#include <fas/misc/http.hpp>
#include <fas/adv/http/tags.hpp>

namespace fas{ namespace adv{ namespace http{

namespace amh = ::fas::misc::http;

class ad_response_header
{
  amh::initial_line _initial_line;
  amh::header _header;
public:

  size_t capacity() const { return _initial_line.capacity() + _header.capacity() ; }

  const amh::initial_line& initial_line() const { return _initial_line; }
  amh::initial_line& initial_line() { return _initial_line; }


  const amh::header& header() const { return _header;}
  amh::header& header() { return _header;}

  bool is_complete() const { return _header.is_complete();}
  void complete() { _header.complete();}

  void set_content_length(int v) { _header.set_content_length(v); }
  void set_chunked() { _header.set_chunked();}

  int get_content_length() const { return _header.get_content_length();}
  bool is_chunked() const { return _header.is_chunked(); }

  template<typename T>
  void clear(T&)
  {
/*    _initial_line.clear();
    _header.clear();
*/

    _initial_line.clear();
    if (_initial_line.capacity() > 256 )
      amh::initial_line().swap(_initial_line);
    _header.clear();
    if (_header.capacity() > 1024 )
      amh::header().swap(_header);



  }

  template<typename I>
  void copyto(I itr)
  {
    _header.complete();
    std::copy(_initial_line.begin(), _initial_line.end(), itr);
    std::copy(_header.begin(), _header.end(), itr );
  }

  template<typename T>
  bool operator ()(T& t, const char *d, size_t s)
  {
    if (_initial_line.empty())
      _initial_line.assign(d, d+ s);
    else
      std::copy(d, d+s, std::back_inserter(_header));
    return _header.is_complete();
  }

};

}}}

#endif
