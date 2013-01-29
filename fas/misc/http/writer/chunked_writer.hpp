#ifndef FAS_MISC_HTTP_WRITER_CHUNKED_WRITER_HPP
#define FAS_MISC_HTTP_WRITER_CHUNKED_WRITER_HPP

#include "ad_chunked_writer.hpp"
#include "../tags.hpp"

#include <fas/aop/advice.hpp>
#include "write_mode.hpp"

namespace fas{ namespace http{ namespace writer{

namespace aa = ::fas::aop;

struct chunked_writer
  : aa::advice<
      aa::tag_list_n<
        _chunked_writer_,
        ap::int2type< write_mode::Chunked >,
        aa::gtag<_body_writer_>
      >::type
     , ad_chunked_writer
    > {};

}}}

#endif
