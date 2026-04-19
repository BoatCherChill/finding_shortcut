cmake_minimum_required(VERSION 3.16)
project(lr_mp VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(Qt5_DIR "C:/Qt/Qt5.14.2/5.14.2/msvc2017_64/lib/cmake/Qt5")
find_package(Qt5 REQUIRED COMPONENTS Widgets)

# ВАШИ ФАЙЛЫ В ПАПКЕ lr_mp/
set(PROJECT_SOURCES
    lr_mp/main.cpp  
    lr_mp/arrow.h 
   lr_mp/arrow.cpp
   lr_mp/lr_mp.qrc
   lr_mp/diagramscene.h 
   lr_mp/diagramscene.cpp
   lr_mp/mainwindow.cpp
    lr_mp/mainwindow.h 
    lr_mp/graph.cpp
    lr_mp/graph.h
)

add_executable(lr_mp ${PROJECT_SOURCES})

target_link_libraries(lr_mp PRIVATE 
    Qt5::Widgets
)

# Автоматическое копирование Qt DLL
get_target_property(_qmake_executable Qt5::qmake IMPORTED_LOCATION)
get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")

if(WINDEPLOYQT_EXECUTABLE)
    add_custom_command(TARGET lr_mp POST_BUILD
        COMMAND "${WINDEPLOYQT_EXECUTABLE}"
                --no-compiler-runtime
                --verbose 1
                "$<TARGET_FILE:lr_mp>"
        COMMENT "Running windeployqt to copy Qt DLLs and plugins..."
    )
endif()