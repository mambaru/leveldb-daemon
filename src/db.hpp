#pragma once



volatile bool is_running;

#if 1==1
leveldb::DB* db_global;
inline leveldb::DB* getDb() {
	return db_global;
}

leveldb::DB* create_db(std::string name, size_t buffer=512,size_t cache=128) {
	leveldb::DB* db;
	leveldb::Options options;
	options.max_open_files = 1024*100;
	options.filter_policy = leveldb::NewBloomFilterPolicy(20);
	options.write_buffer_size = buffer*1024*1024;
	options.block_cache = leveldb::NewLRUCache(cache*1024*1024);
	options.create_if_missing = true;
	leveldb::Status status = leveldb::DB::Open(options, name, &db);
	if (!status.ok()) throw new std::runtime_error("leveldb file opening failed");
	return db;
}

#else

#include <tbb/concurrent_unordered_map.h>

class ldb_mem_Iterator : public leveldb::Iterator {
	multispinlock &mtx;
	std::map<std::string,std::string> &data;
	std::map<std::string,std::string>::iterator it;
public:
	ldb_mem_Iterator(multispinlock &mtx,std::map<std::string,std::string> &data) : mtx(mtx),data(data) {
		mtx.lock_r();
		it = data.begin();
	}
	virtual ~ldb_mem_Iterator() {
		mtx.unlock_r();
	}

	virtual bool Valid() const { return it!=data.end(); }

	virtual void SeekToFirst() { it = data.begin(); }

	virtual void SeekToLast() { it = data.end(); }

	virtual void Seek(const leveldb::Slice& target) {
		it = data.lower_bound(target.ToString());
	}

	virtual void Next() { it++; }

	virtual void Prev() { it--; }

	virtual leveldb::Slice key() const { return it->first; }

	virtual leveldb::Slice value() const { return it->second; }

	virtual leveldb::Status status() const { return leveldb::Status::OK(); }
};


class ldb_mem_db {
	multispinlock mtx;
	std::map<std::string,std::string> data;

public:
	  leveldb::Status Get(const leveldb::ReadOptions&, const leveldb::Slice& key, std::string* value) {
		  std::lock_guard<readlock> lck(readlock(&mtx));
		  value = new std::string(data[key.ToString()]);
		  return leveldb::Status::OK();
	  }
	  leveldb::Status Put(const leveldb::WriteOptions& ,const leveldb::Slice& key,const leveldb::Slice& value) {
		  std::lock_guard<multispinlock> lck(mtx);
		  data[key.ToString()] = value.ToString();
		  return leveldb::Status::OK();
	  }
	  leveldb::Iterator* NewIterator(const leveldb::ReadOptions&) {
		  return new ldb_mem_Iterator(mtx,data);
	  }
	  leveldb::Status Delete(const leveldb::WriteOptions&, const leveldb::Slice& key) {
		  std::lock_guard<multispinlock> lck(mtx);
		  data.erase(key.ToString());
		  return leveldb::Status::OK();
	  }



};

leveldb::DB* tmp;
ldb_mem_db* db_global;
inline ldb_mem_db* getDb() {
	return db_global;
}

ldb_mem_db* create_db(std::string name, size_t buffer=512,size_t cache=128) {
	return new ldb_mem_db();
}


#endif

#include "common/lock_atomic.hpp"
struct LockManager {
	longspinlock locks[1024];
	longspinlock& get(const std::string &str) {
		std::hash<std::string> h;
		return get( h(str) );
	}

	longspinlock& get(size_t n) {
		return locks[n%1024];
	}
};
LockManager lockmanager;
inline LockManager* getLockmanager() {
	return &lockmanager;
}
