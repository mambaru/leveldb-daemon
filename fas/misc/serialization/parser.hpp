#ifndef FAS_MISC_SEREALIZETION_PARSER_HPP
#define FAS_MISC_SEREALIZETION_PARSER_HPP

#include "exceptions.hpp"

namespace fas{ namespace serialization{

struct no_comment
{
	template<typename R>
  R operator ()(R r)
  {
		return r;
	}

};

struct spacer
{
	template<typename R>
  R operator ()(R r)
  {
		while ( r && *r<=' ') ++r;
		return r;
	}
};


struct c_comment
{
	template<typename R>
  R operator ()(R r)
  {
 		register R in = r;
    if ( r && *r == '/' ) ++r;
		else return in;
    if ( r && *r == '*' ) ++r;
		else return in;

		while( r )
    {
			if ( *(r++) == '*' && *(r++) == '/')
				return r;
			else
				++r;
    }
		return r;
  }
};

class parser
{
public:

	template<typename R>
  inline static R parse_space( R r )
  {
		// Переделать в no_comment по умолчанию
		return parse_space( r, spacer(), c_comment() );
	}

	template<typename R,  typename S, typename C>
  inline static R parse_space( R r, S s, C c)
  {
		register R tmp = r;
		for (;;)
		{
			r = s(r);
			if ( r == tmp ) return r;
			tmp = r;
			r = c(r);
			if ( r == tmp ) return r;
			tmp = r;
		}
		return r;
  }

  template<typename R>
  inline static bool is_number( R r )  { return r && ( *r=='-' || ( *r>='0' && *r<='9' ) ); }

  template<typename R>
  inline static R parse_digits( R r )
  {
    for ( ;r && *r >= '0' && *r<='9'; ++r );
    return r;
  }

  template<typename R>
  inline static R parse_number( R r )
  {
    if ( !r ) throw unexpected_end_fragment();

    if ( r && *r=='-') { ++r; }

    if ( !r ) throw unexpected_end_fragment();

    if ( *r == '0') 
			++r;
    else if ( *r >='1' && *r <='9' )
      r = parse_digits(r);
    else
      throw invalid_number( r );

    if ( r && *r=='.' )
    {
      ++r;
      if ( !r )
        throw unexpected_end_fragment();
      if ( *r >='0' && *r <='9')
        r = parse_digits(r);
      else
        throw invalid_number( r );
    }

    if (r && ( *r=='e' || *r=='E' ) )
    {
      ++r;
      if ( r && (*r=='-' || *r=='+'))  ++r;
      if ( !r ) throw unexpected_end_fragment();

      if ( *r < '0' || *r > '9' ) throw invalid_number( r );
      r = parse_digits(r);
    }
    return r;
  }

  template<typename T, typename R>
  inline static R unserialize_integer( T& v, R r )
  {
		if( !r ) throw unexpected_end_fragment();
    v = 0;
		register bool neg = *r=='-';
   	if ( neg ) ++r;
   	if ( !r || *r < '0' || *r > '9')
    	throw invalid_number( r );
	   // цифры с первым нулем запрещены (напр 001), только 0
  	if (*r=='0')
    	return ++r;

   	for ( ;r; ++r )
   	{
     	if (*r < '0' || *r > '9') 
      	break;
     	v = v*10 + (*r - '0');
   	}

		if (neg) v*=-1;
   	return r;
  };

};


}}

#endif