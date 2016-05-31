# Adapted from github.com/flv0/openFrameworks branch feature-cmake-config
get_filename_component(openFrameworksRoot
  "${CMAKE_CURRENT_LIST_DIR}/third-party/openFrameworks"
  ABSOLUTE
  )

# Use $ROOT/cmake/ as custom module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

set(LIB_PREFIX)
# Having to make the following distinction is a result of openFrameworks being
# not properly deployed into standard lib/include paths as of yet, but this is
# subject to change.
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  if(${CMAKE_SIZEOF_VOID_P} EQUAL 8)
    set(PLATFORM "linux64")
  else()
    set(PLATFORM "linux")
  endif()
  set(LIB_PREFIX "lib")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(PLATFORM "osx")
elseif(${WIN32})
  set(PLATFORM "vs")
endif()

if("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
  execute_process(
    COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
  if ((GCC_VERSION VERSION_GREATER 4.3 OR GCC_VERSION VERSION_EQUAL 4.3))
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
  else ()
    message(FATAL_ERROR
      "openFrameworks requires >=gcc-4.3 for C++11 support.")
  endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(CAIRO_INCLUDE_DIRS ${openFrameworksRoot}/libs/cairo/include/cairo)
  set(CAIRO_LIBRARIES
    ${openFrameworksRoot}/libs/cairo/lib/${PLATFORM}/${LIB_PREFIX}cairo-script-interpreter.a
    ${openFrameworksRoot}/libs/cairo/lib/${PLATFORM}/${LIB_PREFIX}cairo.a
    ${openFrameworksRoot}/libs/cairo/lib/${PLATFORM}/${LIB_PREFIX}pixman-1.a
    ${openFrameworksRoot}/libs/cairo/lib/${PLATFORM}/${LIB_PREFIX}png.a
    )
  set(BOOST_LIBRARIES
    ${openFrameworksRoot}/libs/boost/lib/${PLATFORM}/${LIB_PREFIX}boost.a
    ${openFrameworksRoot}/libs/boost/lib/${PLATFORM}/${LIB_PREFIX}boost_filesystem.a
    ${openFrameworksRoot}/libs/boost/lib/${PLATFORM}/${LIB_PREFIX}boost_system.a
    )
  set(FREEIMAGE_LIBRARIES
    ${openFrameworksRoot}/libs/FreeImage/lib/${PLATFORM}/${LIB_PREFIX}freeimage.a
    )
  set(FREETYPE_LIBRARIES
    ${openFrameworksRoot}/libs/freetype/lib/${PLATFORM}/${LIB_PREFIX}freetype.a
    )
  set(GLEW_LIBRARIES
    ${openFrameworksRoot}/libs/glew/lib/${PLATFORM}/${LIB_PREFIX}glew.a
    )
  set(OPENSSL_CRYPTO_LIBRARY
    ${openFrameworksRoot}/libs/openssl/lib/${PLATFORM}/${LIB_PREFIX}crypto.a
    )
  set(OPENSSL_SSL_LIBRARY
    ${openFrameworksRoot}/libs/openssl/lib/${PLATFORM}/${LIB_PREFIX}ssl.a
    )
  set(RTAUDIO_LIBRARIES
    ${openFrameworksRoot}/libs/rtAudio/lib/${PLATFORM}/${LIB_PREFIX}rtAudio.a
    )
else(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  find_package(Cairo REQUIRED)
  find_package(GStreamer REQUIRED)
  find_package(GLib REQUIRED)
  find_package(Boost REQUIRED)
  find_package(FreeImage REQUIRED)
  find_package(Freetype REQUIRED)
  find_package(GLEW REQUIRED)
  find_package(OpenSSL REQUIRED)
  find_package(RtAudio REQUIRED)
endif()

set(openFrameworks_INCLUDES
  # oF include directories
  ${openFrameworksRoot}/libs/openFrameworks
  ${openFrameworksRoot}/libs/openFrameworks/3d
  ${openFrameworksRoot}/libs/openFrameworks/app
  ${openFrameworksRoot}/libs/openFrameworks/communication
  ${openFrameworksRoot}/libs/openFrameworks/events
  ${openFrameworksRoot}/libs/openFrameworks/gl
  ${openFrameworksRoot}/libs/openFrameworks/graphics
  ${openFrameworksRoot}/libs/openFrameworks/math
  ${openFrameworksRoot}/libs/openFrameworks/sound
  ${openFrameworksRoot}/libs/openFrameworks/types
  ${openFrameworksRoot}/libs/openFrameworks/utils
  ${openFrameworksRoot}/libs/openFrameworks/video
  # oF-supplied 3rd party include directories
  ${openFrameworksRoot}/libs/FreeImage/include
  ${openFrameworksRoot}/libs/boost/include
  ${openFrameworksRoot}/libs/fmodex/include
  ${openFrameworksRoot}/libs/freetype/include
  ${openFrameworksRoot}/libs/glew/include
  ${openFrameworksRoot}/libs/glfw/include
  ${openFrameworksRoot}/libs/glu/include
  ${openFrameworksRoot}/libs/glut/include
  ${openFrameworksRoot}/libs/kiss/include
  ${openFrameworksRoot}/libs/openssl/include
  ${openFrameworksRoot}/libs/poco/include
  ${openFrameworksRoot}/libs/rtAudio/include
  ${openFrameworksRoot}/libs/tess2/include
  ${openFrameworksRoot}/libs/utf8cpp/include
  ${openFrameworksRoot}/libs/videoInput/include
  ${CAIRO_INCLUDE_DIRS}
  ${GLIB_INCLUDE_DIRS}
  ${GSTREAMER_BASE_INCLUDE_DIRS}
  ${GSTREAMER_INCLUDE_DIRS}
  # System includes
  /usr/include
  )

if(EXISTS "${openFrameworksRoot}/libs/openFrameworksCompiled/lib/${PLATFORM}/${LIB_PREFIX}openFrameworks.a")
  set(openFrameworks_STATIC
    ${openFrameworksRoot}/libs/openFrameworksCompiled/lib/${PLATFORM}/${LIB_PREFIX}openFrameworks.a)
else()
  set(openFrameworks_STATIC
    ${openFrameworksRoot}/libs/openFrameworksCompiled/lib/${PLATFORM}/${LIB_PREFIX}openFrameworksDebug.a)
endif()

set(openFrameworks_LIBRARIES
  # openFrameworks
  ${openFrameworks_STATIC}
  # oF-supplied libraries
  ${openFrameworksRoot}/libs/glfw/lib/${PLATFORM}/${LIB_PREFIX}glfw3.a
  ${openFrameworksRoot}/libs/poco/lib/${PLATFORM}/${LIB_PREFIX}PocoCrypto.a
  ${openFrameworksRoot}/libs/poco/lib/${PLATFORM}/${LIB_PREFIX}PocoData.a
  ${openFrameworksRoot}/libs/poco/lib/${PLATFORM}/${LIB_PREFIX}PocoDataSQLite.a
  ${openFrameworksRoot}/libs/poco/lib/${PLATFORM}/${LIB_PREFIX}PocoFoundation.a
  ${openFrameworksRoot}/libs/poco/lib/${PLATFORM}/${LIB_PREFIX}PocoJSON.a
  ${openFrameworksRoot}/libs/poco/lib/${PLATFORM}/${LIB_PREFIX}PocoMongoDB.a
  ${openFrameworksRoot}/libs/poco/lib/${PLATFORM}/${LIB_PREFIX}PocoNet.a
  ${openFrameworksRoot}/libs/poco/lib/${PLATFORM}/${LIB_PREFIX}PocoNetSSL.a
  ${openFrameworksRoot}/libs/poco/lib/${PLATFORM}/${LIB_PREFIX}PocoUtil.a
  ${openFrameworksRoot}/libs/poco/lib/${PLATFORM}/${LIB_PREFIX}PocoXML.a
  ${openFrameworksRoot}/libs/poco/lib/${PLATFORM}/${LIB_PREFIX}PocoZip.a
  ${openFrameworksRoot}/libs/tess2/lib/${PLATFORM}/${LIB_PREFIX}tess2.a
  # System libraries
  ${BOOST_LIBRARIES}
  ${CAIRO_LIBRARIES}
  ${FREEIMAGE_LIBRARIES}
  ${FREETYPE_LIBRARIES}
  ${GLEW_LIBRARIES}
  ${GLIB_LIBRARIES}
  ${GSTREAMER_APP_LIBRARIES}
  ${GSTREAMER_BASE_LIBRARIES}
  ${GSTREAMER_LIBRARIES}
  ${GSTREAMER_VIDEO_LIBRARIES}
  ${OPENSSL_CRYPTO_LIBRARY}
  ${OPENSSL_SSL_LIBRARY}
  ${RTAUDIO_LIBRARIES}
  -L${openFrameworksRoot}/libs/poco/lib/${PLATFORM}
  )

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  # Linux Specific
  find_library(X11_LIB X11)
  find_library(PTHREAD_LIB pthread)
  find_package(Fontconfig REQUIRED)
  find_package(GObject REQUIRED)
  find_package(GTK2 REQUIRED)
  find_package(OpenAL REQUIRED)
  find_package(OpenGL REQUIRED)

  # libraries
  list(APPEND openFrameworks_LIBRARIES
    ${openFrameworksRoot}/libs/tess2/lib/${PLATFORM}/libtess2.a
    ${openFrameworksRoot}/libs/kiss/lib/${PLATFORM}/libkiss.a
    ${FONTCONFIG_LIBRARIES}
    ${GOBJECT_LIBRARIES}
    ${OPENAL_LIBRARY}
    ${OPENGL_gl_LIBRARIES}
    ${OPENGL_glu_LIBRARIES}
    ${PTHREAD_LIB}
    ${GTK2_LIBRARIES}
    ${X11_LIB}
    "-lGL -lGLU -lglut -lXxf86vm -lXrandr -lXi -lXcursor -lsndfile"
    "-lgrt -lboost_filesystem -lboost_system"
    "-lPocoCrypto -lPocoData -lPocoFoundation -lPocoJSON"
    "-lPocoNet -lPocoUtil -lPocoXML -lPocoZip"
    )

elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  # osx-specific

  # fmodex dylib
  set(fmodex_LIBRARY ${openFrameworksRoot}/libs/fmodex/lib/${PLATFORM}/libfmodex.dylib)

  # All includes
  set(openFrameworks_INCLUDES
    ${openFrameworks_INCLUDES}
    ${openFrameworksRoot}/libs/fmodex/include
    )

  # All libraries
  set(openFrameworks_LIBRARIES
    ${openFrameworks_LIBRARIES}
    ${fmodex_LIBRARY}
    "-framework Accelerate -framework QTKit -framework GLUT -framework AGL"
    "-framework ApplicationServices -framework AudioToolbox -framework CoreAudio"
    "-framework CoreFoundation -framework CoreServices -framework OpenGL"
    "-framework IOKit -framework Cocoa -framework CoreVideo"
    "-framework AVFoundation -framework CoreMedia -framework QuartzCore"
    )
endif()

message("** openFrameworks configured in ${openFrameworksRoot}")
