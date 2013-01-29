#pragma once

#include <fas/system/system.hpp>
#include <fas/adv/json_rpc3/method.hpp>
#include "common/aspect/rn_json_aspect.hpp"
#include "common/aspect/json_aspect.hpp"
#include "common/ap_type_list_v.hpp"
#include "common/aj_int_quoted.hpp"
#include "strvalue.hpp"
#include "aj_pairvector.hpp"

#include <unordered_map>


namespace methods {
	struct _rep_setstate_ {};
	struct _set_ {};
	struct _set_range_ {};
	struct _del_ {};
}

namespace aj = ::fas::json;
namespace ap = ::fas::pattern;

#define NAME(X) struct n_##X { const char* operator()() const{ return #X;} }

NAME(inc);
NAME(key);
NAME(set);
NAME(def);
NAME(from);
NAME(to);
NAME(limit);
NAME(skip);
NAME(id);
NAME(state);

typedef std::vector<std::pair<std::string,std::string> > valmap2raw;
typedef aj::paired_array<std::pair<aj::value<std::string>,aj::raw_value<std::string> > > valmap2raw_json;

typedef std::vector<std::pair<std::string,std::string> > valmap2str;
typedef aj::paired_array<std::pair<aj::value<std::string>,aj::strvalue<std::string> > > valmap2str_json;

typedef std::unordered_map<std::string,std::string> valmap2str_map;
typedef aj::array<std::unordered_map<aj::value<std::string>,aj::strvalue<std::string> > > valmap2str_map_json;

typedef std::vector<std::pair<std::string,long long int> > valmap2int;
typedef aj::paired_array<std::pair<aj::value<std::string>,aj::value<long long int> > > valmap2int_json;

typedef std::unordered_map<std::string,long long int> valmap2int_map;
typedef aj::array<std::unordered_map<aj::value<std::string>,aj::value<long long int> > > valmap2int_map_json;

typedef std::vector<std::pair<std::string,bool> > valmap2bool;
typedef aj::paired_array<std::pair<aj::value<std::string>,aj::value<bool> > > valmap2bool_json;

//typedef std::vector<std::pair<std::string,bool> > xvalmap2bool;
//typedef aj::paired_array<std::vector<std::pair<aj::value<std::string>,aj::value<bool> > > > xvalmap2bool_json;

typedef std::vector<std::string> vallist;
typedef aj::array<std::vector<aj::value<std::string> > > vallist_json;


namespace request {
	struct rep_setstate {
		std::string id;
		std::string state;

		rep_setstate() : state("start") {
		}
		rep_setstate(std::string id,std::string state) : id(id),state(state) {
		}
	};
	typedef aj::object<
		rep_setstate,
		ap::type_list_v<
			aj::member<n_id, rep_setstate, std::string, &rep_setstate::id>
			,aj::member<n_state, rep_setstate, std::string, &rep_setstate::state>
		>::type
	> rep_setstate_json;

	struct inc_add {
		valmap2int inc;
		valmap2int_map def;
	};
	typedef aj::object<
		inc_add,
		ap::type_list_v<
			aj::member<n_inc, inc_add, valmap2int, &inc_add::inc, valmap2int_json>
			,aj::member<n_def, inc_add, valmap2int_map, &inc_add::def, valmap2int_map_json>
		>::type
	> inc_add_json;

	struct update_packed {
		std::string key;
		valmap2int inc;
		valmap2str set;
		valmap2str_map def;
	};
	typedef aj::object<
		update_packed,
		ap::type_list_v<
			aj::member<n_key, update_packed, std::string, &update_packed::key>
			,aj::member<n_inc, update_packed, valmap2int, &update_packed::inc, valmap2int_json>
			,aj::member<n_set, update_packed, valmap2str, &update_packed::set, valmap2str_json>
			,aj::member<n_def, update_packed, valmap2str_map, &update_packed::def, valmap2str_map_json>
		>::type
	> update_packed_json;

	struct get_range {
		std::string from;
		std::string to;
		int limit;
		int skip;

		get_range() : limit(1000),skip(0){
		}
	};

	typedef aj::object<
		get_range,
		ap::type_list_v<
			aj::member<n_from, get_range, std::string, &get_range::from>
			,aj::member<n_to, get_range, std::string, &get_range::to>
			,aj::member<n_limit, get_range, int, &get_range::limit>
			,aj::member<n_skip, get_range, int, &get_range::skip>
		>::type
	> get_range_json;

}
