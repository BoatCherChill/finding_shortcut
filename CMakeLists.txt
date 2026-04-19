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

# Файлы в папке src/ (ВНИМАНИЕ: пути без lr_mp/)
set(PROJECT_SOURCES
    src/main.cpp  
    src/arrow.h 
    src/arrow.cpp
    src/diagramscene.h 
    src/diagramscene.cpp
    src/mainwindow.cpp
    src/mainwindow.h 
    src/mainwindow.ui
    src/graph.cpp
    src/graph.h
    
)

# Файл ресурсов (если есть)
# set(RESOURCES
#     src/lr_mp.qrc
# )

add_executable(lr_mp ${PROJECT_SOURCES}) # ${RESOURCES}

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