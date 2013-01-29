#ifndef FAS_MISC_PHPSER_PHPSER_PROPERTY_HPP
#define FAS_MISC_PHPSER_PHPSER_PROPERTY_HPP

#include <fas/misc/meta.hpp>
#include "phpser_parser.hpp"

namespace fas{ namespace phpser{

namespace ap = ::fas::pattern;

namespace detail
{
  template<typename V, template<typename> class S>
  class property_deserializer< meta::metatype::property, V, S >
  {
    typedef typename V::target target; // Тип исходного класса
    typedef typename V::nametype nametype;
    typedef typename V::valuetype valuetype;
    typedef typename V::membertype membertype;

    property_deserializer()
      : _result(false)
    {}

    template<typename P>
    P operator() ( target& t, P beg, P end )
    {
       _result = false;
       P start = beg;
       beg = parser::parse_space(beg, end);
       beg = _deserializer1(t, beg, end);
       if ( !_deserializer1 )
         return start;
       beg = parser::parse_space(beg, end);
       beg = _deserializer2( membertype()(t), beg, end);
       if ( !_deserializer2 )
         return start;
       _result = true;
       return beg;
    };

    operator bool () const { return _deserializer1 && _deserializer2 ; };

    void clear() { _result = false; _deserializer1.clear(); _deserializer2.clear();};


  private:
    bool _result;
    S<nametype> _deserializer1;
    S<valuetype> _deserializer2;
  };
}


template<typename V, template<typename> class S>
class deserializer_t< meta::metatype::property, V, S >
  : public property_deserializer
{
};

template<typename V, template<typename> class S>
class deserializer_t< meta::metatype::attribute, V, S >
  : public property_deserializer
{
};

}}


#endif

