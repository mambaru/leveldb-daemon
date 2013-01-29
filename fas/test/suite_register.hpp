#ifndef FAS_TEST_SUITE_REGISTER_HPP
#define FAS_TEST_SUITE_REGISTER_HPP


#include <fas/test/suite_result.hpp>
#include <fas/test/suite_config.hpp>

#include <vector>

namespace fas{ namespace test{

class suite_register
{
  typedef std::vector<isuite*> suite_list;
  suite_list _suite_list;
  suite_register(){};
  ~suite_register(){};
public:
  static suite_register* instance()
  {
    static suite_register* _instance = 0;
    if ( _instance == 0 )
      _instance = new suite_register();
    return _instance;
  }
  void registry(isuite* suite)
  {
  _suite_list.push_back(suite);

  }
  suite_list get()
  {
  return _suite_list;
  }
};

}}

#endif
