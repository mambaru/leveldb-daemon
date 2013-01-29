#ifndef FAS_MISC_HTTP_WRITER_WRITE_MODE_HPP
#define FAS_MISC_HTTP_WRITER_WRITE_MODE_HPP

namespace fas{ namespace http { namespace writer{

struct write_mode
{
  enum type{
    Content = 0,
    Chunked,
    Multipart,
    UserMode = 100
  };
};

}}}

#endif

