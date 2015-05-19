CPPFLAGS+=-Wall -Icxx
LDFLAGS+=-lm

TEST_CPPFLAGS=-fprofile-arcs -ftest-coverage  -Ilibtap -g -ggdb3
TEST_LDFLAGS= -Llibtap -ltap 

FILES=cxx/ReedSolomon.cpp cxx/ReedSolomon.h 

all: reedSolomon

reedSolomon: ${FILES} cxx/main.cpp
	g++ -O3 ${CPPFLAGS} -o reedSolomon cxx/ReedSolomon.cpp  cxx/main.cpp  ${LDFLAGS} 

test: ${FILES} tests/test.cpp
	cd libtap;make
	g++ ${CPPFLAGS}  ${TEST_CPPFLAGS} -o test cxx/ReedSolomon.cpp  tests/test.cpp  ${LDFLAGS}   ${TEST_LDFLAGS}

python: ${FILES} cxx/ReedSolomon_python.cpp
	python setup.py build_ext -i

clean:
	rm -f *.o  cxx/*.o reedSolomon test
