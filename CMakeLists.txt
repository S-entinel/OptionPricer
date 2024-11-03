cmake_minimum_required(VERSION 3.1.0)
project(options_pricing VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_PREFIX_PATH "/opt/homebrew/Cellar/qt@5/5.15.15_2")

find_package(Qt5 COMPONENTS Widgets REQUIRED)

# Define source files
set(CORE_SOURCES
    src/core/greek_calculations.cpp
    src/core/option_pricing.cpp
)

set(GUI_SOURCES
    src/gui/options_gui.cpp
)

set(HEADERS
    src/core/greek_calculations.h
    src/core/math_utils.h
    src/core/option_pricing.h
    src/core/pricing_exceptions.h
    src/core/types.h
    src/gui/options_gui.h
)

# Create executable
add_executable(options_pricing
    src/main.cpp
    ${CORE_SOURCES}
    ${GUI_SOURCES}
    ${HEADERS}
)

# Include directories
target_include_directories(options_pricing PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gui
)

# Link Qt libraries
target_link_libraries(options_pricing PRIVATE
    Qt5::Widgets
)

# Install rules
install(TARGETS options_pricing
    RUNTIME DESTINATION bin
)