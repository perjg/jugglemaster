# If you have ffmpeg, point this at the directory you built it in,
#  and set HAVE_FFMPEG to 1. Tested with 0.4.9-pre1
FFMPEG_PREFIX=/tmp/chunky/ffmpeg
HAVE_FFMPEG=0


# Nothing below this should need to be edited
CXXFLAGS=-Wall -Werror -fsigned-char `wx-config --cppflags`
# -ansi -pedantic cause warnings from some compilers [wx uses long long]
LDFLAGS=`wx-config --ldflags`
# STATICFLAGS=-Wl,-Bstatic
STATICFLAGS=-static
LIBS+=`wx-config --libs`
BINARY=jmdlx
VERSION=0.4

ifeq ($(HAVE_FFMPEG), 1)
CXXFLAGS+=-DHAVE_AVCODEC_H -I$(FFMPEG_PREFIX)/libavcodec
LDFLAGS+=-L$(FFMPEG_PREFIX)/libavcodec
LIBS+=-lavcodec
endif


# OSX Voodoo
APPNAME=JuggleMaster
BUNDLE=$(APPNAME).app/Contents
RSRC=`wx-config --prefix`/lib/*.rsrc

OBJ=jmdlx.o patt.o advsite.o choosepatt.o choosestyle.o newsemaphore.o print.o

all: jm_lib $(OBJ)
	$(CXX) $(LDFLAGS) -o ./$(BINARY) $(CFLAGS) $(OBJ) ../jmlib/jmlib.a $(LIBS)
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

patt: jm_lib
	rm -f patt.o
	$(CXX) -DPATT_STANDALONE $(CXXFLAGS) -c -o ./patt.o ./patt.cpp
	$(CXX) $(LDFLAGS) -o ./patt ./patt.o ../jmlib/jmlib.a $(LIBS)

clean:
	rm -f *.o core $(BINARY) $(BINARY)-static patt patterns.jm semaphore.jm
	rm -rf JuggleMaster.app
	$(MAKE) -C ../jmlib clean

v: clean all
	valgrind ./jmdlx
