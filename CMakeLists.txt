# Specify the minimum version of CMAKE

cmake_minimum_required(VERSION 3.7.2)

# Projects Name
project(weatherStationPlus)

# C++ Standard
set(CMAKE_CXX_STANDARD 11)

# Complier Flags
set(CMAKE_CXX_FLAGS_DEBUG "-DCMAKE_BUILD_TYPE=Debug")
set(CMAKE_CXX_FLAGS_RELEASE "-DELPP_NO_DEFAULT_LOG_FILE" -O2)

set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-fsanitize=address -fno-optimize-sibling-calls -fno-omit-frame-pointer -g -O1")

# Allow the building of pqxx to work
ADD_DEFINITIONS ( -DPQXX_HIDE_EXP_OPTIONAL )
#Output folders
#set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
#set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
#set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

#Find libraries
find_library(LIBCONFIGPP config++)
find_library(LIBCONFIG config)
find_library(LIBYANG yang)
message(STATUS "FOUND LIBCONFIG ++: " ${LIBCONFIGPP})
message(STATUS "FOUND LIBCONFIG : " ${LIBCONFIG})
message(STATUS "FOUND LIBYANG : " ${LIBYANG})

#Include the tests
add_subdirectory(test)

include_directories(include)
include_directories(SYSTEM /usr/include)
include_directories(SYSTEM /usr/local/include)

set(DYNAM_SOURCE_FILES prototype/DynamicSensors.cpp src/logging/easylogging++.cpp src/hal/GPIOControl.cpp src/hwcontrol/BtnControl.cpp src/data/WeatherSensor.cpp src/tools/Utilities.cpp)
set(LCD_SOURCE_FILES prototype/testLCD.cpp src/logging/easylogging++.cpp src/hal/i2cControl.cpp src/hal/lcdDriver.cpp)
set(GPIO_SOURCE_FILES prototype/GPIOTestMain.cpp src/logging/easylogging++.cpp src/hal/GPIOControl.cpp src/hwcontrol/BtnControl.cpp src/tools/Utilities.cpp)
set(MAIN_SOURCE_FILES src/main.cpp
        src/logging/easylogging++.cpp
        src/sensor/retrieveSenData.cpp
        src/data/DynamicSensorFactory.cpp
        src/hal/i2cControl.cpp
        src/hal/lcdDriver.cpp
        src/controller/lcdController.cpp
        src/hal/GPIOControl.cpp
        src/hwcontrol/BtnControl.cpp
        src/tools/Utilities.cpp
        src/tools/configParser.cpp
        src/data/WeatherSensor.cpp
        src/model/yang.cpp)

add_executable(DynamSensors ${DYNAM_SOURCE_FILES})
add_executable(TestLCD ${LCD_SOURCE_FILES})
add_executable(GPIOTest ${GPIO_SOURCE_FILES})
add_executable(weatherStationPlus ${MAIN_SOURCE_FILES})

target_link_libraries(weatherStationPlus
        pthread
        rt
        gtest
        pqxx
        pq
        yang
        ${LIBCONFIGPP})

target_link_libraries(GPIOTest
        rt)

if (CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)
    target_link_libraries(weatherStationPlus
            pthread
            rt
            gtest
            pqxx
            pq
            yang
            ${LIBCONFIGPP}
            -fno-omit-frame-pointer
            -fsanitize=address)
endif()

set_target_properties(weatherStationPlus PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/bin/debug)

set(lfc "\"~/.config/wsp/logging.conf\"")
if (lfc)
    target_compile_definitions(weatherStationPlus PUBLIC -DLOGCONFIGLOCATION=${lfc})
endif()

add_custom_target(fullclean
        COMMAND git clean -f -d -x -e .vscode -e .idea
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        )

install(FILES conf/logging.conf DESTINATION ../conf/logging.conf COMPONENT config)