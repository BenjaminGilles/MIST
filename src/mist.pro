
QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MIST
TEMPLATE = app

CIMG_PATH = extlibs/CImg

SOURCES += main.cpp\
        mainwindow.cpp \
    imageview.cpp \
    widgets/mprlayout.cpp \
    widgets/qSlice.cpp

HEADERS  += mainwindow.h\
        $$CIMG_PATH/CImg.h \
    imageview.h \
    image.h \
    cimgTools.h \
    widgets/mprlayout.h \
    widgets/qSlice.h \
    tools/baseTool.h \
    tools/labelTable.h \
    tools/imageTool.h \
    tools/morpho.h \
    tools/brush.h \
    tools/regionGrowing.h \
    tools/landmarks.h \
    tools/marchingCubes.h \
    tools/segmentation3D.h

INCLUDEPATH += $$CIMG_PATH

RESOURCES += ressources.qrc

DEFINES -= USEGL
contains(DEFINES, USEGL) {
    message( "opengl used..." )
    QT       += opengl
    HEADERS  += widgets/glWidget.h
    SOURCES  += widgets/glWidget.cpp
}

LIBS += -lz

#QMAKE_CXXFLAGS+= -fopenmp
#QMAKE_LFLAGS +=  -fopenmp


