include ../../Makefile.cfg

CXXFLAGS+=-Wall -pedantic -fsigned-char
CFLAGS+=-Wall -pedantic -fsigned-char
LIBS+=-lm -ldl

ifeq ($(DEBUG), 1)
CXXFLAGS+=-g -Werror
endif

DATA=../../data/patterns.jm ../../data/semaphore.jm
OBJ=jmlib.o prefs.o util.o validator.o patterns.o randompatt.o

LIBNAME=libjm.a
all: $(LIBNAME)

$(LIBNAME): $(OBJ)
	ar -r $(LIBNAME) $(OBJ)
	ranlib $(LIBNAME)

patterns:
	$(CXX) $(CXXFLAGS) -DPATTERNS_STANDALONE -c -o ./patterns-standalone.o ./patterns.cpp
	$(CXX) $(LDFLAGS) -o ./patterns ./patterns-standalone.o $(LIBS)

rand: clean validator.o
	$(CXX) $(CXXFLAGS) -DRAND_STANDALONE -c -o ./rand-standalone.o ./randompatt.cpp
	$(CXX) $(LDFLAGS) -o ./rand ./rand-standalone.o validator.o $(LIBS)
	./rand | less

validator: clean
	$(CXX) $(CXXFLAGS) -DVALIDATOR_STANDALONE -c -o ./validator-standalone.o ./validator.cpp
	$(CXX) $(LDFLAGS) -o ./validator ./validator-standalone.o $(LIBS)

install: $(LIBNAME)
	mkdir -p /usr/local/include/jm
	cp *.h /usr/local/include/jm/
	cp libjm.a /usr/local/lib

clean:
	rm -f *.o core $(LIBNAME) patterns rand validator

p: clean patterns
	./patterns $(DATA)

v: clean patterns
	valgrind --leak-check=yes ./patterns -q $(DATA)
