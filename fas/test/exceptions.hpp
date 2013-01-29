#ifndef FAS_TEST_EXCEPTIONS_HPP
#define FAS_TEST_EXCEPTIONS_HPP

#include <stdexcept>
#include <string>


namespace fas{ namespace test{

class exception
  : public std::logic_error
{
public:
  exception(const std::string& what="")
    : std::logic_error(what)
  {}
};

/** Не перывает suite*/
class statment_error
  : public exception
{
public:
  statment_error(const std::string& what="")
    : exception(what)
  {}
};

/** Перывает suite и переходит на следующий */
class fail_error
  : public exception
{
public:
  fail_error(const std::string& what="")
    : exception(what)
  {}
};

/** Перывает тестирование */
class fatal_error
  : public exception
{
public:
  fatal_error(const std::string& what="")
    : exception(what)
  {}
};


}}

#endif
