#ifndef FAS_MISC_SERIALIZATION_EXCEPTIONS_HPP
#define FAS_MISC_SERIALIZATION_EXCEPTIONS_HPP

#include <stdexcept>

namespace fas{ namespace serialization{

class serialization_error_base
  : public std::runtime_error
{
  std::ptrdiff_t _tail_of;
public:
  serialization_error_base(const std::string& msg, std::ptrdiff_t tail_of = 0 ): std::runtime_error(msg), _tail_of(tail_of) 
	{
	}
  size_t tail_of() const { return _tail_of; }

  template<typename P>
  std::string message( P beg, P end ) const
  {
    
    if (std::distance(beg, end) < _tail_of )
      return this->what();
    std::string msg; 
    msg = this->what();
    msg += std::string(": ") + std::string(beg, end - _tail_of ) + ">>>" + std::string( end - _tail_of, end );
    return msg;
  };
};


class serialization_error
		: public serialization_error_base
{
public:

  explicit serialization_error(const std::string& msg)
    : serialization_error_base(msg)
  {
  };

	template<typename R>
  serialization_error(const std::string& msg, R r)
    : serialization_error_base(msg, r.distance() )
  {
		
  };

};


class invalid_conversion
  : public serialization_error
{
public:
  invalid_conversion( )
    : serialization_error( "invalid conversion") {}

	template<typename R>
  invalid_conversion( R r )
    : serialization_error( "invalid conversion", r) {}

  invalid_conversion( const std::string& from, const std::string& to )
    : serialization_error( std::string("invalid conversion from '") + from + std::string("' to '") + to ) {}

	template<typename R>
  invalid_conversion( const std::string& from, const std::string& to, R r )
    : serialization_error( std::string("invalid conversion from '") + from + std::string("' to '") + to, r ) {}
};

class not_supported
  : public serialization_error
{ 
public:
  not_supported()
    : serialization_error("not supported")  {}

	template<typename R>
  not_supported(R r)
    : serialization_error("not supported", r)  {}
};

class deserialize_error
  : public serialization_error
{ 
public:

  deserialize_error(const std::string& str = "deserialize error")
    : serialization_error(str)  {}

/*	template<typename R>
  deserialize_error(R r)
    : serialization_error("deserialize error", r)  {}
*/

	template<typename R>
  deserialize_error(const std::string& str , R r)
    : serialization_error(str, r)  {}
};

class serialize_error
  : public serialization_error
{ 
public:
  serialize_error()
    : serialization_error("serialize error") {}

  template<typename R>
  serialize_error(R r)
    : serialization_error("serialize error", r) {}
};


class invalid_null
  : public deserialize_error
{ 
public:
  invalid_null()
    : deserialize_error("invalid null")  {}

	template<typename R>
  invalid_null(R r)
    : deserialize_error("invalid null", r)  {}
};


class invalid_number
  : public deserialize_error
{ 
public:

  invalid_number()
    : deserialize_error("invalid number")  {}

	template<typename R>
  invalid_number(R r)
    : deserialize_error("invalid number", r)  {}
};

class invalid_bool
  : public deserialize_error
{ 
public:
  invalid_bool()
    : deserialize_error("invalid bool")  {}

	template<typename R>
  invalid_bool(R r)
    : deserialize_error("invalid bool", r)  {}
};

class invalid_string
  : public deserialize_error
{ 
public:
  invalid_string()
    : deserialize_error("invalid string")  {}

	template<typename R>
  invalid_string(R r)
    : deserialize_error("invalid string", r)  {}
};

class invalid_member
  : public deserialize_error
{ 
public:
  invalid_member()
    : deserialize_error("invalid member")  {}

	template<typename R>
  invalid_member(R r)
    : deserialize_error("invalid member", r)  {}
};

class invalid_object
  : public deserialize_error
{ 
public:
  invalid_object()
    : deserialize_error("invalid object")  {}
	
	template<typename R>
  invalid_object(R r)
    : deserialize_error("invalid object", r)  {}
};

class invalid_array
  : public deserialize_error
{ 
public:

  invalid_array()
    : deserialize_error("invalid array")  {}

	template<typename R>
  invalid_array(R r)
    : deserialize_error("invalid array", r)  {}
};

/*
class invalid_hash
  : public deserialize_error
{ 
public:
  invalid_hash(size_t tail_of = 0)
    : deserialize_error("invalid hash", tail_of)  {}
};*/


class unexpected_end_fragment
  : public deserialize_error
{
public:
  unexpected_end_fragment()
    : deserialize_error( "unexpected end of ragment") {}

  unexpected_end_fragment(const std::string& str)
    : deserialize_error( std::string("unexpected end of ragment: ") + str) {}

	template<typename R>
  unexpected_end_fragment(R r)
    : deserialize_error( "unexpected end of ragment", r) {}

	template<typename R>
  unexpected_end_fragment(const std::string& str, R r)
    : deserialize_error( std::string("unexpected end of ragment: ") + str, r ) {}
};

class expected_of
  : public deserialize_error
{
public:
  expected_of(char ch)
    : deserialize_error( std::string("expected of '") + ch + std::string("'")) {}

  expected_of(const std::string& str)
    : deserialize_error( std::string("expected of '") + str + std::string("'")) {}

  template<typename R>
  expected_of(const std::string& str, R r)
    : deserialize_error( std::string("expected of '") + str + std::string("'"), r) {}

  template<typename R>
  expected_of(char ch, R r)
    : deserialize_error( std::string("expected of '") + ch + std::string("'"), r) {}

  
};

}}

#endif