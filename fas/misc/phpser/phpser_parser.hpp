#ifndef FAS_MISC_PHPSER_SPECIALIZATION_PHPSER_PARSER_HPP
#define FAS_MISC_PHPSER_SPECIALIZATION_PHPSER_PARSER_HPP

#include <fas/misc/json/json_except.hpp>
#include <fas/misc/json/json_parser.hpp>

namespace fas{ namespace phpser{

/**
PHP's Serialization Format

Type    Serialized                  Example
NULL      N;                              N;
Integer   i:$data;                        i:123;
Double    d:$data;                        d:1.23;
Float     d:$data;                        d:1.23;
Boolean   b:$bool_value;                  b:1;
String    s:$data_length:"$data";         s:5:"Hello"
Array     a:$key_count:{$key;$value}      a:1:{i:1;i:2}

$value can be any data type

*/

class parser
{
public:

  template<typename P>
  static P parse_space( P beg, P end ) { return json::parser::parse_space(beg, end); }

  template<typename P>
  static P parse_space( P beg, P end, size_t& p ) { return json::parser::parse_space(beg, end, p);  }

public:

  template<typename P>
  static bool is_null( P beg, P end ) {  return beg!=end && *beg=='N'; }

  template<typename P>
  static P parse_null( P beg, P end ) { size_t p = 0; return parse_null(beg, end, p); }

  template<typename P>
  static P parse_null( P beg, P end, size_t& p )
  {
    if (beg==end)
      throw json::unexpected_end_fragment( );

    if ( *beg!='N' )
      throw json::expected_of("N", p );

    ++beg;
    return beg;
  }

public:

  template<typename P>
  static bool is_bool( P beg, P end )  { return beg!=end && *beg=='b'; }

  template<typename P>
  static P parse_bool( P beg, P end ) { size_t p = 0; return parse_bool(beg, end, p); }

  template<typename P>
  static P parse_bool( P beg, P end, size_t& p )
  {
    if (beg==end)
      throw json::unexpected_end_fragment();

    if (*beg!='b')
      throw json::invalid_json_bool( p );

    ++beg;

    if (beg==end)
      throw json::unexpected_end_fragment();

    if (*beg!=':')
      throw json::expected_of(":", p);

    ++beg;

    if (beg==end)
      throw json::unexpected_end_fragment();

    if ( *beg!='1' || *beg!='0')
      throw json::expected_of("1 or 0 for boolean", p);;

    ++beg;

    return beg;
  }


public:

  template<typename P>
  static bool is_number( P beg, P end )  { return beg!=end && ( *beg=='i' || *beg=='d' ); }

  template<typename P>
  static P parse_number( P beg, P end ) { size_t p = 0; return parse_number(beg, end, p); }

  template<typename P>
  static P parse_number( P beg, P end, size_t& p )
  {
    if (beg==end)
      throw json::unexpected_end_fragment();

    if (*beg!='i' && *beg!='d')
      throw json::invalid_json_number( std::distance(beg, end) );

    ++beg;

    if ( beg==end )
      throw json::unexpected_end_fragment();

    if ( *beg!=':' )
      throw json::expected_of(":", std::distance(beg, end) );

    ++beg;

    if (beg==end)
      throw json::unexpected_end_fragment();

    return json::parser::parse_number(beg, end, p);
  }

public:


  template<typename P>
  static bool is_string( P beg, P end )  { return beg!=end && *beg=='s'; }

  template<char t, typename P>
  static P parse_typename( P beg, P end )
  {
    return parse_typename<t>(beg, end, static_cast<int*>(0) );
  }


  template<typename T, typename P>
  inline static P unserialize_integer( T& v, P beg, P end )
  {
    return json::parser::unserialize_integer(v, beg, end);
  }

  // s:12:
  // a:3:
  // i:
  template<char t, typename P, typename S>
  static P parse_typename( P beg, P end, S* size )
  {
    if (beg==end)
      throw json::unexpected_end_fragment();

    if (*beg!=t)
      throw json::expected_of( std::string()+t, std::distance(beg, end));

    if ( t!= 'N')
    {
      ++beg;

      if ( beg==end )
        throw json::unexpected_end_fragment();

      if ( *beg!=':' )
        throw json::expected_of(":", std::distance(beg, end));

      ++beg;

      if (beg==end)
        throw json::unexpected_end_fragment();

      if ( t=='s' || t=='a')
      {
        if (size)
          beg = unserialize_integer(*size, beg, end);
        else
          beg = json::parser::parse_number(beg, end/*, p*/);

        if (beg==end)
          throw json::unexpected_end_fragment();

        if ( *(beg++)!=':' )
          throw json::expected_of(":", std::distance(beg, end));
      }
    }

    if (beg==end)
      throw json::unexpected_end_fragment();

    return beg;
  }

  template<typename P>
  static P parse_string( P beg, P end)
  {
    size_t size = 0;
    return parse_string(beg, end, size);
  }


  template<typename P>
  static P parse_string( P beg, P end, size_t& size )
  {
    beg = parse_typename<'s'>(beg, end, &size);

    if ( *(beg++)!='"' )
      throw json::expected_of("\"", std::distance(beg, end));

    if (beg==end)
      throw json::unexpected_end_fragment();

    if ( beg + size >= end)
      throw json::unexpected_end_fragment();

    std::advance(beg, size);

    if ( *(beg++)!='"' )
      throw json::expected_of("\"", std::distance(beg, end));

    return beg;
  }

  template<typename P>
  static bool is_object( P beg, P end )
  {
    return beg!=end && *beg=='a';
  }

  template<typename P>
  static bool is_array( P beg, P end )
  {
    return beg!=end && *beg=='a';
  }

  template<typename P>
  static P parse_value( P beg, P end )
  {
    if ( is_null(beg, end) )
      return parse_null(beg, end);
    else if (is_bool(beg, end) )
      return parse_bool(beg, end);
    else if (is_number(beg, end) )
      return parse_number(beg, end);
    else if (is_string(beg, end) )
      return parse_string(beg, end);
    else if (is_object(beg, end) )
      return parse_object(beg, end);
    else if (is_array(beg, end) )
      return parse_array(beg, end);
    throw json::invalid_json( std::distance(beg, end) );
  }

  template<typename P>
  static P parse_member( P beg, P end )
  {
    beg = parse_value(beg, end);
    if ( beg==end ) throw json::unexpected_end_fragment();
    beg = parse_space(beg, end);
    if ( beg==end ) throw json::unexpected_end_fragment();
    if ( *beg == ';' ) ++beg;
    // if ( *(beg++)!=';' ) throw json::expected_of(";", std::distance(beg, end) );
    if ( beg==end ) throw json::unexpected_end_fragment();
    beg = parse_space(beg, end);
    if ( beg==end ) throw json::unexpected_end_fragment();
    beg = parse_value(beg, end);
    beg = parse_space(beg, end);
    if ( beg==end ) throw json::unexpected_end_fragment();
    if ( *beg == ';' ) ++beg;
    //if ( *(beg++)!=';' ) throw json::expected_of(";", std::distance(beg, end) );
    return beg;
  }

  template<typename P>
  static P parse_object( P beg, P end )
  {
    size_t size = 0;
    return parse_object( beg, end, size );
  }


  template<typename P>
  static P parse_object( P beg, P end, size_t& size )
  {
    beg = parse_typename<'a'>(beg, end, &size);
    beg = parse_space(beg, end);
    if ( beg==end ) throw json::unexpected_end_fragment();

    if ( *(beg++)!='{' )
      throw json::expected_of("{", std::distance(beg, end));
    beg = parse_space(beg, end);
    if ( beg==end ) throw json::unexpected_end_fragment();

    for ( /*++beg*/; beg!=end && *beg!='}'; )
    {
      beg = parse_space(beg, end);
      beg = parse_member(beg, end);
      beg = parse_space(beg, end);
      /*
      if (beg == end || ( *beg!=';' && *beg!='}' ) )
        throw json::expected_of("}", std::distance(beg, end) );

      if ( *beg==';' )  ++beg;
      */
    }
    if (beg == end || *beg!='}') 
      throw json::expected_of("}", std::distance(beg, end) );
    ++beg;
    return beg;
  }

  template<typename P>
  static P parse_array( P beg, P end )
  {
    size_t size = 0;
    return parse_array(beg, end, size );
  }

  template<typename P>
  static P parse_array( P beg, P end, size_t& size )
  {
    return parse_object(beg, end, size );
  }

};


}}

#endif
