#ifndef FAS_MISC_HTTP_READER_INITIAL_RESPONSE_HPP
#define FAS_MISC_HTTP_READER_INITIAL_RESPONSE_HPP

#include "initial_base.hpp"

namespace fas{ namespace http{ namespace reader{

struct initial_response: initial_base
{
  std::string get_version() const { return initial_base::get_first(); }
  std::string get_code() const { return initial_base::get_second(); }
  std::string get_phrase() const { return initial_base::get_thrid(); }

  bool check_version(const char *value) const { return initial_base::check_first(value); }
  bool check_code(const char *value) const { return initial_base::check_second(value); }
  bool check_phrase(const char *value) const { return initial_base::check_thrid(value); }
};

}}}

#endif
