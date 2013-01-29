#pragma once

namespace fas{ namespace json{

template<typename C>
struct paired_array_base;

template<typename C>
struct paired_array;

template< typename T, char L, char R >
class paired_array_serializer : public serializerA< T, L, R > {
	typedef T array_type;
	typedef typename array_type::target_container target_container;

	typedef serializerA< T, L, R > parent;

public:
	template<typename P>
	P operator()( target_container& t,  P beg, P end) {
		P nbeg = beg;
		if (*nbeg=='[') {
			nbeg++;
			if (*nbeg==']') {
				nbeg++;
				t = target_container();
			    return nbeg;
			}
		}
		return parent::operator()(t,beg,end);
	}

	template<typename P>
	P operator()( const target_container& t, P end) {
		return parent::operator()(t,end);
	}

};

template<typename JK, typename JV>
struct paired_array_base< std::pair<JK, JV> > {
  typedef paired_array_base< std::pair<JK, JV> > self;
  typedef std::vector< aj::value<std::pair<JK, JV> > > json_container;
  typedef typename JK::target key;
  typedef typename JV::target value;
  typedef std::pair<key, value> pair_type;
  typedef pair< JK , JV > json_value;
  typedef typename json_value::target target;
  typedef std::vector< std::pair<key, value> > target_container;
  typedef paired_array_serializer< self, '{', '}' > serializer;

  typedef std::insert_iterator<target_container> inserter_iterator;
  static inserter_iterator inserter(target_container& t) { return std::inserter(t, t.begin()); }
};

template<typename C>
struct paired_array: public paired_array_base<C>
{
  typedef paired_array_base<C> base;
  typedef typename base::target_container target;
  typedef typename base::serializer serializer;
};

}}


template<typename T, typename K, typename V>
void push_pair(T &target, K&& k, V&& v) {
	target.push_back(std::pair<K,V>(k,v));
}
