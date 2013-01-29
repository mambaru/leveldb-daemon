//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_ADV_JSON_RPC_TAGS_HPP
#define FAS_ADV_JSON_RPC_TAGS_HPP

namespace fas{ namespace adv{ namespace json_rpc{

struct _input_{};
struct _output_{};

struct _incoming_{};
struct _outgoing_{};

struct _outgoing_object_{};

struct _invoke_{};
struct _ginvoke_{};
// struct _writer_{};

struct _proxy_{};
struct _gcall_{};

struct _unknown_method_{};
struct _invalid_json_{};
struct _result_{};

	
struct _lost_call_{};
}}}

#endif
