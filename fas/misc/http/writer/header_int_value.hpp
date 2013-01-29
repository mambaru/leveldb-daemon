#ifndef FAS_MISC_HTTP_WRITER_HEADER_INT_VALUE_HPP
#define FAS_MISC_HTTP_WRITER_HEADER_INT_VALUE_HPP

#include "header_value.hpp"

namespace fas{ namespace http{ namespace writer{

namespace detail
{
  template<typename T>
  struct is_signed_integer
  {
    enum { result = T(-1) < T(1) };

    static bool is_less_zero(T v)
    {
      return result && (v < 0);
    }
  };

  template<typename T>
  struct integer_buffer_size
  {
    enum { result = sizeof(T)*2 + sizeof(T)/2 + sizeof(T)%2 + is_signed_integer<T>::result };
  };
}


template<int D = 0, bool U = false>
struct header_int_value_base
  : private header_value<>
{
  typedef header_value<> super;
private:
  int value;
  bool set_flag;

public:

  typedef int value_type;

  header_int_value_base()
  : value()
  , set_flag(false) 
  {
    if ( U ) _set_buffer(D);
    super::set_value(_buffer);
  };

  value_type get_value() const { return set_flag ? value : D; }

  const char* get_str_value() const { return super::get_value(); }

  void set_value(value_type v)
  {
    set_flag = true;
    value = v;
    _set_buffer(set_flag ? value : D);
    super::set_value(_buffer);
  }

  bool has_value() const { return set_flag || U;}

  void clear() 
  {
    value = value_type();
    set_flag = false;
    super::clear();

    if ( U ) _set_buffer(D);
    super::set_value(_buffer);
  }

  template<typename Itr>
  Itr operator()(Itr itr) const
  {
    return super::operator ( )(itr);
  }

  /*
  template<typename Itr>
  Itr operator()( Itr itr ) const
  {
    char buf[ sizeof(value_type)*2 + sizeof(value_type)/2 + sizeof(value_type)%2 + 1 ];
    register char *beg = buf;
    register char *end = buf;
    register value_type v = set_flag ? value : D;
    if (v==0) *(end++) = '0';
    else
    {
       if ( v < 0 )
       {
         *(end++)='-';
         ++beg;
         for( ; v!=0 ; ++end, v/=10) 
           *end = '0' - v%10;
       }
       else
         for( ; v!=0 ; ++end, v/=10) 
           *end = '0' + v%10;
    }

    for ( register char* cur = end ; cur-beg > 1;--cur, ++beg)
    { *beg ^= *(cur-1); *(cur-1)^=*beg; *beg^=*(cur-1); }

    for (beg = buf; beg!=end; ++beg)
      *(itr++)=*beg;

    return itr;
  }
  */


private:
  void _set_buffer(value_type v)
  {
    register char *beg = _buffer;
    register char *end = _buffer;
    if (v==0) *(end++) = '0';
    else
    {
       if ( v < 0 )
       {
         *(end++)='-';
         ++beg;
         for( ; v!=0 ; ++end, v/=10) 
           *end = '0' - v%10;
       }
       else
         for( ; v!=0 ; ++end, v/=10) 
           *end = '0' + v%10;
    }

    for ( register char* cur = end ; cur-beg > 1;--cur, ++beg)
    { *beg ^= *(cur-1); *(cur-1)^=*beg; *beg^=*(cur-1); }

    *end = '\0';
  }

  char _buffer[ sizeof(value_type)*2 + sizeof(value_type)/2 + sizeof(value_type)%2 + 1 ];
};

struct header_int_value: header_int_value_base<0, false> {};

template<int N>
struct header_intdef_value: header_int_value_base<N, true> {};

}}}

#endif
