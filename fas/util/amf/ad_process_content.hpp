//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_UTIL_AMF_AD_PROCESS_CONTENT_HPP
#define FAS_UTIL_AMF_AD_PROCESS_CONTENT_HPP

#include <fas/pattern/type_list.hpp>
#include <fas/util/amf/tags.hpp>

namespace fas{ namespace  util{ namespace amf{

namespace ap = ::fas::pattern;

template<unsigned char t, typename T>
struct content
{
  enum { content_type = t };
  typedef T content_tag;
};

template< typename L >
class ad_process_content
{
  const char* _d;
  size_t _s;
  unsigned char _content_type;

  template<typename TT, typename T>
  void _process(T& t)
  {
    t.get_aspect().template get<TT>()(t, _d, _s);
  }

  template<typename T, typename LL, typename LR>
  void _select_content( T& t, const ap::type_list<LL, LR>& )
  {
    if (LL::content_type == _content_type)
      _process<typename LL::content_tag>(t);
    else
      _select_content(LR());
  }

  template<typename T, typename LL>
  void _select_content( T& t,
          const ap::type_list<LL, ap::empty_type>& )
  {
    if ( LL::content_type == _content_type)
      _process<typename LL::content_tag>(t);
  }

  template<typename T, typename LL>
  void _select_content( T& t, const LL& )
  {
    if ( LL::content_type == _content_type)
      _process<typename LL::content_tag>(t);
  }


public:
  template<typename T>
  void operator()(T& t, int content,  const char* d, size_t s)
  {
    _d = d; _s = s;
    _content_type = content;
    _select_content(t, L());
  }
};

}}}

#endif
