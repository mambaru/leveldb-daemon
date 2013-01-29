#ifndef FAS_MISC_HTTP_READER_INITIAL_REQUEST_HPP
#define FAS_MISC_HTTP_READER_INITIAL_REQUEST_HPP

#include "initial_base.hpp"

namespace fas{ namespace http{ namespace reader{

struct initial_request: initial_base
{
  std::string get_method() const { return initial_base::get_first(); }
  std::string get_uri() const { return initial_base::get_second(); }
  std::string get_version() const { return initial_base::get_thrid(); }

  bool check_method(const char *value) const { return initial_base::check_first(value); }
  bool check_uri(const char *value) const { return initial_base::check_second(value); }
  bool check_version(const char *value) const { return initial_base::check_thrid(value); }
};

}}}

#endif

