CXXFLAGS=-Wall -pedantic -Werror -ansi -fsigned-char
LIBS+=-lm -ldl

ifeq ($(DEBUG), 1)
CFLAGS +=-g
endif

OBJ=jmlib.o  prefs.o  util.o  validator.o

all: $(OBJ)
	ar -r jmlib.a ${OBJ}
	ranlib jmlib.a

clean:
	rm -f *.o core jmlib.a
