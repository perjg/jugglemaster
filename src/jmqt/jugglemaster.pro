TEMPLATE	= app
CONFIG		= qt warn_on release
LIBS    += ../jmlib/jmlib.a
HEADERS		= jugglemaster.h \
		  jugglewidget.h\
            ../jmlib/jmlib.h \
		  ../jmlib/jmlib_types.h \
		  ../jmlib/juggler.h \
		  ../jmlib/prefs.h \
		  ../jmlib/util.h \
		  ../jmlib/validator.h 
SOURCES		= jugglemaster.cpp \
		  jugglewidget.cpp \
            ../jmlib/jmlib.cpp \
		  ../jmlib/prefs.cpp \
		  ../jmlib/util.cpp \
		  ../jmlib/validator.cpp \
		  main.cpp
INTERFACES	= siteswapdlg.ui siteswapadvdlg.ui styledlg.ui
INCLUDEPATH += ../jmlib
TARGET = jmqt
