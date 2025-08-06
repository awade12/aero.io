cmake_minimum_required(VERSION 3.16)

project(DroneView VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network NetworkAuth WebEngineWidgets Positioning Location)

qt_standard_project_setup()

set(SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/weatherservice.cpp
    src/flightconditions.cpp
    src/locationservice.cpp
    src/settingsdialog.cpp
    src/aboutdialog.cpp
    src/widgets/weatherwidget.cpp
    src/widgets/radarwidget.cpp
    src/widgets/windwidget.cpp
    src/widgets/airportpresetwidget.cpp
)

set(HEADERS
    src/mainwindow.h
    src/weatherservice.h
    src/flightconditions.h
    src/locationservice.h
    src/settingsdialog.h
    src/aboutdialog.h
    src/widgets/weatherwidget.h
    src/widgets/radarwidget.h
    src/widgets/windwidget.h
    src/widgets/airportpresetwidget.h
)

qt_add_executable(DroneView ${SOURCES} ${HEADERS})

# qt_add_resources(DroneView "resources"
#     PREFIX "/"
#     FILES
#         resources/icons/drone.png
#         resources/icons/weather.png
#         resources/icons/radar.png
#         resources/icons/wind.png
#         resources/icons/flight.png
# )

target_link_libraries(DroneView PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Network
    Qt6::NetworkAuth
    Qt6::WebEngineWidgets
    Qt6::Positioning
    Qt6::Location
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(${TARGET})
endif()

if(WIN32)
    set_target_properties(DroneView PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()

# macOS
if(APPLE)
    set_target_properties(DroneView PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in
    )
endif()