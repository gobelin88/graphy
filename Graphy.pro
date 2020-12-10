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

RC_ICONS = icons/graphy.ico

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
        2d_curve.cpp \
        2d_curveComplex.cpp \
        2d_viewer.cpp \
        3d_cameraParams.cpp \
        3d_cloud.cpp \
        3d_customViewContainer.cpp \
        3d_label.cpp \
        3d_light.cpp \
        3d_object.cpp \
        3d_objectLoader.cpp \
        3d_shapes.cpp \
        3d_viewer.cpp \
        Cloud.cpp \
        ColorWheel.cpp \
        DataConvert.cpp \
        FFT.cpp \
        FIR.cpp \
        MainWindow.cpp \
        MyCustomPlot.cpp \
        MyGradientComboBox.cpp \
        MyItemDelegate.cpp \
        MyTableModel.cpp \
        MyTableView.cpp \
        MyVariant.cpp \
        PosAtt.cpp \
        Register.cpp \
        ShapeFit.cpp \
        bode_viewer.cpp \
        complex_hack.cpp \
        main.cpp

HEADERS += \
        1d_viewer.h \
        2d_curve.h \
        2d_curveComplex.h \
        2d_viewer.h \
        3d_base.h \
        3d_cameraParams.h \
        3d_cloud.h \
        3d_customViewContainer.h \
        3d_label.h \
        3d_light.h \
        3d_object.h \
        3d_objectLoader.h \
        3d_shapes.h \
        3d_viewer.h \
        Cloud.h \
        ColorWheel.hpp \
        DataConvert.h \
        FFT.h \
        FIR.h \
        Functor.h \
        MainWindow.h \
        MyCustomPlot.h \
        MyGradientComboBox.h \
        MyItemDelegate.h \
        MyTableModel.h \
        MyTableView.h \
        MyVariant.h \
        PosAtt.h \
        Register.h \
        ShapeFit.h \
        bode_viewer.h \
        complex_hack.h \
        exprtk\exprtk.hpp

FORMS += \
        mainwindow.ui

INCLUDEPATH += $$(EIGEN_PATH)/

QMAKE_CXXFLAGS += -Dexprtk_disable_caseinsensitivity
#QMAKE_CXXFLAGS_RELEASE -= -O2
#QMAKE_CXXFLAGS_RELEASE += -O3
#QMAKE_CXXFLAGS += -O3
msvc {
  QMAKE_CXXFLAGS += -openmp
}

gcc {
  QMAKE_CXXFLAGS += -fopenmp
}


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    rsc.qrc
