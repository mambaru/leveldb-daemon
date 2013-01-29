#ifndef FAS_MISC_JSON_SPECIALIZATION_JSON_SET_HPP
#define FAS_MISC_JSON_SPECIALIZATION_JSON_SET_HPP

//#include "json_parser.hpp"
#include <list>

namespace fas{ namespace json{

template< typename T, typename L>
struct set_enumerator
{
	typedef T target;
	typedef typename ap::type_list_traits<L>::type enum_list;
	typedef serializerT< set_enumerator<T, enum_list> > serializer;
};

/// /////////////////////////////////////////////////////////////////

template<typename T, typename L>
class serializerT< set_enumerator<T, L> >
{
	typedef typename set_enumerator<T, L>::enum_list enum_list;
public:
	template<typename P>
	P operator()( const T& v, P end)
	{
		*(end++)='[';
		end = this->serialize(v, enum_list(), end,true);
		*(end++)=']';
		return end;
	}

	template<typename LL, typename RR, typename P>
	P serialize( const T& v, ap::type_list<LL, RR>, P end, bool isFirst)
	{
		if (LL::value & v)
		{
			if (!isFirst) *(end++)=',';
			*(end++)='"';
			const char* val = LL()();
			for ( ; *val!='\0' ; ++val) *(end++) = *val;
			*(end++)='"';
			return this->serialize(v, RR(), end, false);
		}
		else
		{
			return this->serialize(v, RR(), end, isFirst);
		}
	}

	template<typename P>
	P serialize( const T& v, ap::empty_type, P end, bool isFirst)
	{
		return end;
	}

	template<typename P>
	P operator() ( T& v, P beg, P end )
	{
		for ( ; beg!=end && *beg<=' '; ++beg);
		if (beg==end) throw unexpected_end_fragment();
		if (*beg =='"') {
			++beg;
			if (beg==end) throw unexpected_end_fragment();
			P first = beg;
			for ( ; beg!=end && *beg!='"'; ++beg);
			if (beg==end) throw unexpected_end_fragment();
			if (*beg!='"') throw expected_of("\"");
			this->deserialize(v, enum_list(), first, beg);
			++beg;
		} else if (*beg == '[') {
			++beg;
			if (beg==end) throw unexpected_end_fragment();
			beg = this->deserialize_arr(v, enum_list(), beg, end);
			if (beg==end) throw unexpected_end_fragment();
			if (*beg!=']') throw expected_of("]");
			++beg;
		} else throw expected_of("\"");
		return beg;
	}

	template<typename LL, typename RR, typename P>
	P deserialize_one( T& v, ap::type_list<LL, RR>, P beg, P end)
	{
		P first = beg;
		const char *pstr = LL()();
		for ( ; beg!=end && *pstr!='\0' && *pstr==*beg; ++beg, ++pstr);
		if ( beg==end && *pstr=='\0') {
			v = (T)(v|LL::value);
			return end;
		} else if (*beg==',' && *pstr=='\0') {
			beg++;
			v = (T)(v|LL::value);
			return beg;
		} else {
			return deserialize_one(v, RR(), first, end);
		}
	}

	template<typename P>
	P deserialize_one( T& , ap::empty_type,P beg,P end)
	{
		return beg;
	}

public:
  /**
    * для десериализации строки вида "One,Two,Three"
    */
	template<typename LL, typename RR, typename P>
	void deserialize( T& v, ap::type_list<LL, RR> tl, P beg, P end) {
		v = T();
		P beg_prev = end;
		while (beg != beg_prev && beg!=end) {
			beg_prev = beg;
			beg = deserialize_one(v, tl, beg, end);
		}
	}
  /**
	* для десериализации массива вида ["One","Two","Three"]
	*/
	template<typename LL, typename RR, typename P>
	P deserialize_arr( T& v, ap::type_list<LL, RR> tl, P beg, P end) {
		v = T();
		char lastChar = ' ';
		while (*beg=='"') {
			++beg;
			if (beg==end) throw unexpected_end_fragment();
			P first = beg;
			for ( ; beg!=end && *beg!='"'; ++beg);
			if (beg==end) throw unexpected_end_fragment();
			if (*beg!='"') throw expected_of("\"");
			this->deserialize_one(v, enum_list(), first, beg);
			beg++;
			lastChar = *beg;
			if (lastChar==',') beg++;
		}
		if (lastChar==',') throw unexpected_end_fragment();
		return beg;
	}

};






template<typename J>
struct array_base< std::list<J> >
{
  typedef std::list<J> json_container;
  typedef J json_value;
  typedef typename json_value::target target;
  typedef std::list<target> target_container;
  typedef serializerT< array< json_container > > serializer;
  typedef std::back_insert_iterator<target_container> inserter_iterator;
  static inserter_iterator inserter(target_container& t) { return std::back_inserter(t); }
};


}}

#endif
