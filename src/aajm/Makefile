include ../../Makefile.cfg

CXXFLAGS+=-Wall -pedantic -fsigned-char `aalib-config --cflags`
LDFLAGS+=`aalib-config --libs`
BINARY=aajm
INSTALL=install

ifeq ($(DEBUG), 1)
CXXFLAGS += -g -Werror
endif

OBJ=aa_drawcircle.o aa_drawline.o aajm.o

all: aajm aajm-remote

aajm: jm_lib $(OBJ)
	$(CXX) $(LDFLAGS) -o $(BINARY) $(OBJ) ../jmlib/jmlib.a

jm_lib:
	$(MAKE) -C ../jmlib

install:
	mkdir -p $(INSTALLDIR)/bin
	$(INSTALL) $(BINARY) $(INSTALLDIR)/bin
	$(INSTALL) aajm-remote $(INSTALLDIR)/bin

clean:
	rm -f *.o core aajm aajm-remote
	$(MAKE) -C ../jmlib clean
