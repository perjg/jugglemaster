include ../../Makefile.cfg

CXXFLAGS+=-Wall -fsigned-char `aalib-config --cflags`
CFLAGS+=-Wall -fsigned-char `aalib-config --cflags` -I../jmlib
LIBS+=`aalib-config --libs` -L../jmlib -ljm
BINARY=aajm
INSTALL=install

ifeq ($(HAVE_FFMPEG), 1)
CXXFLAGS+=-DHAVE_AVCODEC_H -I$(FFMPEG_PREFIX)/libavcodec
CFLAGS+=-DHAVE_AVCODEC_H -I$(FFMPEG_PREFIX)/libavcodec
LDFLAGS+=-L$(FFMPEG_PREFIX)/libavcodec -lz
LIBS+=-lavcodec -lz
endif

ifeq ($(DEBUG), 1)
CXXFLAGS += -g -Werror
endif

OBJ=aa_drawcircle.o aa_drawline.o aajm.o aampeg.o

all: aajm aajm-remote

aajm: jm_lib $(OBJ)
	$(CXX) $(LDFLAGS) -o $(BINARY) $(OBJ) $(LIBS)

jm_lib:
	$(MAKE) -C ../jmlib

install:
	mkdir -p $(INSTALLDIR)/bin
	$(INSTALL) $(BINARY) $(INSTALLDIR)/bin
	$(INSTALL) aajm-remote $(INSTALLDIR)/bin

clean:
	rm -f *.o core aajm aajm-remote
	$(MAKE) -C ../jmlib clean
