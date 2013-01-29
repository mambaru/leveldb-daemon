#ifndef FAS_MISC_HTTP_READER_HEADER_READER_ASPECT_HPP
#define FAS_MISC_HTTP_READER_HEADER_READER_ASPECT_HPP

#include <fas/aop/aspect.hpp>
#include "headers/content_length.hpp"
#include "headers/transfer_encoding.hpp"
#include "initial_header.hpp"
#include "../cleaner_advice.hpp"

namespace fas{ namespace http{ namespace reader{

namespace aa = ::fas::aop;
namespace ad = ::fas::adv;
namespace ap = ::fas::pattern;

typedef ap::type_list_n<
  initial_header,
  content_length,
  transfer_encoding,
  cleaner_advice
>::type header_reader_list;

typedef aa::aspect< header_reader_list > header_reader_aspect;

}}}


#endif