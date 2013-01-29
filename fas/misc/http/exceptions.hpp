#ifndef FAS_MISC_HTTP_EXCEPTIONS_HPP
#define FAS_MISC_HTTP_EXCEPTIONS_HPP

#include <stdexcept>
#include <fas/pattern/conversion.hpp>
/** Классы имен полей являются таже их тегами */

/// http://upload.wikimedia.org/wikipedia/commons/6/65/Http-headers-status.gif
namespace fas{ namespace http{

namespace ap = ::fas::pattern;

class http_exception: public std::domain_error 
{
  int _code;
public:
  http_exception(): std::domain_error("HTTP Error"), _code(0) {} 
  explicit http_exception(const std::string& arg, int code): std::domain_error(arg), _code(code) {} 

  int get_code() const{ return _code;}
};

// 1xx: Informational (Информационные).
// 2xx: Success (Успешно).
// 3xx: Redirection (Перенаправление).
// 4xx: Client Error (Ошибка клиента).
// 5xx: Server Error (Ошибка сервера).

class informational_exception: public http_exception, public ap::int2type<1> 
{
public:
  informational_exception(): http_exception("HTTP informational exception", -100) {} 
  explicit informational_exception(const std::string& arg, int code): http_exception(arg, code) {} 
};

class success_exception: public http_exception, public ap::int2type<2> 
{
public:
  success_exception(): http_exception("HTTP success exception", -200) {} 
  explicit success_exception(const std::string& arg, int code): http_exception(arg, code) {} 
};

class redirection_exception: public http_exception, public ap::int2type<3> 
{
public:
  redirection_exception(): http_exception("HTTP redirection exception", -300) {} 
  explicit redirection_exception(const std::string& arg, int code): http_exception(arg, code) {} 
};

class client_error: public http_exception, public ap::int2type<4> 
{
public:
  client_error(): http_exception("HTTP client error exception", -400) {} 
  explicit client_error(const std::string& arg, int code): http_exception(arg, code) {} 
};

class server_error: public http_exception, public ap::int2type<5> 
{
public:
  server_error(): http_exception("HTTP server error exception", -500) {} 
  explicit server_error(const std::string& arg, int code): http_exception(arg, code) {} 
};



}}

#endif
