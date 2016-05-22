set(PROJECT ESP)

project(${PROJECT})
cmake_minimum_required(VERSION 3.0)

# =============================================================
#   openFrameworks
# =============================================================
set(OF_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/third-party/openFrameworks)
set(openFrameworks_DIR ${CMAKE_CURRENT_SOURCE_DIR})
find_package(openFrameworks REQUIRED)

# =============================================================
#   Add-ons
# =============================================================
set(ADDONS_PATH ${OF_ROOT}/addons)

set(OFX_DATGUI_SRC
  ${ADDONS_PATH}/ofxDatGui/src/core/ofxDatGuiComponent.cpp
  ${ADDONS_PATH}/ofxDatGui/src/libs/ofxSmartFont/ofxSmartFont.cpp
  ${ADDONS_PATH}/ofxDatGui/src/ofxDatGui.cpp
  )

set(OFX_PARAGRAPH_SRC
  ${ADDONS_PATH}/ofxParagraph/src/ofxParagraph.cpp
  )

set(OFX_GRT_SRC
  ${ADDONS_PATH}/ofxGrt/src/ofxGrtTimeseriesPlot.cpp
  )

set(OFX_GUI_SRC
  ${ADDONS_PATH}/ofxGui/src/ofxBaseGui.cpp
  ${ADDONS_PATH}/ofxGui/src/ofxButton.cpp
  ${ADDONS_PATH}/ofxGui/src/ofxGuiGroup.cpp
  ${ADDONS_PATH}/ofxGui/src/ofxLabel.cpp
  ${ADDONS_PATH}/ofxGui/src/ofxPanel.cpp
  ${ADDONS_PATH}/ofxGui/src/ofxSlider.cpp
  ${ADDONS_PATH}/ofxGui/src/ofxSliderGroup.cpp
  ${ADDONS_PATH}/ofxGui/src/ofxToggle.cpp
  )

list(APPEND ADDONS_SRC
  ${OFX_DATGUI_SRC}
  ${OFX_GRT_SRC}
  ${OFX_GUI_SRC}
  ${OFX_PARAGRAPH_SRC}
  )

list(APPEND ADDONS_INCLUDE_PATH
  ${ADDONS_PATH}/ofxDatGui/src
  ${ADDONS_PATH}/ofxDatGui/src/core
  ${ADDONS_PATH}/ofxDatGui/src/components
  ${ADDONS_PATH}/ofxDatGui/src/themes
  ${ADDONS_PATH}/ofxDatGui/src/libs/ofxSmartFont
  ${ADDONS_PATH}/ofxGui/src
  ${ADDONS_PATH}/ofxGrt/src
  ${ADDONS_PATH}/ofxParagraph/src
  ${ADDONS_PATH}/ofxNetwork/src
  )

# =============================================================
#   ESP sources
# =============================================================
set(ESP_PATH ${CMAKE_CURRENT_SOURCE_DIR}/Xcode/ESP)
set(ESP_SRC
  ${ESP_PATH}/src/calibrator.cpp
  ${ESP_PATH}/src/iostream.cpp
  ${ESP_PATH}/src/istream.cpp
  ${ESP_PATH}/src/main.cpp
  ${ESP_PATH}/src/ofApp.cpp
  ${ESP_PATH}/src/ostream.cpp
  ${ESP_PATH}/src/plotter.cpp
  ${ESP_PATH}/src/tuneable.cpp
  ${ESP_PATH}/src/user.cpp
)

set(GRT_LIBS ${CMAKE_CURRENT_SOURCE_DIR}/Xcode/ESP/libgrt.dylib "-lgrt")
set(GRT_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/third-party/grt/)
set(SYS_LIBS "-lobjc" "-L/usr/local/lib")

# =============================================================
#   Create executables
# =============================================================
add_executable(${PROJECT} ${ADDONS_SRC} ${ESP_SRC})

target_include_directories(${PROJECT} PUBLIC
  ${openFrameworks_INCLUDES}
  ${ADDONS_INCLUDE_PATH}
  ${ESP_PATH}/src
  ${GRT_INCLUDES}
  )

target_link_libraries(${PROJECT} PUBLIC
  ${openFrameworks_LIBRARIES}
  ${ADDONS_LIBS}
  ${ESP_LIBS}
  ${GRT_LIBS}
  ${SYS_LIBS}
  )

# =============================================================
#   Package and copy dylib/assets
# =============================================================
set_target_properties(${PROJECT} PROPERTIES
  MACOSX_BUNDLE TRUE
  MACOSX_BUNDLE_INFO_PLIST ${ESP_PATH}/Info.plist
  )

message( STATUS "" ${PROJECT}.app/Contents)
FILE(COPY ${fmodex_LIBRARY} DESTINATION ${PROJECT}.app/Contents/MacOS)

set(OFX_ASSETS ${ADDONS_PATH}/ofxDatGui/ofxbraitsch)
FILE(COPY ${OFX_ASSETS} DESTINATION ${PROJECT}.app/Contents/Resources/data/)
