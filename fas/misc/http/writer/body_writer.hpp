#ifndef FAS_MISC_HTTP_WRITER_BODY_WRITER_HPP
#define FAS_MISC_HTTP_WRITER_BODY_WRITER_HPP

#include "body_writer_aspect.hpp"

namespace fas{ namespace http{ namespace writer{


template<typename A = aa::aspect<> >
class body_writer
  : public aa::aspect_class<body_writer_aspect, A>
{

  typedef aa::aspect_class<A, body_writer_aspect> super;
  typedef typename super::aspect aspect;
  typedef typename aspect::template select<_body_writer_>::type writer_list;

  bool _complete;
public:

  body_writer(): _complete(false)
  {
  }

  operator bool () const { return _complete; }


  void clear()
  {
    _complete = false;
    this->get_aspect().template get< _make_clean_ >()(*this);
  }


  template<typename T, typename H, typename Itr>
  void operator()(T& , H& h, int mode, const char* beg, size_t s, Itr itr)
  {
    _complete = false;
    _write_body( writer_list(), h, mode, beg, s, itr );
  }

private:
  
  template<typename L, typename H, typename Itr>
  void _write_body(L, H& h, int mode, const char* beg, size_t s, Itr itr)
  {
    typedef typename L::left_type writer_type;
    typedef typename L::right_type tail_list;
    if ( !_write_body2(this->get_aspect().template get<writer_type>(), h, mode, typename aa::tag_list_traits<typename writer_type::tag_list>::hierarchy(), beg, s, itr) )
       _write_body( tail_list(), h, mode, beg, s, itr);
  }

  template<typename AD, typename H, typename Itr, int M >
  bool _write_body2(AD& adv, H& h, int mode, ap::int2type<M>, const char* beg, size_t s, Itr itr)
  {
    if ( M == mode )
      adv(*this, h, beg, s, itr );
    else
      return false;
    _complete = adv;
    return true;
  }

  template<typename H, typename Itr>
  void _write_body(ap::empty_type, H&,int, const char* , size_t, Itr)
  {
  }


};

}}}
#endif
