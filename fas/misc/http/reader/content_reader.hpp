#ifndef FAS_MISC_HTTP_READER_CONTENT_READER_HPP
#define FAS_MISC_HTTP_READER_CONTENT_READER_HPP

#include "ad_content_reader.hpp"
#include "../tags.hpp"

#include <fas/aop/advice.hpp>

namespace fas{ namespace http{ namespace reader{

namespace aa = ::fas::aop;

struct content_reader
  : aa::advice<
      _content_reader_,
      ad_content_reader
    >
{
};

}}}

#endif
