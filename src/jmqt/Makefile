include ../../Makefile.cfg

BINARY=jmqt
INSTALL=install

all: Makefile.jmqt
	$(MAKE) -C ../jmlib
	$(MAKE) -f Makefile.jmqt

install:
	mkdir -p $(INSTALLDIR)/bin
	$(INSTALL) $(BINARY) $(INSTALLDIR)/bin

clean: Makefile.jmqt
	$(MAKE) -f Makefile.jmqt clean 
	rm -f Makefile.jmqt
	rm -f $(BINARY)

Makefile.jmqt:
	qmake jugglemaster.pro -o Makefile.jmqt
