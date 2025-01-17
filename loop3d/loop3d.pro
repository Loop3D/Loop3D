QT += quick qml
QT += positioning
QT += 3drender gui opengl
QT += 3dcore 3drender 3dinput

QT_OPENGL=desktop

CONFIG += c++11 no_keywords

win32 {
    CONFIG(release, debug|release) {
        CONFIG += console
#        DEFINES += QT_NO_DEBUG_OUTPUT
    }
}

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Refer to the documentation for the
# deprecated API to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

linux {
    INCLUDEPATH += ~/.local/include/python3.8m /usr/include/python3.8m
    TARGET = loop3d
} else :win32 {
    INCLUDEPATH += $$(VCPKGINSTALL)/include
    INCLUDEPATH += $$(PYTHONHOME)/include
    INCLUDEPATH += $$(PYTHONHOME)/Library/include
    INCLUDEPATH += $$(LOOPPROJECTFILECPP)
}

linux {
    LIBS += -lpython3.7m -lnetcdf -lnetcdf_c++4
} else :win32 {
    LIBS += -L$$(LOOPPROJECTFILECPP)
    CONFIG(debug, debug|release) {
        LIBS += -L$$(VCPKGINSTALL)/debug/lib
        LIBS += -lLoopProjectFileCppD
    }
    CONFIG(release, debug|release) {
        LIBS += -L$$(VCPKGINSTALL)/lib
        LIBS += -lLoopProjectFileCpp
    }
    LIBS += -L$$(PYTHONHOME)/libs
    LIBS += -L$$(PYTHONHOME)/Lib
    LIBS += -L$$(PYTHONHOME)/Lib/site-packages
    LIBS += -L$$(PYTHONHOME)
    LIBS += -lnetcdf -lnetcdf-cxx4 -lpython39
}

SOURCES += \
        3dviewer.cpp \
        datasourcelist.cpp \
        datasourcemodel.cpp \
        eventlist.cpp \
        eventmodel.cpp \
        eventpermutation.cpp \
        lsconfig.cpp \
        m2lconfig.cpp \
        main.cpp \
        observationlist.cpp \
        observationmodel.cpp \
        projectmanagement.cpp \
        pythonhighlighter.cpp \
        pythontext.cpp \
        structuralmodel.cpp \
        tokenise.cpp \
        utmconverter.cpp

RESOURCES += qml.qrc qtquickcontrols2.conf \
    3dassets.qrc \
    images.qrc \
    shaders.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    3dviewer.h \
    datasourcelist.h \
    datasourcemodel.h \
    eventlist.h \
    eventmodel.h \
    eventpermutation.h \
    lsconfig.h \
    m2lconfig.h \
    observationlist.h \
    observationmodel.h \
    projectmanagement.h \
    pythonhighlighter.h \
    pythontext.h \
    structuralmodel.h \
    tokenise.h \
    utmconverter.h

DISTFILES += \
    ../DataSource.conf \
    ../test.py \
    isosurface.frag \
    isosurface.vert \
    qtquickcontrols2.conf
