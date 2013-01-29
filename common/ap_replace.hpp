#pragma once

#include <fas/pattern/type_list.hpp>

namespace fas {
namespace pattern {

/**
 * операция: удалить из списка типов L все вхождения From и добавить To
 */
template<typename L, typename From, typename To>
struct replace {
	typedef typename ap::organize< ap::type_list<
		typename ap::erase< From, L >::type
		,To
	> >::type type;
};

/**
 * операция: добавит в список типов L тип To
 */
template<typename L, typename To>
struct insert {
	typedef typename ap::organize< ap::type_list<
		L
		,To
	> >::type type;
};

}
}
