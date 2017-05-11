cmake_minimum_required(VERSION 3.7)
project(opencv_app)

set(CMAKE_CXX_STANDARD 14)

find_package(Qt5 REQUIRED COMPONENTS Core Widgets Gui)

set(CMAKE_AUTOMOC ON)
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ";" ${QT5_PACKAGE_DIR})

find_package(OpenCV REQUIRED)

include_directories(/usr/include/qt5)

set(SOURCE_FILES
        opencv_app.cpp
        project.cpp
        tools.cpp
        labs/lab1.cpp
        labs/lab2.cpp
        labs/lab3.cpp
        labs/lab4.cpp
        labs/lab5.cpp
        labs/lab6.cpp
        labs/lab7.cpp
        labs/lab8.cpp
        labs/lab9.cpp
        labs/lab10.cpp
        labs/lab11.cpp
        )

add_executable(opencv_app ${SOURCE_FILES})

TARGET_LINK_LIBRARIES(opencv_app Qt5::Core Qt5::Widgets Qt5::Gui ${OpenCV_LIBS})
