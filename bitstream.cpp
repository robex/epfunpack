#include <cstdio>
#include <stdint.h>
#include <iostream>
#include <vector>
#include <byteswap.h>
#include "bitstream.hpp"

bitstream::~bitstream() {
	close();
}

void bitstream::close() {
	if (f != NULL) {
		fclose(f);
		f = NULL;
	}
	if (out != NULL) {
		fclose(out);
		out = NULL;
	}
	bitpos = 0;
}

int bitstream::get_file_size(FILE *f)
{
	fseek(f, 0, SEEK_END);
	int len = ftell(f);
	rewind(f);
	return len;
}

int bitstream::openread(std::string filename)
{
	f = fopen(filename.c_str(), "rb");
	return f != NULL;
}

int bitstream::openwrite(std::string filename)
{
	out = fopen(filename.c_str(), "wb");
	return out != NULL;
}

/* pos is in BITS */
uint8_t bitstream::readbit(int pos)
{
	uint8_t bit;
	int bytetoread = pos / 8;
	int bittoread = 7 - (pos % 8);
	fseek(f, bytetoread, 0);
	fread(&bit, 1, 1, f);
	bit >>= bittoread;
	bit &= 1;
	return bit;
}

/* read next n-bit unit into a 16-bit unsigned int */
uint16_t bitstream::read(int nbits)
{
	uint16_t cur = 0;
	uint8_t  bit;
	for (int j = 0; j < nbits; j++) {
		bit = readbit(bitpos++);
		cur = (cur << 1) | bit;
	}
	return cur;
}

/* read and decode entire file into a vector */
std::vector<uint16_t> bitstream::readarr(int nbits)
{
	int len = get_file_size(f) * 8;
	int nsteps = len / nbits;

	std::vector<uint16_t> v;
	for (int i = 0; i < nsteps; i++) {
		v.push_back(read(nbits));
	}
	return v;
}

/* flush incomplete byte (zeroed to the right) */
void bitstream::flush()
{
	if (nbuf == 0)
		return;
	fwrite(&wbyte, 1, 1, out);
	nbuf = 0;
	wbyte = 0;
}

/* write single bit at the end of file *out (will be
 * automatically flushed when whole byte is full) */
void bitstream::writebit(uint8_t bit, FILE *out)
{
	wbyte = wbyte | ((bit & 1) << (7 - nbuf));
	nbuf++;
	bitpos++;

	if (nbuf == 8) {
		fwrite(&wbyte, 1, 1, out);
		wbyte = 0;
		nbuf = 0;
	}
}

/* write single n-bit unit at the end of the file */
void bitstream::write(uint16_t data, int nbits)
{
	for (int j = 0; j < nbits; j++) {
		writebit(data >> ((nbits - 1) - j), out);
	}
}

/* write entire vector to file, where each uint16_t must have
 * at least nbits set, higher ones will be discarded */
void bitstream::writearr(std::vector<uint16_t> data, int nbits)
{
	int len = data.size();

	for (int i = 0; i < len; i++) {
		write(data[i], nbits);
	}
}

/* set cursor at position pos (in bits) */
void bitstream::seek(int pos)
{
	bitpos = pos;
	nbuf = 0;
	fseek(out, 0, pos/8);
}

/* return current cursor position */
int bitstream::getpos()
{
	return bitpos;
}
