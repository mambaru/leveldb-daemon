/*
 * lrucache.hpp
 *
 *  http://www.bottlenose.demon.co.uk/article/lru.htm
 */

#ifndef LRUCACHE_HPP_
#define LRUCACHE_HPP_

#include <boost/bimap.hpp>
#include <boost/bimap/list_of.hpp>
#include <boost/bimap/set_of.hpp>
#include <boost/function.hpp>
#include <boost/thread.hpp>

// Class providing fixed-size (by number of records)
// LRU-replacement cache of a function with signature
// V f(K)
template <typename K,typename V> class lrucache
{
public:
	lrucache(const lrucache<K,V>& other):_capacity(other._capacity),_cache(other._cache) { }

	typedef int dummy_type;

	// Bimap with key access on left view, key access
	// history on right view, and associated value.
	typedef boost::bimaps::bimap<
			boost::bimaps::set_of<K>,
			boost::bimaps::list_of<dummy_type>,
			boost::bimaps::with_info<V>
	> cache_type;

	// Constuctor specifies the cached function and
	// the maximum number of records to be stored.
	lrucache( size_t c ) :_capacity(c) {
		assert(_capacity!=0);
	}

	void set_capacity(size_t c) {
		_capacity(c);
	}

	// Obtain value of the cached function for k
	bool get(const K& k, V& v) {
		mtx.lock();
		const typename cache_type::left_iterator it =_cache.left.find(k);
		if (it==_cache.left.end()) {
			mtx.unlock();
			return false;
		} else {
			// We do have it:
			// Update the access record view.
			_cache.right.relocate(
					_cache.right.end(),
					_cache.project_right(it)
			);
			v = it->info;
			mtx.unlock();
			return true;
		}
	}

	// Obtain the cached keys, most recently used element
	// at head, least recently used at tail.
	// This method is provided purely to support testing.
	template <typename IT> void get_keys(IT dst) const {
		typename cache_type::right_const_reverse_iterator src =_cache.right.rbegin();
		while (src!=_cache.right.rend()) {
			*dst++=(*src++).second;
		}
	}

	int size() {
		return _cache.size();
	}

	void insert(const K& k,const V& v) {
		mtx.lock();
		assert(_cache.size()<=_capacity);

		const typename cache_type::left_iterator it =_cache.left.find(k);
		if (it!=_cache.left.end()) {
			_cache.left.erase(k);
		}
		// If necessary, make space
		if (_cache.size()==_capacity) {
			// by purging the least-recently-used element
			_cache.right.erase(_cache.right.begin());
		}

		// Create a new record from the key, a dummy and the value
		_cache.insert(
				typename cache_type::value_type(
						k,0,v
				)
		);
		mtx.unlock();
	}

private:
	boost::mutex mtx;
	const size_t _capacity;
	cache_type _cache;
};



template <typename K,typename V> class lrucache_functional : public lrucache<K,V> {
public:
	lrucache_functional(const lrucache_functional<K,V>& other):_fn(other._fn),lrucache<K,V>(other) {
	}
	lrucache_functional(const boost::function<V(const K&)>& f, size_t c) :_fn(f) , lrucache<K,V>(c) {
	}

	// Obtain value of the cached function for k
	V operator()(const K& k) {
		V v;
		if (!get(k,v)) {
			v=_fn(k);
			insert(k,v);
		}
		return v;
	}

private:
	const boost::function<V(const K&)> _fn;
};

#endif /* LRUCACHE_HPP_ */
