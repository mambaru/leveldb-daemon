#pragma once

#include <zlib.h>

namespace fas {
namespace filter {



//ad_file_write
struct ad_compressed_file_write : fas::adv::io::desc_holder<gzFile> {
	typedef gzFile desc_type;
	typedef fas::adv::io::desc_holder<gzFile> super;

	typedef ad_compressed_file_write write_advice_tag;
	typedef size_t size_type;
	typedef size_t return_type;

	template<typename T>
	size_t operator()(T& t, const char* d, typename T::write_size_type s) {
		size_t rt = gzwrite(super::get_d(), d, s);
		if (rt == 0 ) super::set_status(false);
		return rt;
	}
	void clear() {
	}
};

struct compressed_file_write_advice : aa::advice<
	aa::tag<_write_policy_>,
	ad_compressed_file_write
> {};

template<typename T>
struct compressed_filter : binary_filter<
	typename aa::aspect_merge< T, aa::aspect <
		ap::type_list_n<
			compressed_file_write_advice
		>::type
	> >::type
> {


};



}}


namespace std {

	inline bool getline (gzFile file,std::vector<char> &line) {
		line.resize(line.capacity());
		line[line.size()-1] = 0;
		if (!gzgets(file, line.data(), line.size() )) return false;
		if (line[line.size()-1]==0) {
			line.resize(std::strlen(line.data()));
			return true;
		}

		int pos = 0;
		while (line[line.size()-1]!=0) {	//если последний символ изменился, значит, нам не хватило буфера на чтение
			pos+=line.size()-1;
			if (pos>1024*1024*1024) {
				std::cerr << "maximum line length reached\n";
				return false;
			}
			line.resize(line.size()+16384);
			line[line.size()-1] = 0;
			if (!gzgets(file, line.data()+pos, line.size()-pos )) return false;
		}
		line.resize(strlen(line.data()+pos)+pos);
		return true;
	}
}
