include ../../Makefile.cfg

CXXFLAGS=-Wall -pedantic -fsigned-char
LIBS+=-lm -ldl

ifeq ($(DEBUG), 1)
CXXFLAGS+=-g -Werror
endif

DATA=../../data/patterns.jm ../../data/semaphore.jm
OBJ=jmlib.o prefs.o util.o validator.o patterns.o

all: jmlib.a

jmlib.a: $(OBJ)
	ar -r jmlib.a $(OBJ)
	ranlib jmlib.a

patterns:
	$(CXX) $(CXXFLAGS) -DPATTERNS_STANDALONE -c -o ./patterns-standalone.o ./patterns.cpp
	$(CXX) $(LDFLAGS) -o ./patterns ./patterns-standalone.o $(LIBS)

clean:
	rm -f *.o core jmlib.a patterns

p: clean patterns
	./patterns $(DATA)

v: clean patterns
	valgrind --leak-check=yes ./patterns -q $(DATA)
