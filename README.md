## epfunpack

Extract files from EPF archives used in some DOS games (mainly published by Virgin) such as The Lion King. The files are packed and then compressed using a variant of the LZW algorithm. This tool can unpack and uncompress the files contained in the EPF archive in a single pass.

---

### Compilation:

This program has no dependencies outside of standard C++ libraries. Simply run:

```make```

### Usage:

```./epfunpack [epf_file]```

Files will be output to directory `dump`, the ones with extension `.dec` are the decompressed versions.

---

**Reference file** used for developing and testing the program (`SHA1`):

```8385e8287264143588457d414a1c24d49e8684b7  LIONKING.EPF```
