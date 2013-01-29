#ifndef FAS_MISC_HTTP_WRITER_HEADER_WRITER_HPP
#define FAS_MISC_HTTP_WRITER_HEADER_WRITER_HPP

#include <fas/aop/aspect.hpp>
#include "header_writer_aspect.hpp"

namespace fas{ namespace http{ namespace writer{

namespace aa = ::fas::aop;

template<typename U = aa::aspect<>, typename A = header_writer_aspect >
class header_writer
  : public aa::aspect_class<U, A>
{
  typedef aa::aspect_class<U, A> super;
  typedef typename super::aspect aspect;
  typedef typename super::template select<_header_>::type headers_list;

  bool _complete;
public:

  typedef typename aa::advice_cast< _initial_, aspect >::type initial_type;

  header_writer(): _complete(false){}

  const initial_type& initial() const
  {
    return this->get_aspect().template get<_initial_>();
  }

  initial_type& initial()
  {
    return this->get_aspect().template get<_initial_>();
  }

  operator bool () const { return _complete;}
  void clear()
  {
    _complete = false;
    this->get_aspect().template get< _make_clean_ >()(*this); 
  }

  template<typename T, typename Itr>
  Itr operator()(T&, Itr itr)
  {
    _complete = true;
    itr = this->get_aspect().template get<_initial_>()(itr);
    return _write_headers( headers_list(), itr );
  }

  template<typename N>
  bool clear_header()
  {
    if ( _clear_header( this->get_aspect().template get<N>() ) )
      return true;
    if ( _clear_common_header( N(), this->get_aspect().template get<_common_headers_>() ) )
      return true;
    return false;
  }

  /// Установка заголовка по тегу (например Content_Lenght)
  /// return false - если поле не найдено, и нет общего (универсального)  списка полей
  template<typename N, typename V>
  bool set_header(V value)
  {
    if ( _set_value( this->get_aspect().template get<N>(), value) )
      return true;
    if ( _set_common_value( N(), this->get_aspect().template get<_common_headers_>() , value) )
      return true;
    return false;
  }

  /// Установка заголовка по имени(например "Content_Lenght")
  template<typename V>
  bool set_header(const char*, V value)
  {
    return false;
  }

  template<typename N>
  const char* get_value()
  {
    return this->_get_value( this->get_aspect().template get<N>() );
  }

  template<typename N>
  int get_int_value()
  {
    return this->_get_int_value( this->get_aspect().template get<N>() );
  }


private:

  template<typename L, typename Itr>
  Itr _write_headers(L, Itr itr)
  {
    typedef typename L::left_type advice_type;
    typedef typename L::right_type list_type;
    itr = this->get_aspect().template get< advice_type >()( *this, itr);
    return _write_headers(list_type(), itr);
  }

  template< typename Itr>
  Itr _write_headers(ap::empty_type, Itr itr)
  {
    *(itr++) = '\r';
    *(itr++) = '\n';
    return itr;
  }

  template<typename AD, typename V>
  bool _set_value(AD& adv, V v)
  {
    adv.set_value(v);
    return true;
  }

  template<typename V>
  bool _set_value(ap::empty_type, V) {  return false; }

  template<typename N, typename AD, typename V>
  bool _set_common_value(N name, AD& adv, V v)
  {
    // TODO:
    adv.set_value1(name, v);
    return true;
  }

  template<typename N, typename V>
  bool _set_common_value(N, ap::empty_type, V) {  return false; }


/// получение указателя на буфер 
  template<typename AD>
  const char* _get_value(const AD& adv) const { return _get_value2(adv, typename AD::value_type() ); }

  template<typename V>
  const char* _get_value(ap::empty_type, V) const {  return 0; }

  template<typename AD>
  const char* _get_value2(const AD& adv, int) const { return adv.get_str_value(); }

  template<typename AD>
  const char* _get_value2(const AD& adv, const char*) const { return adv.get_value(); }

/// получение целочисленного значения
  template<typename AD>
  int _get_int_value(const AD& adv) const { return adv.get_value(); }

  int _get_int_value(ap::empty_type) const { return int(); }

///
  template<typename AD>
  bool _clear_header(AD& adv)
  {
    adv.clear(*this);
    return true;
  }

  bool _clear_header(ap::empty_type) {  return false; }

  template<typename N, typename AD>
  bool _clear_common_header(N name, AD& adv)
  {
    // Toodo
    return true;
  }

  template<typename N>
  bool _clear_common_header(N, ap::empty_type) {  return false; }

};



}}}

#endif
