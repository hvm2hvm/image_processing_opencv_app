cmake_minimum_required(VERSION 3.7)
project(opencv_app)

set(CMAKE_CXX_STANDARD 14)

FIND_PACKAGE(Qt4 REQUIRED)
find_package(OpenCV REQUIRED)
INCLUDE(${QT_USE_FILE})

# include_directories(/usr/include/qt4)

set(SOURCE_FILES
        opencv_app.cpp
        tools.cpp
        labs/lab1.cpp
        labs/lab2.cpp
        labs/lab3.cpp
        labs/lab4.cpp
        labs/lab5.cpp
        labs/lab6.cpp
        )

add_executable(opencv_app ${SOURCE_FILES})

TARGET_LINK_LIBRARIES(opencv_app ${QT_LIBRARIES} ${OpenCV_LIBS})