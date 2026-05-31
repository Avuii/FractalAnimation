cmake_minimum_required(VERSION 3.16)
project(Fraktale LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_add_executable(Fraktale
    main.cpp
    mainwindow.cpp   mainwindow.h   mainwindow.ui
    fractalwidget.cpp fractalwidget.h
    fractalworker.cpp fractalworker.h
    fractalparams.h
)

target_link_libraries(Fraktale PRIVATE Qt6::Widgets)