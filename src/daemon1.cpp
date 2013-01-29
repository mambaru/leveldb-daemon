#include <iostream>
#include "assert.h"
#include "malloc.h"
//#include "LyaTree.hpp"
//#include "fas_server.hpp"

#include "types.h"
#include "hitlist.h"

#include <string>
#include <leveldb/db.h>
#include <leveldb/filter_policy.h>

double microtime() {
    timeval tv;
    gettimeofday(&tv,NULL);
    return tv.tv_sec + tv.tv_usec/1000000.0;
}


int main(int argc, char* argv[])
{
	if (argc != 3) {
		std::cout<<"usage: hitconvert in_file out_file"<<std::endl;
		exit(1);
	}

	std::ifstream inf(argv[1], std::fstream::binary);
	int count = 0;
	uint64_t count2 = 0;
	uint64_t temp = 0;

	leveldb::DB* db;
	leveldb::Options options;
	options.max_open_files = 4096;
	options.filter_policy = leveldb::NewBloomFilterPolicy(20);
	options.write_buffer_size = 1024*1024*1024;
	options.create_if_missing = true;
	leveldb::Status status = leveldb::DB::Open(options, argv[2], &db);

	inf.read(reinterpret_cast<char*>(&temp), sizeof(temp));
	time_t last = time(0) - SEC_IN_MONTH+86400*5;
	double ts = microtime();
	while (inf)
	{
		hit_base oh;
		inf.read(reinterpret_cast<char*>(&oh), sizeof(oh));
		if (oh.src!=0 && oh.src!=(unsigned long)-1 )
		{
			char buf[100];
			memcpy(&buf[0], &oh.dst, 4);
			memcpy(&buf[4], &oh.src, 4);
//			db.set(&buf[0],8,(const char*)&oh,sizeof(oh));

			{
				leveldb::Slice s1 = leveldb::Slice(&buf[0],8);
				leveldb::Slice s2 = leveldb::Slice((const char*)&oh,sizeof(oh));
				db->Put(leveldb::WriteOptions(), s1, s2);
			}

			memcpy(&buf[4], &oh.hittime, 4);
			memcpy(&buf[8], &oh.src, 4);
			int buf2 = 0xFFFFFFFF;
//			db.set(&buf[0],12,(const char*)&buf2,1);

			{
				leveldb::Slice s1 = leveldb::Slice(&buf[0],8);
				leveldb::Slice s2 = leveldb::Slice((const char*)&buf2,1);
				db->Put(leveldb::WriteOptions(), s1, s2);
			}

		}
		if ( ++count % 100000 == 0 ) {
			double ts2 = microtime();
			std::cout<<count<<"\t"<<count2<<"\t"<<oh.src<<"\t"<<oh.dst<<"\tts: " << round(ts2-ts) << "\t" << (100000/(ts2-ts)) << " rec/s" << std::endl;
			ts = ts2;
		}
		//hitlist::instance().insert(oh.src, oh.dst, oh.hittime);
	}
	return 0;
}
