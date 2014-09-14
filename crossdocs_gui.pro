QT          += core gui webkitwidgets network widgets
CONFIG      += thread
TARGET      =  crossdocs_gui
QMAKE_CXXFLAGS += -std=c++11

SOURCES     += \
            main.cpp \
            projectworker.cpp \
            cdcmainwindow.cpp \
            configurationfileparser.cpp \
            documentworker.cpp \
            inputfileparser.cpp \
            cdchighlighter.cpp \
            cdccodeeditor.cpp \
            abstractbuilder.cpp \
    doxygenbuilder.cpp

HEADERS     += \
            projectworker.h \
            cdcmainwindow.h \
            cdcdefs.h \
            configurationfileparser.h \
            documentworker.h \
            inputfileparser.h \
            cdchighlighter.h \
            cdccodeeditor.h \
            abstractbuilder.h \
    doxygenbuilder.h

RESOURCES   += rsr/resources.qrc

OTHER_FILES += \
    doc/pages.dox \
    doc/Doxyfile
