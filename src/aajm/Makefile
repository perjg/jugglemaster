CXX=c++
MAKE=make

# took out -ansi since usleep(3) isn't ANSI
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
	rm -f *.o core aajm patterns.jm semaphore.jm
	$(MAKE) -C ../jmlib clean
