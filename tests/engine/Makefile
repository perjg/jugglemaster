# JuggleSaver engine tests (no GL). `make check` compares against golden.txt and runs the passing tests;
# `make record` rewrites golden.txt and is only for intentional engine changes.
JMLIB    = ../../src/jmlib
CXX     ?= clang++
CXXFLAGS = -std=c++17 -O2 -fsigned-char -DGL_SILENCE_DEPRECATION -I$(JMLIB) -I$(JMLIB)/jugglesaver
SOURCES  = $(JMLIB)/jugglesaver/engine.cpp $(JMLIB)/jugglesaver/js_passing.cpp $(JMLIB)/jugglesaver/js_validator.cpp $(JMLIB)/validator.cpp
BUILD    = build

.PHONY: check record clean
check: $(BUILD)/engine_golden $(BUILD)/passing_test
	$(BUILD)/engine_golden check ../../data/moresites.txt golden.txt
	$(BUILD)/passing_test ../../data/moresites.txt

record: $(BUILD)/engine_golden
	$(BUILD)/engine_golden record ../../data/moresites.txt golden.txt

$(BUILD)/engine_golden: engine_golden.cpp $(SOURCES) $(wildcard $(JMLIB)/jugglesaver/*.h)
	@mkdir -p $(BUILD)
	$(CXX) $(CXXFLAGS) -w engine_golden.cpp $(SOURCES) -o $@

$(BUILD)/passing_test: passing_test.cpp $(SOURCES) $(wildcard $(JMLIB)/jugglesaver/*.h)
	@mkdir -p $(BUILD)
	$(CXX) $(CXXFLAGS) -w passing_test.cpp $(SOURCES) -o $@

clean:
	rm -rf $(BUILD)
