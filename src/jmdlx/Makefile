CXX=c++
MAKE=make

CXXFLAGS=-Wall -Werror -fsigned-char `wx-config --cppflags`
# -ansi -pedantic cause warnings from some compilers [wx uses long long]
LDFLAGS=`wx-config --ldflags`
# STATICFLAGS=-Wl,-Bstatic
STATICFLAGS=-static
LIBS+=`wx-config --libs`
BINARY=jmdlx
VERSION=0.3


# OSX Voodoo
APPNAME=JuggleMaster
BUNDLE=$(APPNAME).app/Contents
RSRC=`wx-config --prefix`/lib/*.rsrc


ifeq ($(DEBUG), 1)
CXXFLAGS += -g
endif

OBJ=jmdlx.o patt.o advsite.o choosepatt.o choosestyle.o newsemaphore.o print.o

all: jm_lib $(OBJ)
	$(CXX) $(LDFLAGS) -o ./$(BINARY) $(CFLAGS) $(OBJ) ../jmlib/jmlib.a $(LIBS)
	cp ../../data/*.jm .

static: jm_lib $(OBJ)
	$(CXX) $(STATICFLAGS) $(LDFLAGS) -o ./$(BINARY)-static $(CFLAGS) $(OBJ) ./jmlib/jmlib.a $(LIBS)
	cp ../../data/*.jm .

osx: all
	tar -xf macosx/JuggleMaster.app.tar
	`wx-config --rezflags` ./$(BINARY)
	cp $(BINARY) JuggleMaster.app/Contents/MacOS/$(APPNAME)
	sed -e s/VERSION/$(VERSION)/ macosx/Info.plist.in > JuggleMaster.app/Contents/Info.plist
	cp ../../data/*.jm JuggleMaster.app/Contents/Resources
	cp $(RSRC) JuggleMaster.app/Contents/Resources/JuggleMaster.rsrc

jm_lib:
	$(MAKE) -C ../jmlib

patt:
	$(CXX) -DPATT_STANDALONE $(CXXFLAGS) -c -o ./patt.o ./patt.cpp
	$(CXX) $(LDFLAGS) -o ./patt $(CXXFLAGS) ./patt.o $(LIBS)

clean:
	rm -f *.o core $(BINARY) $(BINARY)-static patt patterns.jm semaphore.jm
	rm -rf JuggleMaster.app
	$(MAKE) -C ../jmlib clean
