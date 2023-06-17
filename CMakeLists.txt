cmake_minimum_required(VERSION 3.5)

project(test_qwt_slide LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check http://doc.qt.io/qt-5/deployment-android.html for more information.
# They need to be set before the find_package(Qt5 ...) call.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

find_package(Qt5 COMPONENTS Widgets REQUIRED)

if(ANDROID)
  add_library(test_qwt_slide SHARED
    main.cpp
    widget.cpp
    widget.h
    widget.ui
  )
else()
  add_executable(test_qwt_slide
    main.cpp
    widget.cpp
    widget.h
    widget.ui
    SliderBox.cpp
    SliderBox.h
  )
endif()



set(QWT_VER_MAJ 6)
set(QWT_VER_MIN 2)
set(QWT_VER_PAT 0)
set(QWT_VERSION ${QWT_VER_MAJ}.${QWT_VER_MIN}.${QWT_VER_PAT})
if(WIN32)
    message("Detected Windows platform")
    # Windows-specific options
    set(QWT_INSTALL_PREFIX   C:/Qwt-${QWT_VERSION})
else()
    message("Detected non-Windows platform")
    # Non-Windows options
endif()
set(QWT_INSTALL_LIBS  ${QWT_INSTALL_PREFIX}/lib)
set(QWT_INSTALL_HEADERS  ${QWT_INSTALL_PREFIX}/include)
message("QWT_INSTALL_LIBS is ${QWT_INSTALL_LIBS}")
message("QWT_INSTALL_HEADERS is ${QWT_INSTALL_HEADERS}")

#find_library( QWT NAMES qwt  PATHS QWT_INSTALL_LIBS REQUIRED )
find_library(QWT NAMES "qwtd" PATHS ${QWT_INSTALL_LIBS} REQUIRED)
if(QWT)
    include_directories(${QWT_INSTALL_HEADERS})
    link_directories(${QWT_INSTALL_LIBS})
    message("QWT found: ${QWT}")
endif()
target_link_libraries(test_qwt_slide PRIVATE   Qt5::Widgets ${QWT})
