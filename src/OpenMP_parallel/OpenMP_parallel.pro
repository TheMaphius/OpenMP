#-------------------------------------------------
#
# Project created by QtCreator 2015-05-21T14:58:55
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = OpenMP
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    menu.cpp \
    histogram.cpp \
    showimages.cpp

HEADERS  += mainwindow.h \
    menu.h \
    histogram.h \
    showimages.h

INCLUDEPATH += /usr/local/include/opencv2

LIBS += -L/usr/local/lib
LIBS += -lopencv_core
LIBS += -lopencv_imgproc
LIBS += -lopencv_highgui
LIBS += -lopencv_ml
LIBS += -lopencv_video
LIBS += -lopencv_features2d
LIBS += -lopencv_calib3d
LIBS += -lopencv_objdetect
LIBS += -lopencv_contrib
LIBS += -lopencv_legacy
LIBS += -lopencv_flann
#LIBS += -lopencv_nonfree

QMAKE_CXXFLAGS += -fopenmp
QMAKE_LFLAGS += -fopenmp
