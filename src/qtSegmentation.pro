
QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qtSegmentation
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
    tools/marchingCubes.h

INCLUDEPATH += $$CIMG_PATH

RESOURCES += ressources.qrc

DEFINES += USEGL
contains(DEFINES, USEGL)
{
QT       += opengl
HEADERS  += widgets/glWidget.h
SOURCES +=  widgets/glWidget.cpp
}

#QMAKE_CXXFLAGS+= -fopenmp
#QMAKE_LFLAGS +=  -fopenmp


