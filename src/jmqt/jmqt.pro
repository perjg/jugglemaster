TEMPLATE	= app
CONFIG		= qt warn_on release
LIBS		+= ../jmlib/jmlib.a ../qtkfiledialog/libqtkfiledialog.a -lqpe
HEADERS		= aboutdlg.h \
		  callback.h \
		  colordlg.h \
		  jmqt.h \
		  jugglemaster.h \
		  jugglewidget.h\
		  pattern.h \
		  patterngroup.h \
		  patternloader.h \
		  patternselectdlg.h \
		  patternselectdlgitem.h \
		  siteswapdlg.h \
		  style.h \
		  styledlg.h \
		  userpreference.h
SOURCES		= aboutdlg.cpp \
		  callback.cpp \
		  colordlg.cpp \
		  jugglemaster.cpp \
		  jugglewidget.cpp \
		  main.cpp \
		  patternloader.cpp \
		  pattern.cpp \
		  patternselectdlg.cpp \
		  patternselectdlgitem.cpp \
		  siteswapdlg.cpp \
		  style.cpp \
		  styledlg.cpp \
		  userpreference.cpp
DIST		= COPYING \
		  COPYING.JMQT
INTERFACES	=
#INCLUDEPATH += ../jmlib ../qtkfiledialog
TARGET = jmqt

