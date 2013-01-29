#ifndef FAS_MISC_HTTP_WRITER_BODY_WRITER_ASPECT_HPP
#define FAS_MISC_HTTP_WRITER_BODY_WRITER_ASPECT_HPP

#include <fas/aop/aspect.hpp>

#include "content_writer.hpp"
#include "chunked_writer.hpp"
#include "../cleaner_advice.hpp"

namespace fas{ namespace http{ namespace writer{

namespace aa = ::fas::aop;
namespace ap = ::fas::pattern;
namespace ad = ::fas::adv;

typedef ap::type_list_n<
  content_writer,
  chunked_writer,
  cleaner_advice
> body_writer_list;

typedef aa::aspect< body_writer_list > body_writer_aspect;

}}}

#endif
