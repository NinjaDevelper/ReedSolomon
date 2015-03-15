CPPFLAGS+=-Wall -IReedSolomon
LDFLAGS+=-lm

TEST_CPPFLAGS=-fprofile-arcs -ftest-coverage  -Ilibtap -g -ggdb3
TEST_LDFLAGS= -Llibtap -ltap 

FILES=ReedSolomon/ReedSolomon.cpp ReedSolomon/ReedSolomon.h 

all: reedSolomon

reedSolomon: ${FILES} ReedSolomon/main.cpp
	g++ -O3 ${CPPFLAGS} -o reedSolomon ReedSolomon/ReedSolomon.cpp  ReedSolomon/main.cpp  ${LDFLAGS} 

test: ${FILES} tests/test.cpp
	cd libtap;make
	g++ ${CPPFLAGS}  ${TEST_CPPFLAGS} -o test ReedSolomon/ReedSolomon.cpp  tests/test.cpp  ${LDFLAGS}   ${TEST_LDFLAGS}

python: ${FILES} ReedSolomon/ReedSolomon_python.cpp
	python setup.py build_ext -i

clean:
	rm -f *.o  ReedSolomon/*.o reedSolomon test
