#ifndef FAS_TEST_UNIT_HPP
#define FAS_TEST_UNIT_HPP

#include <string>
#include <fas/aop/tag.hpp>

namespace fas{ namespace test{

namespace aa = ::fas::aop;

struct _gunit_{};

class unit
   : public aa::gtag<_gunit_>
{
  std::string _name;
  std::string _desc;
public:
  unit(const std::string& name = "", const std::string& desc = "")
   : _name(name), _desc(desc)
  {
  }

  const std::string& name() const { return _name; }
  const std::string& desc() const { return _desc; }
};


#define UNIT(name, desc)\
struct name: ::fas::test::unit\
{ \
  name(): unit( #name, desc) { }\
\
  template<typename T>\
  void operator()(T& t);\
};\
\
template<typename T>\
void name::operator()(T& t)\

}}

#endif
