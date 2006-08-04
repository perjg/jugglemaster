default:
	@echo Please use "\"make {target}\"" where target can be any of:
	@echo jmlib, jmdlx, aajm, jmqt, all
	@echo
	@echo If you don\'t know what you want, use "make jmdlx"
	@echo
	@echo To install just the lib, \"make jmlib-install\"
	@echo
	@echo When built, binaries should be in the bin dir

all: jmlib jmdlx aajm

jmlib:
	$(MAKE) -C src/jmlib

jmdlx:
	$(MAKE) -C src/jmdlx
	mkdir -p ./bin
	cp src/jmdlx/jmdlx ./bin

aajm:
	$(MAKE) -C src/aajm
	mkdir -p ./bin
	cp src/aajm/aajm src/aajm/aajm-remote ./bin

jmqt:
	$(MAKE) -C src/jmqt
	mkdir -p ./bin
	cp src/jmqt/jmqt ./bin

jmlib-install:
	$(MAKE) -C src/jmlib install

install:
	@echo Current no-oped.
	@echo Please look in the bin dir after building binaries using
	@echo  "make {target}"

clean:
	$(MAKE) -C src/jmlib clean
	$(MAKE) -C src/jmdlx clean
	$(MAKE) -C src/aajm clean
	$(MAKE) -C src/jmqt clean
	rm -rf bin
