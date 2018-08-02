#ifndef LZW_H
#define LZW_H

#include <vector>
#include <map>
#include <stdint.h>

class lzw {
	FILE *f = NULL;
	FILE *out = NULL;
	int get_file_size(FILE *f);
	void dict_init_comp(std::map<std::string, uint16_t> &dict);
	void dict_init_decomp(std::map<uint16_t, std::string> &dict);

    public:
	int compress(uint16_t bitlimit, std::string infile,
		     std::string outfile);
	int decompress(uint16_t bitlimit, std::string infile,
		       std::string outfile);
};

#endif
