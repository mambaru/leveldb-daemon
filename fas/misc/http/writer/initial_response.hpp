#ifndef FAS_MISC_HTTP_WRITER_INITIAL_RESPONSE_HPP
#define FAS_MISC_HTTP_WRITER_INITIAL_RESPONSE_HPP

#include "initial_base.hpp"
#include "header_value.hpp"
#include "header_int_value.hpp"
#include "../values.hpp"
#include "../phrases.hpp"

namespace fas{ namespace http{ namespace writer{

struct initial_response
  : initial_base< header_value<HTTP11>, header_intdef_value<200>, header_value<OK>  >
{
  typedef initial_base< header_value<HTTP11>, header_intdef_value<200>, header_value<OK>  > super;
  void set_version(const char* value) { super::set_first(value); }
  void set_code(int value) { super::set_second(value); }
  void set_phrase(const char* value) { super::set_thrid(value); }

  const char* get_version() const { return super::get_first(); }
  int get_code() const { return super::get_second(); }
  const char* get_phrase() const { return super::get_thrid(); }

  template<typename S>
  void set_status(const S& s)
  {
    this->set_code( s.get_code() );
    this->set_phrase( s.what() );
  }
};

}}}

#endif
