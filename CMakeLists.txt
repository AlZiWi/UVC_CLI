cmake_minimum_required(VERSION 3.16)

project(UVC_CLI VERSION 0.1.0 LANGUAGES C CXX)

option(UVC_ENABLE_PREVIEW "Build the OpenCV preview window" OFF)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBUVC REQUIRED IMPORTED_TARGET libuvc)

add_executable(libuvc_util
  src/libuvc_util.c
)

target_link_libraries(libuvc_util PRIVATE PkgConfig::LIBUVC)
target_compile_features(libuvc_util PRIVATE c_std_99)

if(UVC_ENABLE_PREVIEW)
  find_package(OpenCV CONFIG REQUIRED COMPONENTS core highgui imgcodecs imgproc)

  target_sources(libuvc_util PRIVATE src/preview.cpp)
  target_link_libraries(libuvc_util PRIVATE ${OpenCV_LIBS})
  target_include_directories(libuvc_util PRIVATE ${OpenCV_INCLUDE_DIRS})
  target_compile_definitions(libuvc_util PRIVATE UVC_CLI_HAS_PREVIEW=1)
  target_compile_features(libuvc_util PRIVATE cxx_std_11)
endif()

if(APPLE)
  target_compile_options(libuvc_util PRIVATE -Wall -Wextra -Wpedantic)
endif()
