#ifndef FAS_TEST_SUITE_CONFIG_HPP
#define FAS_TEST_SUITE_CONFIG_HPP

namespace fas{ namespace test{

struct error_type
{
  enum error_enum
  {
    passed,
    warning,
    error,
    fail,
    fatal,
    exception
  };
};

struct print_mode
{
  enum print_enum
  {
    never     = 0x0000,
    run       = 0x0001,
    ok        = 0x0002,
    passed    = 0x0004,
    warning   = 0x0008,
    error     = 0x0010,
    fail      = 0x0020,
    fatal     = 0x0040,
    exception = 0x0080,
    message   = 0x0100,
    beg       = 0x0200,
    end       = 0x0400,
    always    = 0xFFFF
  };
};

struct suite_config
{
  typedef error_type::error_enum error_enum;

  error_enum passed_as;
  error_enum warning_as;
  error_enum error_as;
  error_enum fail_as;
  error_enum fatal_as;
  error_enum exception_as;

  int print_name;
  int print_desc;

  suite_config()
    : passed_as(error_type::passed)
    , warning_as(error_type::warning)
    , error_as(error_type::error)
    , fail_as(error_type::fail)
    , fatal_as(error_type::fatal)
    , exception_as(error_type::exception)
    , print_name( print_mode::always & ~(print_mode::message|print_mode::warning) )
    , print_desc( print_mode::error | print_mode::fail | print_mode::fatal | print_mode::exception | print_mode::beg)
  {}
};

}}

#endif
