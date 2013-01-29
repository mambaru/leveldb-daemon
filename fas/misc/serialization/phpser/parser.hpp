#ifndef FAS_MISC_SEREALIZETION_PHPSER_PARSER_HPP
#define FAS_MISC_SEREALIZETION_PHPSER_PARSER_HPP

#include "../exceptions.hpp"
#include "../parser.hpp"

namespace fas{ namespace serialization{ namespace phpser {

class parser
{
	typedef ::fas::serialization::parser common_parser;

public:

  template<typename T, typename R>
  inline static R unserialize_integer( T& v, R r ) { return common_parser::unserialize_integer(v, r); }

	template<typename R>
  inline static R parse_space( R r ) { return common_parser::parse_space( r, spacer(), no_comment() );}

  template<typename R>
  inline static bool is_number( R r )  { return common_parser::is_number(r); }

  template<typename R>
  inline static R parse_digits( R r )  { return common_parser::parse_digits(r); }

  template<typename R>
  inline static R parse_number( R r )  { return common_parser::parse_number(r); }

  template<typename R>
  inline static bool is_numeric( R r )  { return r && ( *r=='i' || *r=='d' ); }

  template<typename R>
  inline static bool is_int( R r )  { return r && ( *r=='i'); }

  template<typename R>
  inline static bool is_double( R r )  { return r && ( *r=='d'); }

  template<typename R>
  inline static bool is_float( R r )  { return r && ( *r=='d'); }

  template<typename R>
  inline static R parse_int( R r)
	{
		r = parse_typename<'i'>(r);
    if ( !r )
      throw unexpected_end_fragment();
    return parse_number(r);
  }

  template<typename R>
  inline static R parse_double( R r)
  {
		r = parse_typename<'d'>(r);
    if ( !r )
      throw unexpected_end_fragment();
    return parse_number(r);
  }

  template<typename R>
  inline static R parse_float( R r) { return parse_double(r); }

	template<typename R>
  inline static bool is_null( R r ) {  return r && *r=='N'; }

  template<typename R>
  inline static R parse_null( R r)
  {
    if (!r) throw unexpected_end_fragment(r);
    if ( *r!='N' ) throw expected_of('N', r );
    return ++r;
  }

  template<typename R>
  inline static bool is_bool( R r )  { return r && *r=='b'; }

  template<typename R>
  inline static R parse_bool( R r )
  {
		r = parse_typename<'b'>(r);
    if ( *r!='1' && *r!='0') throw expected_of("1 or 0 for boolean", r);
    return ++r;
  }

	template<typename R>
  inline static R unserialize_bool(bool& v, R r)
	{
		v = *r!='0';
    return ++r;
  };

  template<typename R>
  inline static bool is_string( R r )  { return r && *r=='s'; }

  template<typename R>
  inline static R parse_string( R r )
  {
    size_t size = 0;
    r = parse_typename<'s'>(r, &size);

    if ( *r !='"' ) throw expected_of('"', r);

    if ( !(++r) ) throw unexpected_end_fragment();

		r.advance(size);

    if ( !r ) throw unexpected_end_fragment();

    if ( *r!='"' ) throw expected_of('"', r);

    return ++r;
  }


  template<typename R>
  inline static bool is_object( R r ) { return r && *r=='a'; }

  template<typename R>
  inline static bool is_array( R r )  { return r && *r=='a'; }

  template<typename R>
  inline static R parse_value( R r )
  {
    if ( is_null(r) )
      return parse_null(r);
    else if (is_bool(r) )
      return parse_bool(r);
    else if (is_int(r) )
      return parse_int(r);
    else if (is_float(r) )
      return parse_float(r);
    else if (is_string(r) )
      return parse_string(r);
    else if (is_object(r) )
      return parse_object(r);
    else if (is_array(r) )
      return parse_array(r);
    throw deserialize_error( "syntax error", r );
  }

  template<typename R>
  inline static R parse_member( R r )
  {
    r = parse_value(r);
    if ( !r ) throw unexpected_end_fragment();
    r = parse_space(r);
    if ( !r ) throw unexpected_end_fragment();
    if ( *r == ';' ) ++r;
    if ( !r ) throw unexpected_end_fragment();
    r = parse_space(r);
    if ( !r ) throw unexpected_end_fragment();
    r = parse_value(r);
    r = parse_space(r);
    if ( !r ) throw unexpected_end_fragment();
    if ( *r == ';' ) ++r;
    return r;
  }

  template<typename R>
  inline static R parse_object( R r )
  {
		size_t s = 0;
    r = parse_typename<'a'>(r, &s);
    r = parse_space(r);
    if ( !r ) throw unexpected_end_fragment();

    if ( *r!='{' ) throw expected_of('{', r );
		++r;
    r = parse_space(r);
    if ( !r ) throw unexpected_end_fragment();

    for ( ; r && *r!='}'; )
    {
      r = parse_space(r);
      r = parse_member(r);
      r = parse_space(r);
    }
		if ( !r ) throw unexpected_end_fragment();
    if ( *r!='}') throw expected_of("}", r );
    ++r;
    return r;
  }

  template<typename R>
  inline static R parse_array( R r )
  {
    return parse_object(r);
  }

  // s:12:
  // a:3:
  // i:
  template<char t, typename R>
  inline static R parse_typename( R r, size_t* size = 0 )
  {
    if (!r) throw unexpected_end_fragment();

    if (*r!=t)
      throw expected_of( t, r );

    if ( !(++r) ) throw unexpected_end_fragment();
    if ( *r!=':' ) throw expected_of(':', r);
    if ( !(++r) ) throw unexpected_end_fragment();

    if ( t=='s' || t=='a')
    {
			r = size ? unserialize_integer(*size, r) : parse_number(r);
      if (!r) throw unexpected_end_fragment();
      if ( *r!=':' ) throw expected_of(':', r);
			++r;
    }
    if (!r) throw unexpected_end_fragment();
    return r;
  }



};

}}}

#endif