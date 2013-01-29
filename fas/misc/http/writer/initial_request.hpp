#ifndef FAS_MISC_HTTP_WRITER_INITIAL_REQUEST_HPP
#define FAS_MISC_HTTP_WRITER_INITIAL_REQUEST_HPP

#include "initial_base.hpp"
#include "header_value.hpp"
#include "header_int_value.hpp"
#include "../values.hpp"
#include "../phrases.hpp"


namespace fas{ namespace http{ namespace writer{

struct initial_request
  //: initial_base
//{
  : initial_base< header_value<POST>, header_value<root_uri_value>, header_value<HTTP11> >
{
  typedef initial_base< header_value<POST>, header_value<root_uri_value>, header_value<HTTP11> > super;

  void set_method(const char* value) { super::set_first(value); }
  void set_uri(const char* value) { super::set_second(value); }
  void set_version(const char* value) { super::set_thrid(value); }

  const char* get_method() const { return super::get_first(); }
  const char* get_uri() const { return super::get_second(); }
  const char* get_version() const { return super::get_thrid(); }

/*
  std::string get_method() const { return initial_base::get_first(); }
  std::string get_uri() const { return initial_base::get_second(); }
  std::string get_version() const { return initial_base::get_thrid(); }

  bool check_method(const char *value) const { return initial_base::check_first(value); }
  bool check_uri(const char *value) const { return initial_base::check_second(value); }
  bool check_version(const char *value) const { return initial_base::check_thrid(value); }
*/

/*  std::string get_version() const { return initial_base::get_first(); }
  std::string get_code() const { return initial_base::get_second(); }
  std::string get_phrase() const { return initial_base::get_thrid(); }

  bool check_version(const char *value) const { return initial_base::check_first(value); }
  bool check_code(const char *value) const { return initial_base::check_second(value); }
  bool check_phrase(const char *value) const { return initial_base::check_thrid(value); }
*/
};

}}}

#endif

