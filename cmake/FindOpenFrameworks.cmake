# Adapted from github.com/flv0/openFrameworks branch feature-cmake-config.
#
# Prerequisite: assume the variable ${openFrameworksRoot} is configured and we
#     can use the variable ${ARCH} for the architecture.
#
# Effect: This will configure ${openFrameworks_INCLUDE_DIRS} and
#     ${openFrameworks_LIBRARIES}.
#

# =============================================================
#   Populate necessary variables PLATFORM and LIB_PREFIX
# =============================================================
set(LIB_PREFIX "lib")
if(APPLE)
  set(PLATFORM "osx")
  set(LIB_PREFIX "")
elseif(UNIX AND ${ARCH} STREQUAL "x86_64")
  set(PLATFORM "linux64")
elseif(UNIX AND ${ARCH} STREQUAL "x86_32")
  set(PLATFORM "linux")
elseif(UNIX AND ${ARCH} STREQUAL "armv6")
  set(PLATFORM "linuxarmv6l")
elseif(UNIX AND ${ARCH} STREQUAL "armv7")
  set(PLATFORM "linuxarmv6l")
elseif(MSVC)
  set(PLATFORM "windows")
endif()

# =============================================================
#   Get platform-dependent library right
# =============================================================
if(${PLATFORM} STREQUAL "osx")
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

# =============================================================
#   Configure include directories
# =============================================================
set(openFrameworks_INCLUDE_DIRS
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

# =============================================================
#   Configure openFrameworks static library
#   Prefer to use release to debug library
# =============================================================
if(EXISTS "${openFrameworksRoot}/libs/openFrameworksCompiled/lib/${PLATFORM}/${LIB_PREFIX}openFrameworks.a")
  set(openFrameworks_STATIC
    ${openFrameworksRoot}/libs/openFrameworksCompiled/lib/${PLATFORM}/${LIB_PREFIX}openFrameworks.a)
else()
  set(openFrameworks_STATIC
    ${openFrameworksRoot}/libs/openFrameworksCompiled/lib/${PLATFORM}/${LIB_PREFIX}openFrameworksDebug.a)
endif()

# =============================================================
#   Configure openFrameworks_LIBRARIES
# =============================================================

set(GLFW3_LIB ${openFrameworksRoot}/libs/glfw/lib/${PLATFORM}/${LIB_PREFIX}glfw3.a)
if(${PLATFORM} STREQUAL "linuxarmv6l")
  # overwrite the ${PLATFORM} linuxarmv6l to linux64
  set(GLFW3_LIB ${openFrameworksRoot}/libs/glfw/lib/linux64/${LIB_PREFIX}glfw3.a)
endif()

set(openFrameworks_LIBRARIES
  # openFrameworks
  ${openFrameworks_STATIC}
  # oF-supplied libraries
  ${GLFW3_LIB}
  ${openFrameworksRoot}/libs/poco/lib/${PLATFORM}/${LIB_PREFIX}PocoCrypto.a
  ${openFrameworksRoot}/libs/poco/lib/${PLATFORM}/${LIB_PREFIX}PocoData.a
  ${openFrameworksRoot}/libs/poco/lib/${PLATFORM}/${LIB_PREFIX}PocoFoundation.a
  ${openFrameworksRoot}/libs/poco/lib/${PLATFORM}/${LIB_PREFIX}PocoJSON.a
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

# =============================================================
#   Final touch to update more libraries
# =============================================================
if(APPLE)
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
elseif(UNIX)
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
endif()

if(${PLATFORM} STREQUAL "linuxarmv6l")
  list(APPEND openFrameworks_LIBRARIES
    "-ldl -lEGL -lGLESv2 -ludev"
    "-L/opt/vc/lib/ -lbcm_host"
    )
endif()

message("** openFrameworks configured in ${openFrameworksRoot}")
