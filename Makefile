CPPFLAGS+=-Wall -Icxx
LDFLAGS+=-lm

TEST_CPPFLAGS=-fprofile-arcs -ftest-coverage  -Ilibtap -g -ggdb3
TEST_LDFLAGS= -Llibtap -ltap 

all: reedSolomon

reedSolomon: cxx/ReedSolomon.cpp cxx/main.cpp
	$(CXX) -O3 ${CPPFLAGS} -o $@ $^  ${LDFLAGS}

test: cxx/ReedSolomon.o tests/test.o
	cd libtap;make
	$(CXX) ${CPPFLAGS}  ${TEST_CPPFLAGS} -o $@ $^  ${LDFLAGS}   ${TEST_LDFLAGS}

.cpp.o:
	$(CXX) ${CPPFLAGS}  ${TEST_CPPFLAGS} -o $@ -c $^


clean:
	rm -f *.o  cxx/*.o reedSolomon test
