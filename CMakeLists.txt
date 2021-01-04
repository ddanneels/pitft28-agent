cmake_minimum_required(VERSION 3.0.0)
project(pitft_agent)

file(GLOB_RECURSE SRC src/*.c)

file(GLOB_RECURSE HEADERS src/*.h)

include_directories(src)

set(TARGET pitft_agent)

add_executable(${TARGET} ${SRC})


find_package(PkgConfig)

# pkg_check_modules (GLIB2  REQUIRED glib-2.0)
# if (GLIB2_FOUND)
#   include_directories (${GLIB2_INCLUDE_DIRS})
#   target_link_libraries (${TARGET} ${GLIB2_LIBRARIES})
#   link_directories (${GLIB2_LIBRARY_DIRS})
# else ()
#   message (FATAL_ERROR "GLib2 not found")
# endif ()

# target_link_options(${TARGET} PUBLIC "-lwiringPi")
 
find_package( Threads REQUIRED)
if (THREADS_FOUND)
  target_link_libraries (${TARGET} "${CMAKE_THREAD_LIBS_INIT}")
endif ()

# pkg_check_modules (EVDEV  REQUIRED libevdev)
# if (EVDEV_FOUND)
#   include_directories (${EVDEV_INCLUDE_DIRS})
#   target_link_libraries (${TARGET} ${EVDEV_LIBRARIES})
#   link_directories (${EVDEV_LIBRARY_DIRS})
# else ()
#   message (FATAL_ERROR "libevdev not found")
# endif ()

pkg_check_modules (BLUEZ  REQUIRED bluez)
if(BLUEZ_FOUND)
  include_directories(${BLUEZ_INCLUDE_DIRS})
  target_link_libraries(${TARGET} ${BLUEZ_LIBRARIES})
  link_directories(${BLUEZ_LIBRARY_DIRS})
else()
  message(FATAL_ERROR "Bluez not found")
endif()

pkg_check_modules (SDL2  REQUIRED sdl2)
if(SDL2_FOUND)
  include_directories(${SDL2_INCLUDE_DIRS})
  target_link_libraries(${TARGET} ${SDL2_LIBRARIES})
  link_directories(${SDL2_LIBRARY_DIRS})
else()
  message(FATAL_ERROR "SDL2 not found")
endif()

 
 

