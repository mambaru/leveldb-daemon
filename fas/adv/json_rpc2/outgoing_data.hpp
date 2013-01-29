#ifndef FAS_ADV_JSON_RPC2_OUTGOING_DATA_HPP
#define FAS_ADV_JSON_RPC2_OUTGOING_DATA_HPP

#include <vector>
namespace fas{ namespace adv{ namespace json_rpc2{

class outgoing_data
{
public:
  typedef std::vector<char> data_type;

  template<typename T>
  void clear(T&)
  {
    data_type().swap(_data);
  }

  data_type& operator()() { return _data;}
  const data_type& operator()() const { return _data;}
private:
  data_type _data;
};

}}}

#endif
