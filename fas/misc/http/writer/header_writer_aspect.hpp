#ifndef FAS_MISC_HTTP_WRITER_HEADER_WRITER_ASPECT_HPP
#define FAS_MISC_HTTP_WRITER_HEADER_WRITER_ASPECT_HPP

#include <fas/aop/aspect.hpp>
#include "headers/content_length.hpp"
#include "headers/transfer_encoding.hpp"
#include "initial_header.hpp"
#include "../cleaner_advice.hpp"

namespace fas{ namespace http{ namespace writer{

namespace aa = ::fas::aop;
namespace ad = ::fas::adv;
namespace ap = ::fas::pattern;

typedef ap::type_list_n<
  initial_header,
  content_length,
  transfer_encoding,
  cleaner_advice
>::type header_writer_list;

typedef aa::aspect< header_writer_list > header_writer_aspect;

}}}

#endif
