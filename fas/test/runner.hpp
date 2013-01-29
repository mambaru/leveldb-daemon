#ifndef FAS_TEST_RUNNER_HPP
#define FAS_TEST_RUNNER_HPP

#include <fas/test/suite_register.hpp>
#include <fas/test/isuite.hpp>

#include <fas/aop/aspect.hpp>

#include <vector>
#include <algorithm>

namespace fas{ namespace test{

namespace aa = ::fas::aop;

template<typename A = aa::aspect<> >
class runner
  : aa::aspect_class< aa::aspect<>, A>
{
  typedef std::vector<isuite*> suite_list;
public:
  runner(int argc, char* argv[] )
  {
  }

  void registry(isuite* suite)
  {
    // _suite_list.push_back(suite);
  }

  struct unit_count_f
  {
    int count;
    unit_count_f(): count(0) {};
    operator int() const { return count; }
    void operator()( isuite* s)
    {
      count+=s->size();
    }
  };

  int run()
  {
    suite_list sl = suite_register::instance()->get();
    int unit_count = std::for_each( sl.begin(), sl.end(), unit_count_f() );
    std::cout <<"Running "<<unit_count<<" tests from "<<sl.size()<<" test suites"<<std::endl;
    suite_list::iterator beg = sl.begin();
    suite_list::iterator end = sl.end();
    suite_config conf;
    for ( ; beg!=end; ++beg )
    {
      suite_result result;
      (*beg)->run(conf, result);
    }

    /*
    std::cout<< PASSED << " " << unit_count << " tests"<<std::endl;
    std::cout<< RUN << std::endl;
    std::cout<< OK << std::endl;
    std::cout<< FAIL << std::endl;
    std::cout<< WARNING << std::endl;
    std::cout<< MESSAGE << std::endl;
    */
   return 0;
  }

private:
  //suite_list _suite_list;
};


}}

#endif
