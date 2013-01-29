#include <fas/pattern/type_list.hpp>
#include <fas/misc/json.hpp>

namespace ap = ::fas::pattern;
namespace aj= ::fas::json;

namespace fas { namespace pattern {

template<typename TLI, typename left_type, typename FT>
struct type_list_inst_getter {
	static typename FT::type & get(const FT &fld, TLI&data) {
		return data.right_value.get(fld);
	}
};
template<typename TLI, typename FT>
struct type_list_inst_getter<TLI, FT, FT> {
	static typename FT::type & get(const FT &fld, TLI&data) {
		return data.value;
	}
};

/*
template<bool b, typename FT, typename TLI>
struct type_list_inst_getter {
	FT& get(const FT &fld, const TLI& data) {
		return data.right_value.get(fld);
	}
};
template<true, typename FT, typename TLI>
struct type_list_inst_getter {
	FT& get(const FT &fld, const TLI& data) {
		return data.right_value.get(fld);
	}
};
*/

/**
 * промежуточное представление анкеты. для каждого field_... создаётся экземпляр field_something::type
 */
template< typename T >
struct type_list_inst {
	typedef typename T::left_type left_type ;
	typedef typename T::left_type::type value_type ;
	typedef typename T::left_type::json_type json_type;

	value_type value;
	type_list_inst<typename T::right_type> right_value;

	template<typename FT>
	typename FT::type & get(const FT &fld) {
		return type_list_inst_getter<type_list_inst<T>, left_type, FT>::get(fld,*this);
/*		if (std::is_same<FT,left_type>::value) {
			return (*this);
		} else {
			return right_value.template get(fld);
		}*/
	}

};

/*
template<typename TLI>
value_type& get(const left_type &p) {
	return value;
}*/



template<>
struct type_list_inst<ap::empty_type> {
	ap::empty_type value;
	ap::empty_type right_value;
	typedef ap::empty_type left_type ;
	typedef ap::empty_type json_type ;
	typedef ap::empty_type value_type ;

/*	template<typename FT>
	typename FT::type & get(const FT &fld) {
	}*/
};


/**
 * промежуточное представление запроса для расширенных полей. для каждого field_... создаётся экземпляр std::vector<field_something::type>
 */
template< typename T >
struct type_list_inst_arr {
	typedef typename T::left_type left_type ;
	typedef typename T::left_type::type value_type ;
	typedef aj::array<std::vector<typename T::left_type::json_type> > json_type;

	std::vector<value_type> value;
	type_list_inst_arr<typename T::right_type> right_value;
};

template<>
struct type_list_inst_arr<ap::empty_type> {
	ap::empty_type value;
	ap::empty_type right_value;
	typedef ap::empty_type left_type ;
	typedef ap::empty_type json_type ;
	typedef ap::empty_type value_type ;
};


}}
