# Adapted from github.com/flv0/openFrameworks branch feature-cmake-config
get_filename_component(openFrameworksRoot
  "${CMAKE_CURRENT_LIST_DIR}/third-party/openFrameworks"
  ABSOLUTE
  )

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

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
  execute_process(
    COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
  if ((GCC_VERSION VERSION_GREATER 4.3 OR GCC_VERSION VERSION_EQUAL 4.3))
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
  else()
    message(FATAL_ERROR
      "openFrameworks requires >=gcc-4.3 for C++11 support.")
  endif ()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif ()

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
  ${openFrameworksRoot}/libs/cairo/include/cairo
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
  )

set(openFrameworks_LIBRARIES
  # openFrameworks
  ${openFrameworksRoot}/libs/openFrameworksCompiled/lib/${PLATFORM}/libopenFrameworks.a
  # oF-supplied libraries
  ${openFrameworksRoot}/libs/boost/lib/${PLATFORM}/${LIB_PREFIX}boost.a
  ${openFrameworksRoot}/libs/boost/lib/${PLATFORM}/${LIB_PREFIX}boost_filesystem.a
  ${openFrameworksRoot}/libs/boost/lib/${PLATFORM}/${LIB_PREFIX}boost_system.a
  ${openFrameworksRoot}/libs/cairo/lib/${PLATFORM}/${LIB_PREFIX}cairo-script-interpreter.a
  ${openFrameworksRoot}/libs/cairo/lib/${PLATFORM}/${LIB_PREFIX}cairo.a
  ${openFrameworksRoot}/libs/cairo/lib/${PLATFORM}/${LIB_PREFIX}pixman-1.a
  ${openFrameworksRoot}/libs/cairo/lib/${PLATFORM}/${LIB_PREFIX}png.a
  ${openFrameworksRoot}/libs/FreeImage/lib/${PLATFORM}/${LIB_PREFIX}freeimage.a
  ${openFrameworksRoot}/libs/freetype/lib/${PLATFORM}/${LIB_PREFIX}freetype.a
  ${openFrameworksRoot}/libs/glew/lib/${PLATFORM}/${LIB_PREFIX}glew.a
  ${openFrameworksRoot}/libs/glfw/lib/${PLATFORM}/${LIB_PREFIX}glfw3.a
  ${openFrameworksRoot}/libs/openssl/lib/${PLATFORM}/${LIB_PREFIX}crypto.a
  ${openFrameworksRoot}/libs/openssl/lib/${PLATFORM}/${LIB_PREFIX}ssl.a
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
  ${openFrameworksRoot}/libs/rtAudio/lib/${PLATFORM}/${LIB_PREFIX}rtAudio.a
  ${openFrameworksRoot}/libs/tess2/lib/${PLATFORM}/${LIB_PREFIX}tess2.a
  )

# osx-specific
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  # fmodex dylib
  set(fmodex_LIBRARY ${openFrameworksRoot}/libs/fmodex/lib/${PLATFORM}/libfmodex.dylib)

  # All includes
  set(openFrameworks_INCLUDES
    ${openFrameworks_INCLUDES}
    ${openFrameworksRoot}/libs/fmodex/include
    )

  # All libraries
  set(openFrameworks_LIBRARIES
    ${fmodex_LIBRARY}
    ${openFrameworks_LIBRARIES}
    "-framework Accelerate -framework QTKit -framework GLUT -framework AGL"
    "-framework ApplicationServices -framework AudioToolbox -framework CoreAudio"
    "-framework CoreFoundation -framework CoreServices -framework OpenGL"
    "-framework IOKit -framework Cocoa -framework CoreVideo"
    "-framework AVFoundation -framework CoreMedia -framework QuartzCore"
    )
endif()

message("** openFrameworks configured in ${openFrameworksRoot}")
