cmake_minimum_required(VERSION 3.5)

project(graphy LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(GRAPHY_VERSION 11)
set(TARGET_NAME "graphy${GRAPHY_VERSION}")

list(APPEND CMAKE_PREFIX_PATH "C:/Tools/Eigen3")


find_package (Eigen3 3.3 REQUIRED)
find_package(Qt5 COMPONENTS Widgets 3DCore 3DRender 3DExtras Concurrent PrintSupport REQUIRED)

set(SOURCES
    ModelCurveInterface.h
    FitDialog.cpp
    FitDialog.h
    BrushStyleComboBox.cpp
    BrushStyleComboBox.h
    PenStyleComboBox.cpp
    PenStyleComboBox.h
    HeadComboBox.cpp
    HeadComboBox.h
    ScatterComboBox.cpp
    ScatterComboBox.h
    AppearanceDialog.cpp
    AppearanceDialog.h
    1d_viewer.cpp
    2d_curve.cpp
    2d_curveComplex.cpp
    3d_cameraParams.cpp
    3d_cloud.cpp
    3d_customViewContainer.cpp
    3d_label.cpp
    3d_light.cpp
    3d_object.cpp
    3d_objectLoader.cpp
    3d_shapes.cpp
    3d_viewer.cpp
    Cloud.cpp
    ColorWheel.cpp
    DataConvert.cpp
    FFT.cpp
    FIR.cpp
    FilterDialog.cpp
    MainWindow.cpp
    MyCustomPlot.cpp
    MyGradientComboBox.cpp
    MyHighLighter.cpp
    MyItemDelegate.cpp
    MyLabel.cpp
    MyProgressHandler.cpp
    MySciDoubleSpinBox.cpp
    MyTableModel.cpp
    MyTableView.cpp
    MyTextEdit.cpp
    MyVariant.cpp
    PosAtt.cpp
    Register.cpp
    WinCPURAM.cpp
    bode_viewer.cpp
    complex_hack.cpp
    exprtk/Faddeeva.cc
    main.cpp
    shapes/Circle2D.cpp
    shapes/Circle3D.cpp
    shapes/CustomExpression.cpp
    shapes/Ellipse2D.cpp
    shapes/Gaussian.cpp
    shapes/Sigmoid.cpp
    single_application/singleapplication.cpp
    single_application/singleapplication_p.cpp
    1d_viewer.h
    2d_curve.h
    2d_curveComplex.h
    3d_base.h
    3d_cameraParams.h
    3d_cloud.h
    3d_customViewContainer.h
    3d_label.h
    3d_light.h
    3d_object.h
    3d_objectLoader.h
    3d_shapes.h
    3d_viewer.h
    Cloud.h
    ColorWheel.hpp
    DataConvert.h
    ExprtkCustomFunctions.hpp
    FFT.h
    FIR.h
    FilterDialog.h
    Functor.h
    MainWindow.h
    MyCustomPlot.h
    MyGradientComboBox.h
    MyHighLighter.h
    MyItemDelegate.h
    MyLabel.h
    MyProgressHandler.h
    MySciDoubleSpinBox.h
    MyTableModel.h
    MyTableView.h
    MyTextEdit.h
    MyVariant.h
    PosAtt.h
    Register.h
    WinCPURAM.h
    bode_viewer.h
    complex_hack.h
    exprtk/Faddeeva.hh
    exprtk/exprtk.hpp
    shapes/Circle2D.h
    shapes/Circle3D.h
    shapes/CustomExpression.h
    shapes/Ellipse2D.h
    shapes/Ellipsoid.hpp
    shapes/Gaussian.h
    shapes/Impedances.hpp
    shapes/Plan.hpp
    shapes/Shape.hpp
    shapes/Sigmoid.h
    shapes/Sinusoide.cpp
    shapes/Sinusoide.h
    shapes/Sphere.hpp
    shapes/SolidHarmonicsDecomposition.cpp
    shapes/SolidHarmonicsDecomposition.h
    single_application/singleapplication.h
    single_application/singleapplication_p.h
    mainwindow.ui
    algorithmes/Horn.cpp
    algorithmes/Horn.h
    algorithmes/Arun.cpp
    algorithmes/Arun.h
    algorithmes/Pivot.cpp
    algorithmes/Pivot.h
    solidHarmonics/solidHarmonics.h
    solidHarmonics/solidHarmonics.cpp
    icon.rc
    rsc.qrc
    )

add_executable(${TARGET_NAME} ${SOURCES})
target_compile_features(${TARGET_NAME} PRIVATE cxx_std_17)

string(REGEX REPLACE "/W[3|4]" "/w" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
target_compile_options(${TARGET_NAME} PRIVATE /bigobj /we4456 /wd4127 /DgraphyVersion=${GRAPHY_VERSION} /Dexprtk_disable_caseinsensitivity /D__STDCPP_WANT_MATH_SPEC_FUNCS__)

target_include_directories(${TARGET_NAME} PRIVATE ${EIGEN3_INCLUDE_DIR})

target_link_libraries(${TARGET_NAME} PRIVATE Qt5::Widgets Qt5::3DCore Qt5::3DRender Qt5::3DExtras Qt5::Concurrent Qt5::PrintSupport)
target_link_libraries (${TARGET_NAME} PRIVATE Eigen3::Eigen)

install (TARGETS ${TARGET_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

install (FILES
#         $<TARGET_RUNTIME_DLLS:graphy>
         ${_qt5Core_install_prefix}/bin/Qt5Core.dll
         ${PROJECT_BINARY_DIR}/shortcuts.csv
         $<TARGET_FILE:Qt5::Widgets>
         $<TARGET_FILE:Qt5::3DRender>
         $<TARGET_FILE:Qt5::3DExtras>
         $<TARGET_FILE:Qt5::Concurrent>
         $<TARGET_FILE:Qt5::PrintSupport>
         CONFIGURATIONS Release
         DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
    )

install (DIRECTORY ${_qt5Core_install_prefix}/plugins/platforms DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
install (DIRECTORY ${_qt5Core_install_prefix}/plugins/styles DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
install (DIRECTORY ${_qt5Core_install_prefix}/plugins/imageformats DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
install (DIRECTORY ${_qt5Core_install_prefix}/plugins/printsupport DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)


message("CMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}")
message("PROJECT_BINARY_DIR=${PROJECT_BINARY_DIR}")
message("_qt5Core_install_prefix=${_qt5Core_install_prefix}")
