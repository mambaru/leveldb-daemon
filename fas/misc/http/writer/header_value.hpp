#ifndef FAS_MISC_HTTP_WRITER_HEADER_VALUE_HPP
#define FAS_MISC_HTTP_WRITER_HEADER_VALUE_HPP

#include "../values.hpp"
namespace fas{ namespace http{ namespace writer{

/** во writer может использоваться как значение header'а так и iniitial header'a
  * неизменяемое значение по умолчанию
  */
template<typename V = null_value >
struct header_value: V
{
  typedef V default_value;
  typedef const char* value_type;
  value_type current_value;

  header_value(): current_value(0) {}

  value_type get_value() const { return  current_value != 0 ? current_value : default_value::value(); }
  void set_value( value_type v ) { current_value = v; }

  void clear() { current_value = 0; }

  bool has_value() const
  {
    return current_value!=0 || default_value::value()!=0;
  }

  template<typename Itr>
  Itr operator()(Itr beg) const
  {
    const char *v = current_value != 0 ? current_value : default_value::value();
    if (v!=0) {
      for ( ; *v!='\0'; ++v) *(++beg) = *v; 
    }
    
    return beg;
  }
};

}}}

#endif
