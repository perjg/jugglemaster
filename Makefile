default:
	@echo Please use "\"make {target}\"" where target can be any of:
	@echo jmdlx, aajm, jmqt, all
	@echo
	@echo If you don\'t know what you want, use "make jmdlx"
	@echo
	@echo When built, binaries should be in the bin dir

all: jmdlx aajm jmqt

jmdlx:
	$(MAKE) -C src/jmdlx
	mkdir -p ./bin
	cp src/jmdlx/jmdlx ./bin

aajm:
	$(MAKE) -C src/aajm
	cp src/aajm/aajm src/aajm/aajm-remote ./bin

jmqt:
	$(MAKE) -C src/jmqt
	cp src/jmqt/jmqt ./bin

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
