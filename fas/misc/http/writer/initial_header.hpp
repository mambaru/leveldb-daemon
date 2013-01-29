#ifndef FAS_MISC_HTTP_WRITER_INITIAL_HEADER_HPP
#define FAS_MISC_HTTP_WRITER_INITIAL_HEADER_HPP

#include "../tags.hpp"
#include "ad_initial.hpp"
#include "initial_base.hpp"

#include <fas/aop/advice.hpp>

namespace fas{ namespace http{ namespace writer{

namespace aa = ::fas::aop;

/** заголовок по умолчанию (походит как для клиента так и для сервера, но поля не парсятся) */
struct initial_header
  : aa::advice< _initial_, ad_initial<> >
{
};

}}}

#endif

