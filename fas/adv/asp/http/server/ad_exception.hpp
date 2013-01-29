//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2010
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_ADV_ASP_HTTP_SERVER_AD_EXCEPTION_HPP
#define FAS_ADV_ASP_HTTP_SERVER_AD_EXCEPTION_HPP

#include <fas/misc/http/exceptions.hpp>

#include <sstream>

namespace fas{ namespace adv{ namespace asp{ namespace http{ namespace server{

namespace ah = ::fas::http;
namespace ap = ::fas::pattern;

struct ad_exception
{
public:
  template<typename T>
  void operator()(T& t, const ah::http_exception& )
  {
    try
    {
       throw;
    }
    catch( const ah::informational_exception& e)
    {
      _process( t, _informational_exception_(), e);
    }
    catch( const ah::success_exception& e)
    {
      _process( t, _success_exception_(), e);
    }
    catch( const ah::redirection_exception& e)
    {
      _process( t, _redirection_exception_(), e);
    }
    catch( const ah::client_error& e)
    {
      _process( t, _client_error_(), e);
    }
    catch( const ah::server_error& e)
    {
      _process( t, _server_error_(), e);
    }
  }
private:
  template<typename T, typename Tg, typename E>
  void _process(T& t, Tg, E e) 
  {
    _process2(t, t.get_aspect(). template get<Tg>(), e );
  }

  template<typename T, typename D, typename E>
  void _process2(T& t, D& adv, E e) 
  {
    adv( t, e);
  }

  template<typename T, typename E>
  void _process2(T& t, ap::empty_type, E e)
  {
    _process3(t, t.get_aspect(). template get<_http_error_>(), e );
  }

  template<typename T, typename D, typename E>
  void _process3(T& t, D& adv, E e) 
  {
    adv( t, e);
  }

  template<typename T, typename E>
  void _process3(T& t, ap::empty_type, E e)
  {
    t.get_aspect(). template get<_writer_>().clear(t);
    t.get_aspect(). template get<_writer_>().headers().clear();
    t.get_aspect(). template get<_writer_>().headers().initial().set_status(e);

    std::stringstream ss;

    ss << "<HTML><HEAD><TITLE>"
       << e.what() 
       << "</HEAD><BODY BGCOLOR='#FFFFFF' TEXT='#000000'><H1>"
       << e.what()
       << "</H1><H2>Error "
       << e.get_code()
       << "</H2></BODY></HTML>";
   std::string str = ss.str();

   t.get_aspect(). template get<_writer_>()(t, str.c_str(), str.size() );

// #warning !!!!!!!!!!!!!!!
//   abort();
/*
HTTP/1.1 400 Bad Request
Content-Type: text/html; charset=UTF-8
Content-Length: 145
Date: Mon, 24 May 2010 11:50:27 GMT
Expires: Mon, 24 May 2010 11:50:27 GMT
Cache-Control: private, max-age=0
X-Content-Type-Options: nosniff
X-XSS-Protection: 1; mode=block
Server: GSE

<HTML>
<HEAD>
<TITLE>Bad Request</TITLE>
</HEAD>
<BODY BGCOLOR="#FFFFFF" TEXT="#000000">
<H1>Bad Request</H1>
<H2>Error 400</H2>
</BODY>
</HTML>
*/
  }
};

}}}}}

#endif
