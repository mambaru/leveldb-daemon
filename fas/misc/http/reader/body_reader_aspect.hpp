#ifndef FAS_MISC_HTTP_READER_BODY_READER_ASPECT_HPP
#define FAS_MISC_HTTP_READER_BODY_READER_ASPECT_HPP

#include <fas/aop/aspect.hpp>

#include "content_reader.hpp"
#include "chunked_reader.hpp"
#include "../cleaner_advice.hpp"

namespace fas{ namespace http{ namespace reader{

namespace aa = ::fas::aop;
namespace ap = ::fas::pattern;
namespace ad = ::fas::adv;

typedef ap::type_list_n<
  content_reader,
  chunked_reader,
  cleaner_advice
> body_reader_list;

typedef aa::aspect<body_reader_list> body_reader_aspect;

}}}

#endif
