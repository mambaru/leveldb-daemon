#ifndef FAS_TEST_ISUITE_HPP
#define FAS_TEST_ISUITE_HPP

#include <fas/test/suite_result.hpp>
#include <fas/test/suite_config.hpp>


namespace fas{ namespace test{

struct isuite
{
  virtual void run(const suite_config& conf, suite_result& result) = 0;
  virtual int size() const = 0;
};

}}

#endif
