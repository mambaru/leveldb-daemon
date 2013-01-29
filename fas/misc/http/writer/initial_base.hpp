#ifndef FAS_MISC_HTTP_WRITER_INITIAL_BASE_HPP
#define FAS_MISC_HTTP_WRITER_INITIAL_BASE_HPP

#include <string>
#include "header_value.hpp"

namespace fas{ namespace http{ namespace writer{

template<typename T, int>
struct initial_field: T{};

template<typename F, typename S, typename TH>
struct initial_base
  : initial_field<F, 0>
  , initial_field<S, 1>
  , initial_field<TH, 2>
{
  typedef initial_field<F, 0> first_type;
  typedef initial_field<S, 1> second_type;
  typedef initial_field<TH, 2> thrid_type;

  void clear()
  {
    first_type::clear();
    second_type::clear();
    thrid_type::clear();
  }

  first_type& first() { return static_cast<first_type&>(*this); }
  second_type& second() { return static_cast<second_type&>(*this); }
  thrid_type& thrid() { return static_cast<thrid_type&>(*this); }

  const first_type& first() const { return static_cast< const first_type&>(*this); }
  const second_type& second() const { return static_cast< const second_type&>(*this); }
  const thrid_type& thrid() const { return static_cast<const thrid_type&>(*this); }

  typename first_type::value_type get_first() const { return first_type::get_value(); }
  typename second_type::value_type get_second() const { return second_type::get_value(); }
  typename thrid_type::value_type get_thrid() const { return thrid_type::get_value(); }

  void set_first(typename first_type::value_type v) { first_type::set_value(v); }
  void set_second(typename second_type::value_type v) { second_type::set_value(v); }
  void set_thrid(typename thrid_type::value_type v) { thrid_type::set_value(v); }

  template<typename Itr>
  Itr operator()(Itr beg)
  {
    if ( first_type::has_value() )
    {
      beg = first_type::operator()(beg);
      *(beg++) = ' ';
    }

    if ( second_type::has_value() )
    {
      beg = second_type::operator()(beg);
      *(beg++) = ' ';
    }

    beg = thrid_type::operator()(beg);
    *(beg++) = '\r';
    *(beg++) = '\n';
    return beg;
  }
private:
};

}}}

#endif
