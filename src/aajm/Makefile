CXX=c++
MAKE=make

CXXFLAGS=-Wall -pedantic -Werror -fsigned-char `aalib-config --cflags`
LDFLAGS=`aalib-config --libs`

ifeq ($(DEBUG), 1)
CXXFLAGS += -g
endif

OBJ=aa_drawcircle.o aa_drawline.o aajm.o

all: jm_lib $(OBJ)
	$(CXX) $(LDFLAGS) -o ./aajm $(OBJ) ../jmlib/jmlib.a

jm_lib:
	$(MAKE) -C ../jmlib

clean:
	rm -f *.o core aajm
	$(MAKE) -C ../jmlib clean
