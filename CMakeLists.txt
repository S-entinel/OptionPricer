cmake_minimum_required(VERSION 3.1.0)

project(option_pricing)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# For macOS with Homebrew Qt installation
list(APPEND CMAKE_PREFIX_PATH "/opt/homebrew/opt/qt@6")

# Find Qt package
find_package(Qt6 COMPONENTS Core Widgets REQUIRED)
# If Qt6 is not found, try Qt5
if (NOT Qt6_FOUND)
    find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
endif()

# Add source files
set(SOURCES
    main.cpp
    option_pricing.cpp
    greek_calculations.cpp
    option_pricing_gui.cpp
)

# Add header files
set(HEADERS
    option_pricing.h
    greek_calculations.h
    math_utils.h
    types.h
    pricing_exceptions.h
    option_pricing_gui.h
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Link Qt and include directories
if (Qt6_FOUND)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        Qt6::Core
        Qt6::Widgets
    )
    target_include_directories(${PROJECT_NAME} PRIVATE
        ${Qt6Core_INCLUDE_DIRS}
        ${Qt6Widgets_INCLUDE_DIRS}
    )
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE
        Qt5::Core
        Qt5::Widgets
    )
    target_include_directories(${PROJECT_NAME} PRIVATE
        ${Qt5Core_INCLUDE_DIRS}
        ${Qt5Widgets_INCLUDE_DIRS}
    )
endif()