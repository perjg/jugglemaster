CXX=c++
MAKE=make

CXXFLAGS=-Wall -pedantic -Werror -ansi -fsigned-char `wx-config --cppflags`
LDFLAGS=`wx-config --ldflags`
// STATICFLAGS=-Wl,-Bstatic
STATICFLAGS=-static
LIBS+=`wx-config --libs`

ifeq ($(DEBUG), 1)
CXXFLAGS += -g
endif

OBJ=jmdlx.o patt.o advsite.o choosepatt.o choosestyle.o newsemaphore.o print.o

all: jm_lib $(OBJ)
	$(CXX) $(LDFLAGS) -o ./jmdlx $(CFLAGS) $(OBJ) ../jmlib/jmlib.a $(LIBS)
	cp ../../data/*.jm .

static: jm_lib $(OBJ)
	$(CXX) $(STATICFLAGS) $(LDFLAGS) -o ./jmdlx-static $(CFLAGS) $(OBJ) ./jmlib/jmlib.a $(LIBS)
	cp ../data/*.jm .

osx: all
	mv jmdlx jmdlx-osx
	`wx-config --rezflags` ./jmdlx-osx

jm_lib:
	$(MAKE) -C ../jmlib

patt:
	$(CXX) -DPATT_STANDALONE $(CXXFLAGS) -c -o ./patt.o ./patt.cpp
	$(CXX) $(LDFLAGS) -o ./patt $(CXXFLAGS) ./patt.o $(LIBS)

clean:
	rm -f *.o core jmdlx jmdlx-static jmdlx-osx patt patterns.jm semaphore.jm
	$(MAKE) -C ../jmlib clean
