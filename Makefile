epfunpack: lzw.o epfunpack.cpp epfunpack.hpp lzw.hpp bitstream.hpp bitstream.o
	g++ -g -o epfunpack epfunpack.cpp bitstream.o lzw.o -Wall --std=c++11

lzw.o: lzw.cpp lzw.hpp bitstream.hpp
	g++ -c -g -o lzw.o lzw.cpp -Wall --std=c++11

bitstream.o: bitstream.cpp bitstream.hpp
	g++ -g -c -o bitstream.o bitstream.cpp -Wall -std=c++11

clean:
	rm lzw.o bitstream.o epfunpack
