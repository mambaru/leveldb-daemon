namespace ap = ::fas::pattern;


namespace fas { namespace json {

/**
 * сериалайзер для type_list_inst (промежуточное представление анкеты)
 */
template<typename N>
struct value< fas::pattern::type_list_inst<N> >
{
  typedef fas::pattern::type_list_inst<N> target;
  typedef serializerT< fas::pattern::type_list_inst<N> > serializer;
};
}}

namespace fas{ namespace json{

/// ////////////////////////////////////////////////////////////

template<typename L>
class serializerTT {
public:

	serializerTT(){}

	/** serialization:
	 * end - back inserter iterator
	 */
	template<typename P>
	P operator()( const L& t, P end)
	{
		*(end++)='{';
		end = serialize_members(t, end, true );
		*(end++)='}';
		return end;
	}

	/** deserialization:
	 * beg, end - forward iterator
	 */
	template<typename P>
	P operator()( L& t, P beg, P end)
	{

		if ( parser::is_null(beg, end) )
		{
			return parser::parse_null(beg, end);
		}

		if (beg==end)
			throw unexpected_end_fragment();

		if (beg+2 == end && *beg=='[' && *(beg+1)==']') return end;

		if ( *(beg++) != '{' )
			throw expected_of("{", std::distance(beg, end) );

		beg = parser::parse_space(beg, end);
		if ( beg==end )
			throw unexpected_end_fragment();

		if ( *beg != '}')
		{
			beg = unserialize_members(t, t, beg, end );
			if ( beg==end ) throw unexpected_end_fragment();
			beg = parser::parse_space(beg, end);
			if ( beg==end ) throw unexpected_end_fragment();
		}

		if (beg==end)
			throw unexpected_end_fragment();

		if ( *(beg++) != '}' )
			throw expected_of("}", std::distance(beg, end));

		return beg;
	}

private:

	template<typename P>
	P serialize_members( const ap::empty_type& t, P end,bool First) {
		return end;
	}
	template<typename T,typename P>
	P serialize_members( const T& t, P end, bool First) {
		if (!std::is_base_of<ap::empty_type,typename T::left_type>::value && !First) *(end++)=',';
		end = serialize_member(t, end);
		return serialize_members(t.right_value, end, false );
	}

	template<typename P, typename M >
	P serialize_member_name( const M& m, P end)
	{
		const char* name = M::left_type::get_name();
		*(end++)='"';
		for (;*name!='\0'; ++name) *(end++) = *name;
		*(end++)='"';
		*(end++) = ':';
		return end;
	}

	template<typename P>
	P serialize_member( const ap::type_list_inst< ap::empty_type > & m, P end) {
		return end;
	}
	template<typename P>
	P serialize_member( const ap::type_list_inst_arr< ap::empty_type > & m, P end) {
		return end;
	}

	template<typename P, typename M>
	P serialize_member( const M& m, P end)
	{
		end = serialize_member_name(m, end);

		typedef typename M::json_type::serializer serializer;
		return serializer()( m.value, end );
	}

private:

	template<typename P>
	P unserialize_member_name(const char* name, P beg, P end, bool& unserialized ) {
	  P start = beg;
	  if ( !parser::is_string(beg, end) ) throw expected_of("\"", std::distance(beg, end));
	  ++beg;
	  unserialized = true;
	  for ( ; beg!=end && *name!='\0' && *beg==*name && *beg!='"'; ++name, ++beg) {
		  if (*name!=*beg) {
			  unserialized = false;
			  break;
		  }
	  }

	  if (beg==end) throw unexpected_end_fragment();

	  if (*beg!='"' || *name!='\0') unserialized = false;

	  if ( !unserialized ) return start;
	  ++beg;
	  beg = parser::parse_space(beg, end);
	  if (beg==end) throw unexpected_end_fragment();

	  if (*beg!=':') throw expected_of(":", std::distance(beg, end));
	  ++beg;
	  beg = parser::parse_space(beg, end);

	  if (beg==end) throw unexpected_end_fragment();

	  return beg;
  }

	template<typename P, typename TF>
	P unserialize_member( ap::type_list_inst<ap::empty_type>&, TF &full, P beg, P end, bool &unserialized) {
		unserialized = false;
		return beg;
	}
	template<typename P, typename TF>
	P unserialize_member( ap::type_list_inst_arr<ap::empty_type>&, TF &full, P beg, P end, bool &unserialized) {
		unserialized = false;
		return beg;
	}
	template<typename P, typename TC, typename TF>
	P unserialize_member( TC& current, TF &full, P beg, P end, bool &unserialized) {
		beg = unserialize_member_name(TC::left_type::get_name(), beg, end, unserialized);
		if (!unserialized) {
			return unserialize_member(current.right_value, full, beg, end, unserialized);
		}
		typename TC::json_type::serializer serializer;
		beg = serializer(current.value, beg,end);

		if ( beg==end ) throw unexpected_end_fragment();
		if ( *beg=='}' ) return beg;
		if ( *beg!=',' ) throw expected_of(",", std::distance(beg, end));
		beg++;
		return unserialize_members(current.right_value, full, beg,end);
	}

	//template<typename P, typename TF>
	//P unserialize_members( ap::type_list_inst<ap::empty_type>& current, TF &full, P beg, P end) {
	//}

	template<typename P, typename TC, typename TF>
	P unserialize_members( TC& current, TF &full, P beg, P end) {
	    bool unserialized;
	    beg = unserialize_member( current, full, beg, end, unserialized );
	    if (unserialized) return beg;

	    beg = unserialize_member( full, full, beg, end, unserialized );
	    if (unserialized) return beg;

        beg = parser::parse_member(beg, end);
        beg = parser::parse_space(beg, end);
        if ( beg==end ) throw unexpected_end_fragment();
        if ( *beg=='}' ) return beg;
        if ( *beg!=',' ) throw expected_of(",", std::distance(beg, end));
        ++beg;
        beg = parser::parse_space(beg, end);
        return unserialize_members(current,full,beg,end);
	}
};

template<typename L>
class serializerT< ap::type_list_inst<L> > : public serializerTT< ap::type_list_inst<L> > {
};

template<typename L>
class serializerT< ap::type_list_inst_arr<L> > : public serializerTT< ap::type_list_inst_arr<L> > {
};


}}

