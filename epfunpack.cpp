#include <cstdio>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "lzw.hpp"
#include "epfunpack.hpp"

#define BASEDIR "dump"

void usage()
{
	printf("usage: ./epfunpack [epf_file]\n");
}

int is_valid_epf(FILE *f)
{
	int sig;
	fread(&sig, 4, 1, f);
	// signature = "EPFS"
	return sig == 0x53465045;
}

/* Create directory if it doesn't exist, return 0 if other error happens */
int create_dir(const char *dirname)
{
	DIR *dir = opendir(dirname);
	if (errno == ENOENT) {
	#ifdef _WIN32
		_mkdir(dirname);
	#else
		mkdir(dirname, 0775);
	#endif
		return 1;
	} else if (!dir) {
		return 0;
	} else {
		return 1;
	}
}

int decomp_file(char *file, int compsize, int decompsize)
{
	lzw l;
	char outdec[70];
	sprintf(outdec, "%s.dec", file);
	if (!l.decompress(14, file, outdec))
		return 0;
	else
		return 1;
}

int epf_unpack(FILE *f)
{
	FILE *outfile;
	struct _epf epf;
	char out[64];

	fread(&epf.fat_off, 4, 1, f);
	fread(&epf.unknown, 1, 1, f);
	fread(&epf.numfiles, 2, 1, f);

	// start position of the actual data of the first file
	int pos = 11;
	// start position of the file directory
	int fatpos = epf.fat_off;

	struct epf_file file;
	uint8_t buf;

	fseek(f, fatpos, 0);
	for (int i = 0; i < epf.numfiles; i++) {
		fseek(f, fatpos, 0);
		fread(&file.filename, 13, 1, f);
		fread(&file.iscompressed, 1, 1, f);
		fread(&file.compsize, 4, 1, f);
		fread(&file.decompsize, 4, 1, f);
		fatpos = ftell(f);

		fseek(f, pos, 0);
		sprintf(out, BASEDIR SEP_CHAR "%s", file.filename);
		outfile = fopen(out, "wb");
		if (!outfile)
			return 0;
		for (uint32_t j = 0; j < file.compsize; j++) {
			fread(&buf, 1, 1, f);
			fwrite(&buf, 1, 1, outfile);
		}
		pos = ftell(f);
		fclose(outfile);
		if (file.iscompressed) {
			printf("decompressing %s to %s.dec\n", out, out);
			decomp_file(out, file.compsize, file.decompsize);
		}
	}

	return 1;
}

int main(int argc, char **argv)
{
	if (argc < 2) {
		usage();
		return 1;
	}

	FILE *f = fopen(argv[1], "rb");
	if (!f) {
		printf("error opening file %s\n", argv[1]);
		return 1;
	}
	if (!is_valid_epf(f)) {
		printf("fatal: file %s is not a valid epf archive!\n",
		       argv[1]);
		return 1;
	}
	
	std::string dir = BASEDIR;
	if (!create_dir(dir.c_str())) {
		printf("fatal: error creating directory \"" BASEDIR "\"\n");
		return 1;
	}

	if (!epf_unpack(f)) {
		printf("fatal: error creating directory \"" BASEDIR "\"\n");
		return 1;
	}

	fclose(f);
	return 0;
}
