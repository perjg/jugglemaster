CXXFLAGS=-g -Wall -pedantic -fsigned-char -Werror
LIBS+=-lm -ldl

ifeq ($(DEBUG), 1)
CFLAGS +=-g
endif

DATA=../../data/patterns.jm ../../data/semaphore.jm
OBJ=jmlib.o prefs.o util.o validator.o patterns.o

all: patt_clean $(OBJ)
	ar -r jmlib.a $(OBJ)
	ranlib jmlib.a

patterns: patt_clean
	$(CXX) $(CXXFLAGS) -DPATTERNS_STANDALONE -c -o ./patterns.o ./patterns.cpp
	$(CXX) $(LDFLAGS) -o ./patterns ./patterns.o $(LIBS)

patt_clean:
	rm -f patterns.o

clean:
	rm -f *.o core jmlib.a patterns

p: clean patterns
	./patterns $(DATA)

v: clean patterns
	valgrind --leak-check=yes ./patterns -q $(DATA)
