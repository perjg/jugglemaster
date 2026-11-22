include ../../Makefile.cfg

CXXFLAGS+=-Wall -pedantic -fsigned-char
CFLAGS+=-Wall -pedantic -fsigned-char
LIBS+=-lm -ldl -lsqlite3

# Silence OpenGL deprecation warnings on macOS (OpenGL still works, Apple just prefers Metal)
CXXFLAGS+=-DGL_SILENCE_DEPRECATION
CFLAGS+=-DGL_SILENCE_DEPRECATION

# Enable JuggleSaver support when OpenGL is enabled
ifeq ($(OPENGL_SUPPORT), 1)
CXXFLAGS+=-DJUGGLESAVER_SUPPORT
CFLAGS+=-DJUGGLESAVER_SUPPORT
endif

# If building with USE_SHADERS, add the flag
ifeq ($(USE_SHADERS), 1)
CXXFLAGS+=-DUSE_SHADER_RENDERING
CFLAGS+=-DUSE_SHADER_RENDERING
endif

ifneq ($(DEBUG), 1)
# Optimization flags (if not debug)
CXXFLAGS+=-O2
CFLAGS+=-O2
endif

ifeq ($(DEBUG), 1)
CXXFLAGS+=-g -Werror
endif

DATA=../../data/patterns.jm ../../data/semaphore.jm
OBJ=jmlib.o prefs.o util.o validator.o patterns.o randompatt.o sqlite_patterns.o

# JuggleSaver (3D OpenGL) support files
JUGGLESAVER_OBJ=jmlib_wrapper.o render_jm_flat.o jugglesaver/engine.o jugglesaver/gltrackball.o jugglesaver/jmlib_jsaver.o \
                jugglesaver/js_patterns.o jugglesaver/js_validator.o \
                jugglesaver/trackball.o jugglesaver/render_common.o

ifeq ($(USE_SHADERS), 1)
JUGGLESAVER_OBJ+=jugglesaver/render_shader.o
else
JUGGLESAVER_OBJ+=jugglesaver/render_legacy.o
endif

# Add shader_manager for Shader support
ifeq ($(USE_SHADERS), 1)
JUGGLESAVER_OBJ+=jugglesaver/shader_manager.o
endif

# Check if parent build requested OpenGL support
ifeq ($(OPENGL_SUPPORT), 1)
OBJ+=$(JUGGLESAVER_OBJ)
endif

LIBNAME=libjm.a
all: $(LIBNAME)

$(LIBNAME): $(OBJ)
	ar -r $(LIBNAME) $(OBJ)
	ranlib $(LIBNAME)

patterns:
	$(CXX) $(CXXFLAGS) -DPATTERNS_STANDALONE -c -o ./patterns-standalone.o ./patterns.cpp
	$(CXX) $(LDFLAGS) -o ./patterns ./patterns-standalone.o $(LIBS)

rand: clean validator.o
	$(CXX) $(CXXFLAGS) -DRAND_STANDALONE -c -o ./rand-standalone.o ./randompatt.cpp
	$(CXX) $(LDFLAGS) -o ./rand ./rand-standalone.o validator.o $(LIBS)
	./rand | less

validator: clean
	$(CXX) $(CXXFLAGS) -DVALIDATOR_STANDALONE -c -o ./validator-standalone.o ./validator.cpp
	$(CXX) $(LDFLAGS) -o ./validator ./validator-standalone.o $(LIBS)

install: $(LIBNAME)
	mkdir -p /usr/local/include/jm
	cp *.h /usr/local/include/jm/
	cp libjm.a /usr/local/lib

clean:
	rm -f *.o core $(LIBNAME) patterns rand validator
	rm -f jugglesaver/*.o

p: clean patterns
	./patterns $(DATA)

v: clean patterns
	valgrind --leak-check=yes ./patterns -q $(DATA)
