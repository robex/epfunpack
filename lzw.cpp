#include <iostream>
#include <vector>
#include <cstdio>
#include <stdint.h>
#include <map>
#include "bitstream.hpp"
#include "lzw.hpp"

void lzw::dict_init_comp(std::map<std::string, uint16_t> &dict)
{
	for (uint16_t i = 0; i < 0x100; i++) {
		dict[std::string(1, i)] = i;
	}
}

void lzw::dict_init_decomp(std::map<uint16_t, std::string> &dict)
{
	for (uint16_t i = 0; i < 0x100; i++) {
		dict[i] = std::string(1, i);
	}
}

int lzw::get_file_size(FILE *f)
{
	fseek(f, 0, SEEK_END);
	int length = ftell(f);
	rewind(f);
	return length;
}

int lzw::compress(uint16_t bitlimit, std::string infile, 
			   std::string outfile)
{
	std::map<std::string, uint16_t> dict;
	bitstream b;

	FILE *f = fopen(infile.c_str(), "rb");
	if (!f)
		return 0;
	if (!b.openwrite(outfile))
		return 0;
	int len = get_file_size(f);

	dict_init_comp(dict);
	uint32_t dictsize = 256;
	uint32_t nbits = 9;

	// reserved codes
	uint32_t eofcode;
	uint32_t resetcode;
	eofcode   = (1 << nbits) - 1;
	resetcode = (1 << nbits) - 2;

	std::string w;
	uint8_t curbyte;
	for (int i = 0; i < len; i++) {
		fread(&curbyte, 1, 1, f);
		std::string wc = w;
		wc += curbyte;
		if (dict.count(wc)) {
			w = wc;
		} else {
			uint16_t rep = dict[w];
			b.write(rep, nbits);
			dict[wc] = dictsize++;
			w = std::string(1, curbyte);
			
			// only reset the dictionary when nbits == bitlimit,
			// ignore reserved words until bitlimit
			if (dictsize == eofcode) {
				if (nbits != bitlimit) {
					nbits++;
					eofcode   = (1 << nbits) - 1;
					resetcode = (1 << nbits) - 2;
				} else {
					dictsize = 256;
					dict.clear();
					dict_init_comp(dict);
					// remember to write the actual
					// reset code
					b.write(resetcode, nbits);
				}
			}
		}
	}
	uint16_t rep = dict[w];
	b.write(rep, nbits);
	b.write(eofcode, nbits);
	b.flush();
	fclose(f);
	return 1;
}

int lzw::decompress(uint16_t bitlimit, std::string infile,
		    std::string outfile)
{
	std::map<uint16_t, std::string> dict;
	bitstream b;
	if (!b.openread(infile))
		return 0;
	FILE *out = fopen(outfile.c_str(), "wb");
	if (!out)
		return 0;

	uint32_t eofcode;
	uint32_t resetcode;
	uint32_t maxcode;
	uint32_t nbits = 9;
reset:
	uint32_t dictsize = 256;
	dict.clear();
	dict_init_decomp(dict);
	// reserved codes:
	// 	- 2^n - 1: end of file
	// 	- 2^n - 2: reset dictionary (but dont change bit length)
	eofcode   = (1 << nbits) - 1;
	resetcode = (1 << nbits) - 2;
	maxcode   = (1 << nbits) - 3;

	// add first code to the dictionary
	uint16_t firstcode, curcode = 0;
	firstcode = b.read(nbits);

	std::string w(1, firstcode);
	std::string tempmatch;
	std::string curmatch;
	std::string newdictentry;
	curmatch = dict[firstcode];
	fwrite(curmatch.data(), curmatch.size(), 1, out);

	while (1) {
		curcode = b.read(nbits);
		// end of file
		if (curcode == eofcode)
			break;

		// reset dictionary
		// yes, a goto. what you gonna do?
		if (curcode == resetcode)
			goto reset;

		// decode
		if (dict.count(curcode))
			tempmatch = dict[curcode];
		else
			tempmatch = curmatch + curmatch[0];

		fwrite(tempmatch.data(), tempmatch.size(), 1, out);
		newdictentry = curmatch;
		newdictentry += tempmatch[0];
		dict[dictsize] = newdictentry;
		dictsize++;
		curmatch = tempmatch;

		// when the next code would be reserved,
		// increase the bit length if it is not the maximum
		if (dictsize > maxcode) {
			if (nbits != bitlimit) {
				nbits++;
				eofcode   = (1 << nbits) - 1;
				resetcode = (1 << nbits) - 2;
				maxcode   = (1 << nbits) - 3;
			}
		}
	}
	fclose(out);
	return 1;
}
