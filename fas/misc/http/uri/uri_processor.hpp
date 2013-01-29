#ifndef FAS_MISC_HTTP_URI_URI_PROCESSOR_HPP
#define FAS_MISC_HTTP_URI_URI_PROCESSOR_HPP

#include "../tags.hpp"
#include "uri_aspect.hpp"

namespace fas{ namespace http{ namespace uri{

template<typename A = aa::aspect<> >
struct uri_processor
  : aa::aspect_class<uri_aspect, A >
{
  typedef aa::aspect_class<uri_aspect, A> super;
  typedef typename super::aspect aspect;
  typedef typename super:: template select< _rule_ >::type selected_rules;

  /// Обрабатываем правила начиная с последнего
  typedef typename ap::reverse<selected_rules>::type rules;

  template<typename T, typename H, typename B>
  void operator()(T& t, const H& h, const B& b)
  {
    _process(t, rules(), h, b );
  }

private:

  template<typename T, typename L, typename H, typename B>
  void _process(T& t, L, const H& h, const B& b)
  {
     typedef typename L::left_type current_rule;
     typedef typename L::right_type tail_rules;

     if ( !_process2(t, this->get_aspect().template get<current_rule>(), h, b) )
       _process(t, tail_rules(), h, b );
  }

  template<typename T, typename H, typename B>
  void _process(T& , ap::empty_type, const H& , const B& ) 
  {
  }

  template<typename T, typename D, typename H, typename B>
  bool _process2(T& t, D& adv, const H& h, const B& b)
  {
    typedef typename D::processor_tag processor_tag;

    if ( adv( *this, h.initial().second.first, h.initial().second.second ) )
      return _process_next(t, t.get_aspect().template get<processor_tag>(), h, b);
    return false;
  }

  template<typename T, typename H, typename B>
  bool _process2(T&, ap::empty_type, const H& , const B& )
  {
    return false;
  }

  template<typename T, typename D, typename H, typename B>
  bool _process_next(T& t, D& adv, const H& h, const B& b)
  {
    adv(t, h, b);
    return true;
  }

  template<typename T, typename H, typename B>
  bool _process_next(T&, ap::empty_type, const H& , const B& )
  {
    return false;
  }
};

}}}

#endif
