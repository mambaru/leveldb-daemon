#ifndef FAS_TEST_SUITE_HPP
#define FAS_TEST_SUITE_HPP

#include <fas/test/statements.hpp>
#include <fas/test/suite_result.hpp>
#include <fas/test/suite_config.hpp>
#include <fas/test/formatting.hpp>
#include <stdexcept>

#include <fas/aop/aspect.hpp>

namespace fas{ namespace test{

namespace aa = ::fas::aop;
namespace ap = ::fas::pattern;

template<typename A = aa::aspect<> >
class suite
  : public aa::aspect_class< aa::aspect<>, A>
  , public isuite
{
  typedef suite<A> self;
  typedef aa::aspect_class< aa::aspect<>, A> super;
  typedef typename ap::select< _gunit_, typename super::aspect::advice_list>::type unit_list;
public:
  suite(const std::string& name = "", const std::string& desc = ""): _name(name), _desc(desc)
  {
    suite_register::instance()->registry(this);
  }

  self& operator << ( const statement<expect>& st )
  {
    if ( st.result == false)
      std::cout << ERROR << st.message() << std::endl;
    return *this;
  }

  self& operator << ( const statement<assert>& st )
  {
    if ( st.result == false)
    {
      std::cout << ERROR << st.message() << std::endl;
      throw std::logic_error("error statement");
    }
    return *this;
  }

  self& operator << ( const statement<critical>& st )
  {
    if ( st.result == false)
    {
      std::cout << ERROR << st.message() << std::endl;
      throw std::logic_error("error statement");
    }
    return *this;
  }

  template<typename F>
  self& operator << ( const info<expect, F>& st )
  {
    typename info<expect, F>::manip manip = typename info<expect, F>::manip();
    std::cout << manip << st.message() << std::endl;
    return *this;
  }

  template<typename F>
  self& operator << ( const info<assert, F>& st )
  {
    typename info<assert, F>::manip manip;
    std::cout << manip << st.message() << std::endl;
    return *this;
  }

  template<typename F>
  self& operator << ( const info<critical, F>& st )
  {
    typename info<critical, F>::manip manip;
    std::cout << manip << st.message() << std::endl;
    return *this;
  }

  void run(const suite_config& conf, suite_result& result)
  {
    _conf = &conf;
    _result = &result;

    std::cout << SUITE_BEG << size() << " tests";
    if ( _conf->print_name & print_mode::beg )
      std::cout << " from " << _name<<". ";
    else
      std::cout << ". ";
    if ( _conf->print_desc & print_mode::beg )
      std::cout <<_desc;
    std::cout << std::endl;

    run_units( unit_list() );
    std::cout << SUITE_END <<std::endl;
  }

  template<typename L>
  void run_units(L)
  {
    std::string unit_name = this->get_aspect().template get<typename L::left_type>().name();
    std::string unit_desc = this->get_aspect().template get<typename L::left_type>().desc();

    std::cout<< RUN << unit_name << ". " << unit_desc << std::endl;

    try
    {
      try
      {
        this->get_aspect().template get<typename L::left_type>()(*this);
        std::cout<< OK << unit_name << ". " << unit_desc << std::endl;
      }
      catch(const std::exception& e)
      {
        std::cout<< FAIL << unit_name << ". " << unit_desc << ": exception: " << e.what() << std::endl;
      }
      catch(...)
      {
        std::cout<< FAIL << unit_name << ". " << unit_desc << ": unhandled exception" << std::endl;
      }
    }
    catch(...)
    {
      // std::cout<< FAIL << unit_name << ". " << unit_desc << std::endl;
    }
    run_units( typename L::right_type() );
  }

  void run_units(ap::empty_type){}

  virtual int size() const { return ap::length<unit_list>::result;};

private:

 const suite_config* _conf;
 suite_result* _result;

 std::string _name;
 std::string _desc;

};

}}

#endif
