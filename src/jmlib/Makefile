CXXFLAGS=-g -Wall -pedantic -fsigned-char -Werror
LIBS+=-lm -ldl

ifeq ($(DEBUG), 1)
CFLAGS +=-g
endif

DATA=../../data/patterns.jm ../../data/semaphore.jm
OBJ=jmlib.o  prefs.o  util.o  validator.o

all: $(OBJ)
	ar -r jmlib.a $(OBJ)
	ranlib jmlib.a

patterns:
	rm -f patterns.o
	$(CXX) $(CXXFLAGS) -DPATTERNS_STANDALONE -c -o ./patterns.o ./patterns.cpp
	$(CXX) $(LDFLAGS) -o ./patterns ./patterns.o $(LIBS)

clean:
	rm -f *.o core jmlib.a patterns

p: clean patterns
	./patterns $(DATA)

v: clean patterns
	valgrind --leak-check=yes ./patterns -q $(DATA)
