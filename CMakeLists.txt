# Specify the minimum version of CMAKE

cmake_minimum_required(VERSION 3.7.2)

# Projects Name
project(weatherStationPlus)

# C++ Standard
set(CMAKE_CXX_STANDARD 11)

# Complier Flags
set(CMAKE_CXX_FLAGS_DEBUG "-Wall -g -DELPP_NO_DEFAULT_LOG_FILE")
set(CMAKE_CXX_FLAGS_RELEASE "-DELPP_NO_DEFAULT_LOG_FILE")

#Output folders
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

#Include the tests
add_subdirectory(test)


include_directories(include)

set(DYNAM_SOURCE_FILES prototype/DynamicSensors.cpp src/logging/easylogging++.cpp src/hal/GPIOControl.cpp src/hwcontrol/BtnControl.cpp src/data/WeatherSensor.cpp src/tools/Utilities.cpp)
set(LCD_SOURCE_FILES prototype/testLCD.cpp src/logging/easylogging++.cpp src/hal/i2cControl.cpp src/hal/lcdDriver.cpp)
set(GPIO_SOURCE_FILES prototype/GPIOTestMain.cpp src/logging/easylogging++.cpp src/hal/GPIOControl.cpp src/hwcontrol/BtnControl.cpp src/tools/Utilities.cpp)
set(MAIN_SOURCE_FILES src/main.cpp src/logging/easylogging++.cpp)

add_executable(DynamSensors ${DYNAM_SOURCE_FILES})
add_executable(TestLCD ${LCD_SOURCE_FILES})
add_executable(GPIOTest ${GPIO_SOURCE_FILES})
add_executable(weatherStationPlus ${MAIN_SOURCE_FILES})

set(lfc "\"~/.config/wsp/logging.conf\"")
if (lfc)
    target_compile_definitions(DynamSensors PUBLIC -DLOGCONFIGLOCATION=${lfc})
endif()

add_custom_target(fullclean
        COMMAND git clean -f -d -x -e .vscode -e .idea
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        )

install(FILES conf/logging.conf DESTINATION ../conf/logging.conf COMPONENT config)