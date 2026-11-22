include ../../Makefile.cfg

CXXFLAGS+=-Wall -fsigned-char `wx-config --cppflags` -I../jmlib
CFLAGS+=-Wall -fsigned-char `wx-config --cppflags` -I../jmlib

# Silence OpenGL deprecation warnings on macOS (OpenGL still works, Apple just prefers Metal)
CXXFLAGS+=-DGL_SILENCE_DEPRECATION
CFLAGS+=-DGL_SILENCE_DEPRECATION
# -ansi -pedantic cause warnings from some compilers [wx uses long long]
#LDFLAGS+=`wx-config --ldflags`
# STATICFLAGS=-Wl,-Bstatic
STATICFLAGS=-static

# Base wxWidgets libraries (always needed)
LIBS+=`wx-config --libs`

# Add jmlib and sqlite3
LIBS+=-L../jmlib -ljm -lsqlite3

# OpenGL ES 3.0 via Desktop Shaders (set to 1 to use shader pipeline)
USE_SHADERS?=0
export USE_SHADERS

# OpenGL Support
OPENGL_SUPPORT=1
export OPENGL_SUPPORT

# GL Libraries from wxWidgets (common for both)
WX_GL_LIBS:=$(shell wx-config --libs gl | sed 's/-lwx_baseu[^ ]*//g')

ifeq ($(USE_SHADERS), 1)
    # Use Shader-based pipeline on Desktop GL
    CXXFLAGS+=-DOPENGL_SUPPORT -DUSE_SHADER_RENDERING
    LIBS+=$(WX_GL_LIBS)
else ifeq ($(OPENGL_SUPPORT), 1)
    # Use desktop OpenGL fixed function pipeline
    CXXFLAGS+=-DOPENGL_SUPPORT
    LIBS+=$(WX_GL_LIBS)
endif

BINARY=jmdlx
INSTALL=install

ifeq ($(HAVE_FFMPEG), 1)
CXXFLAGS+=-DHAVE_AVCODEC_H -I$(FFMPEG_PREFIX)/libavcodec
LDFLAGS+=-L$(FFMPEG_PREFIX)/libavcodec
LIBS+=-lavcodec
endif

ifeq ($(DEBUG), 1)
CXXFLAGS+=-g -Werror
endif


# OSX Voodoo
APPNAME=JuggleMaster
BUNDLE=$(APPNAME).app/Contents
RSRC=`wx-config --prefix`/lib/*.rsrc

OBJ=jmdlx.o patt.o advsite.o choosepatt.o choosestyle.o newsemaphore.o print.o
ifeq ($(OPENGL_SUPPORT), 1)
OBJ+=opengl.o
endif

all: jmdlx

jmdlx: jm_lib $(OBJ)
	$(CXX) $(LDFLAGS) -o ./$(BINARY) $(OBJ) $(LIBS)
	cp ../../data/*.jm .
	cp ../../data/moresites.txt .

osx: all
	tar -xf macosx/JuggleMaster.app.tar
	cp $(BINARY) JuggleMaster.app/Contents/MacOS/$(APPNAME)
	sed -e s/VERSION/$(VERSION)/ macosx/Info.plist.in > JuggleMaster.app/Contents/Info.plist
	cp ../../data/*.jm JuggleMaster.app/Contents/Resources
	cp ../../data/moresites.txt JuggleMaster.app/Contents/Resources
	-cp $(RSRC) JuggleMaster.app/Contents/Resources/JuggleMaster.rsrc 2>/dev/null || true

jm_lib:
	$(MAKE) -C ../jmlib

install:
	mkdir -p $(INSTALLDIR)/bin
	$(INSTALL) $(BINARY) $(INSTALLDIR)/bin

patt: jm_lib
	rm -f patt.o
	$(CXX) -DPATT_STANDALONE $(CXXFLAGS) -c -o ./patt.o ./patt.cpp
	$(CXX) $(LDFLAGS) -o ./patt ./patt.o $(LIBS)

clean:
	rm -f *.o core $(BINARY) $(BINARY)-static patt patterns.jm semaphore.jm moresites.txt
	rm -rf JuggleMaster.app
	$(MAKE) -C ../jmlib clean

v: clean all
	valgrind ./jmdlx

a: all
	./jmdlx
