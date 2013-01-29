#ifndef FAS_MISC_HTTP_EXCEPTIONS_MACROS_DEFINED_HPP
#define FAS_MISC_HTTP_EXCEPTIONS_MACROS_DEFINED_HPP

#include "../exceptions.hpp"

namespace fas{ namespace http{

template<int C, typename PH, typename B >
struct status_exception
  : PH, B
{
public:
  status_exception(): B(PH::value(), C) {};
  enum {code = C};
  const char *phrase() const { return PH::value();}
};

}}

#define FAS_HTTP_INFORMATIONAL_EXCEPTION(name, code) namespace fas { namespace http { \
  class name##_informational: public status_exception<code, name, informational_exception>{};  } }

#define FAS_HTTP_SUCCESS_EXCEPTION(name, code) namespace fas { namespace http { \
  class name##_success: public status_exception<code, name, success_exception>{};  } }

#define FAS_HTTP_REDIRECTION_EXCEPTION(name, code) namespace fas { namespace http { \
  class name##_redirection: public status_exception<code, name, redirection_exception>{};  } }

#define FAS_HTTP_CLIENT_ERROR(name, code) namespace fas { namespace http { \
  class name##_error: public status_exception<code, name, client_error>{};  } }

#define FAS_HTTP_SERVER_ERROR(name, code) namespace fas { namespace http { \
  class name##_error: public status_exception<code, name, server_error>{};  } }


/*
#define FAS_HTTP_INFORMATIONAL_EXCEPTION(name, code, text) namespace fas { namespace http { \
  class name##_exception: public informational_exception, public ::fas::pattern::int2type<code>{\
    name##_exception(): informational_exception(text) {} };  } }

#define FAS_HTTP_SUCCESS_EXCEPTION(name, code, text) namespace fas { namespace http { \
  class name##_exception: public success_exception, public ::fas::pattern::int2type<code>{\
    name##_exception(): success_exception(text) {} };  } }

#define FAS_HTTP_REDIRECTION_EXCEPTION(name, code, text) namespace fas { namespace http { \
  class name##_exception: public redirection_exception, public ::fas::pattern::int2type<code>{\
    name##_exception(): redirection_exception(text) {} };  } }

#define FAS_HTTP_CLIENT_ERROR(name, code, text) namespace fas { namespace http { \
  class name##_exception: public client_error, public ::fas::pattern::int2type<code>{\
    name##_exception(): client_error(text) {} };  } }

#define FAS_HTTP_SERVER_ERROR(name, code, text) namespace fas { namespace http { \
  class name##_exception: public server_error, public ::fas::pattern::int2type<code>{\
    name##_exception(): server_exception(text) {} };  } }
*/
/*
#include <stdexcept>
#include <fas/pattern/conversion.hpp>

namespace fas{ namespace http{


namespace ap = ::fas::pattern;

class http_exception: public std::domain_error{};

// 1xx: Informational (Информационные).
// 2xx: Success (Успешно).
// 3xx: Redirection (Перенаправление).
// 4xx: Client Error (Ошибка клиента).
// 5xx: Server Error (Ошибка сервера).

class informational_exception: public http_exception, public ap::int2type<1> {};
class success_exception: public http_exception, public ap::int2type<2> {};
class redirection_exception: public http_exception, public ap::int2type<3> {};
class client_error: public http_exception, public ap::int2type<4> {};
class server_error: public http_exception, public ap::int2type<5> {};



}}
*/

#endif
