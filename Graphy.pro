#-------------------------------------------------
#
# Project created by QtCreator 2019-04-30T08:23:38
#
#-------------------------------------------------

QT+= 3dcore 3drender 3dinput 3dextras datavisualization
QT       += core gui printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Graphy
TEMPLATE = app

QMAKE_CXXFLAGS += -bigobj

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        1d_viewer.cpp \
        1d_cplx_viewer.cpp \
        2d_viewer.cpp \
        3d_viewer.cpp \
        Cloud.cpp \
        FIR.cpp \
        ScatterModifier.cpp \
        SurfaceModifier.cpp \
        ShapeFit.cpp \
        cameraparams.cpp \
        cloud3d.cpp \
        color_wheel.cpp \
        curve2d.cpp \
        curve2d_gainphase.cpp \
        customviewcontainer.cpp \
        grid3d.cpp \
        label3d.cpp \
        light3d.cpp \
        main.cpp \
        mainwindow.cpp \
        obj.cpp \
        object3d.cpp \
        qcustomplot.cpp \
        tabledata.cpp \
        view3d.cpp \
        qPosAtt.cpp

HEADERS += \
        1d_viewer.h \
        1d_cplx_viewer.h \
        2d_viewer.h \
        3d_viewer.h \
        Base3D.h \
        Cloud.h \
        FIR.h \
        Functor.h \
        ScatterModifier.h \
        SurfaceModifier.h \
        ShapeFit.h \
        box.h \
        boxplot.h \
        cameraparams.h \
        cloud3d.h \
        color_wheel.hpp \
        curve2d.h \
        curve2d_gainphase.h \
        customviewcontainer.h \
        grid3d.h \
        label3d.h \
        light3d.h \
        mainwindow.h \
        obj.h \
        object3d.h \
        qcustomplot.h \
        tabledata.h \
        view3d.h \
        exprtk\exprtk.hpp \
        qPosAtt.h

FORMS += \
        mainwindow.ui

INCLUDEPATH += $$(EIGEN_PATH)/

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    rsc.qrc
