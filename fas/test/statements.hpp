#ifndef FAS_TEST_STATMENTS_HPP
#define FAS_TEST_STATMENTS_HPP

#include <string>
#include <sstream>
#include "formatting.hpp"
namespace fas{ namespace test{

/** Добавить манипулятор вторым параметром */
template<typename M>
struct statement
{
private:
  statement(const statement<M>&){}
public:
  bool result;
  bool fail;
  bool unk;
  std::exception ex;
  bool _user_msg;
  std::stringstream* smessage;

  statement(): result(false), fail(false), unk(false), _user_msg(false), smessage(new std::stringstream) {}
  ~statement() { delete smessage; }
  

  template<typename T>
  statement<M>& operator % (T t)
  {
    // Если просто ошибка fail==false, то сообщение заменяется
    // в противном случае расширяется
    if ( !_user_msg )
    {
      delete smessage;
      smessage = new std::stringstream;
      _user_msg = true;
    }

    *smessage << t;
    return *this;
  }

  template<typename T>
  statement<M>& operator << (T t)
  {
    *smessage << t;
    _user_msg = true;
    return *this;
  }

  std::string message() const { return smessage->str();}
};

struct assert {};
struct expect {};
struct critical {};

template<typename M = assert>
struct equal: statement<M>
{
  typedef statement<M> super;

  template<typename L, typename R>
  equal(L left, R right): statement<M>()
  {
    try
    {
       super::result = ( left == right );
       if ( !super::result )
         *super::smessage << "equal statement false. ";
    }
    catch(const std::exception& e)
    {
      super::result = false;
      super::fail = true;
      super::ex = e;
    }
    catch(...)
    {
      super::result = false;
      super::fail = true;
      super::unk = true;
    }
  }
};

template<typename M = assert>
struct is_true: statement<M>
{
  typedef statement<M> super;

  is_true(bool is): statement<M>()
  {
    try
    {
       super::result = is;
       if ( !super::result )
         *super::smessage << "is_true statement false. ";
    }
    catch(const std::exception& e)
    {
      super::result = false;
      super::fail = true;
      super::ex = e;
    }
    catch(...)
    {
      super::result = false;
      super::fail = true;
      super::unk = true;
    }
  }
};

template<typename M = assert>
struct is_false: statement<M>
{
  typedef statement<M> super;

  //template<typename L, typename R>
  is_false(bool is): statement<M>()
  {
    try
    {
       super::result = !is;
       if ( !super::result )
         *super::smessage << "is_false statement true. ";
    }
    catch(const std::exception& e)
    {
      super::result = false;
      super::fail = true;
      super::ex = e;
    }
    catch(...)
    {
      super::result = false;
      super::fail = true;
      super::unk = true;
    }
  }
};


// void (*)( ::fas::test::_passed_ )
template<typename M = expect, typename F = _message_ >
struct info: statement<M>
{
  typedef statement<M> super;
  typedef M statement_type;
  typedef void (*manip)( F );
  info(const std::string& msg)// : message(msg) 
  {
    *super::smessage << msg;
  }
};

struct message
  : info< expect, _message_ >
{
   message(const std::string& msg)
    : info< expect, _message_ >(msg) 
   {}
};

struct warning: info< expect, _warning_ > { warning(const std::string& msg): info< expect, _warning_ >(msg) {} };

struct error: info< assert, _error_ > { error(const std::string& msg): info< assert, _error_ >(msg) {} };

struct fatal: info< critical, _fatal_> { fatal(const std::string& msg): info< critical, _fatal_ >(msg) {} };


}}

#endif
