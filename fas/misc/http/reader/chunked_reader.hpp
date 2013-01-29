#ifndef FAS_MISC_HTTP_READER_CHUNKED_READER_HPP
#define FAS_MISC_HTTP_READER_CHUNKED_READER_HPP

#include "ad_chunked_reader.hpp"
#include "../tags.hpp"

#include <fas/aop/advice.hpp>

namespace fas{ namespace http{ namespace reader{

namespace aa = ::fas::aop;

struct chunked_reader
  : aa::advice<
      _chunked_reader_,
      ad_chunked_reader
    >
{
};

}}}

#endif
