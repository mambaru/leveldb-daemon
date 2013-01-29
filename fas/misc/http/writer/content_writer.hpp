#ifndef FAS_MISC_HTTP_WRITER_CONTENT_WRITER_HPP
#define FAS_MISC_HTTP_WRITER_CONTENT_WRITER_HPP

#include "ad_content_writer.hpp"
#include "../tags.hpp"

#include <fas/aop/advice.hpp>
#include "write_mode.hpp"

namespace fas{ namespace http{ namespace writer{

namespace aa = ::fas::aop;
namespace ap = ::fas::pattern;

struct content_writer
  : aa::advice<
      aa::tag_list_n<
        _content_writer_,
        ap::int2type< write_mode::Content >,
        aa::gtag<_body_writer_>
      >::type
     , ad_content_writer
    > {};

}}}

#endif
