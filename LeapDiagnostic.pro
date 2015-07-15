#-------------------------------------------------
#
# Project created by QtCreator 2015-06-29T16:15:08
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LeapDiagnostic
TEMPLATE = app


SOURCES += main.cpp\
        leapwindow.cpp \
    vtkleapscaleactor.cpp

HEADERS  += leapwindow.h \
    vtkleapscaleactor.h \
    Sample.h

FORMS    += leapwindow.ui

unix {
LIBS += -L/home/pmulumba/VTK5.10.1/bin -lQVTK -lvtkVolumeRendering -lvtkHybrid -lvtkWidgets   \
        -lvtkInfovis -lvtkGeovis -lvtkViews -lvtkCharts -lvtkFiltering -lvtkImaging \
        -lvtkGraphics -lvtkGenericFiltering -lvtkIO -lvtkRendering -lvtkCommon  \
        -lcfitsio -lLeap

INCLUDEPATH += /usr/local/include/vtk-5.10
}
