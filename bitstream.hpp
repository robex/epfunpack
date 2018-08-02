#ifndef BINARY_H
#define BINARY_H

#include <cstdio>
#include <vector>
#include <stdint.h>

class bitstream {
	// file for reading 
	FILE *f = NULL;
	// file for writing 
	FILE *out = NULL;
	// number of bits in the buffer to be written
	// to file (flushed when 8)
	int nbuf = 0;
	// buffer byte
	uint8_t wbyte = 0;
	// current position for bitstream::read
	int bitpos = 0;

	/* get file size in bytes */
	int get_file_size(FILE *f);
	/* read and return single bit at position pos */
	uint8_t readbit(int pos);
	/* write single bit at the end of file *out (will be
	 * automatically flushed when whole byte is full) */
	void writebit(uint8_t bit, FILE *out);

    public:
	/* open file for reading */
	int openread(std::string filename);
	/* open file for writing */
	int openwrite(std::string filename);
	/* read next n-bit unit into a 16-bit unsigned int */
	uint16_t read(int nbits);
	/* read and decode entire file into a vector */
	std::vector<uint16_t> readarr(int nbits);
	/* flush incomplete byte (zeroed to the right) */
	void flush();
	/* write single n-bit unit at the end of the file */
	void write(uint16_t data, int nbits);
	/* write entire vector to file, where each uint16_t must
	 * have at least nbits set, higher ones will be discarded */
	void writearr(std::vector<uint16_t> data, int nbits);
	/* set cursor at position pos (in bits) */
	void seek(int pos);
	/* return current cursor position */
	int getpos();
	/* close open files */
	void close();
	~bitstream();
};

#endif
