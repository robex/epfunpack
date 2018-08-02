#ifndef EPF_H
#define EPF_H

#include <stdint.h>

#ifdef __linux__
	#define SEP_CHAR "/"
#elif _WIN32
	#define SEP_CHAR "\\"
#else
	#define SEP_CHAR "/"
#endif

struct _epf {
	uint32_t fat_off;
	uint8_t  unknown;
	uint16_t numfiles;
};

struct epf_file {
	char     filename[13];
	uint8_t  iscompressed;
	uint32_t compsize;
	uint32_t decompsize;
};

#endif
