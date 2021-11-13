project(Google_Test_Suit)

#Set the flags for testing
#set(CMAKE_CXX_FLAGS_DEBUG "-DELPP_NO_DEFAULT_LOG_FILE -ftest-coverage ${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG -fno-access-control")
SET(CMAKE_CXX_FLAGS "-DELPP_NO_DEFAULT_LOG_FILE -g -O0 -Wall -fprofile-arcs -ftest-coverage")
SET(CMAKE_C_FLAGS "-DELPP_NO_DEFAULT_LOG_FILE -g -O0 -Wall -W -fprofile-arcs -ftest-coverage")
SET(CMAKE_EXE_LINKER_FLAGS "-fprofile-arcs -ftest-coverage")

# Allow the building of pqxx to work
ADD_DEFINITIONS ( -DPQXX_HIDE_EXP_OPTIONAL )

#Stop gcno being prepended with file type as GOV can't find them
set(CMAKE_CXX_OUTPUT_EXTENSION_REPLACE 1)

#Output folders
#set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/test/unit/bin)
#set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

# Download and unpack googletest at configure time
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
    message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
    message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
        ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
        EXCLUDE_FROM_ALL)

# The gtest/gtest_main targets carry header search path
# dependencies automatically when using CMake 2.8.11 or
# later. Otherwise we have to add them here ourselves.
if (CMAKE_VERSION VERSION_LESS 2.8.11)
    include_directories("${gtest_SOURCE_DIR}/include")
endif()

enable_testing()

get_filename_component(MAIN_PROJECT_DIR ../ ABSOLUTE)

file(GLOB_RECURSE ALL_SOURCE_FILES
        ${MAIN_PROJECT_DIR}/src/data/*.cpp
        ${MAIN_PROJECT_DIR}/src/hal/*.cpp
        ${MAIN_PROJECT_DIR}/src/hwcontrol/*.cpp
        ${MAIN_PROJECT_DIR}/src/logging/*.cpp
        ${MAIN_PROJECT_DIR}/src/model/*.cpp
        ${MAIN_PROJECT_DIR}/src/sensor/*.cpp
        ${MAIN_PROJECT_DIR}/src/tools/*.cpp
        ${MAIN_PROJECT_DIR}/src/controller/*.cpp
        )

file(GLOB TEST_SOURCE_FILES
        unit/src/test_*.cpp)

add_executable(test_all_unit
        ${TEST_SOURCE_FILES}
        ${ALL_SOURCE_FILES}
        unit/src/myGtestMain.cpp
        )


add_executable(test_btncontrol
        unit/src/test_btncontrol.cpp
        ${MAIN_PROJECT_DIR}/src/hwcontrol/BtnControl.cpp
        ${MAIN_PROJECT_DIR}/src/logging/easylogging++.cpp
        ${MAIN_PROJECT_DIR}/src/hal/GPIOControl.cpp
        ${MAIN_PROJECT_DIR}/src/tools/Utilities.cpp
        unit/src/myGtestMain.cpp
)

add_executable(test_dynamsensorfact
        unit/src/test_dynamicsensorfactory.cpp
        ${MAIN_PROJECT_DIR}/src/data/WeatherSensor.cpp
        ${MAIN_PROJECT_DIR}/src/data/DynamicSensorFactory.cpp
        ${MAIN_PROJECT_DIR}/src/logging/easylogging++.cpp
        ${MAIN_PROJECT_DIR}/src/tools/configParser.cpp
        unit/src/myGtestMain.cpp
        )

add_executable(test_retrievesendata
        unit/src/test_retrievesendata.cpp
        ${MAIN_PROJECT_DIR}/src/data/DynamicSensorFactory.cpp
        ${MAIN_PROJECT_DIR}/src/controller/lcdController.cpp
        ${MAIN_PROJECT_DIR}/src/hal/lcdDriver.cpp
        ${MAIN_PROJECT_DIR}/src/data/WeatherSensor.cpp
        ${MAIN_PROJECT_DIR}/src/hal/i2cControl.cpp
        ${MAIN_PROJECT_DIR}/src/sensor/retrieveSenData.cpp
        ${MAIN_PROJECT_DIR}/src/logging/easylogging++.cpp
        ${MAIN_PROJECT_DIR}/src/tools/configParser.cpp
        unit/src/myGtestMain.cpp
        )

add_executable(test_weathersensor
        unit/src/test_weathersensor.cpp
        ${MAIN_PROJECT_DIR}/src/data/WeatherSensor.cpp
        ${MAIN_PROJECT_DIR}/src/logging/easylogging++.cpp
        ${MAIN_PROJECT_DIR}/src/tools/Utilities.cpp
        ${MAIN_PROJECT_DIR}/src/tools/configParser.cpp
        unit/src/myGtestMain.cpp
        )

add_executable(test_lcdcontroller
        unit/src/test_lcdcontroller.cpp
        ${MAIN_PROJECT_DIR}/src/data/WeatherSensor.cpp
        ${MAIN_PROJECT_DIR}/src/data/DynamicSensorFactory.cpp
        ${MAIN_PROJECT_DIR}/src/controller/lcdController.cpp
        ${MAIN_PROJECT_DIR}/src/hal/i2cControl.cpp
        ${MAIN_PROJECT_DIR}/src/hal/lcdDriver.cpp
        ${MAIN_PROJECT_DIR}/src/tools/Utilities.cpp
        ${MAIN_PROJECT_DIR}/src/logging/easylogging++.cpp
        ${MAIN_PROJECT_DIR}/src/tools/configParser.cpp
        unit/src/myGtestMain.cpp
        )

add_executable(test_configparser
        unit/src/test_configParser.cpp
        ${MAIN_PROJECT_DIR}/src/tools/configParser.cpp
        ${MAIN_PROJECT_DIR}/src/model/yang.cpp
        ${MAIN_PROJECT_DIR}/src/logging/easylogging++.cpp
        unit/src/myGtestMain.cpp
        )

add_executable(test_yangparser
        unit/src/test_yangparser.cpp
        ${MAIN_PROJECT_DIR}/src/model/yang.cpp
        ${MAIN_PROJECT_DIR}/src/logging/easylogging++.cpp
        unit/src/myGtestMain.cpp
        )

target_link_libraries(test_btncontrol gtest rt)
target_link_libraries(test_configparser gtest ${LIBCONFIGPP} yang)
target_link_libraries(test_yangparser gtest ${LIBCONFIGPP} yang)
target_link_libraries(test_dynamsensorfact gtest pqxx ${LIBCONFIGPP})
target_link_libraries(test_all_unit gtest rt pqxx pq ${LIBCONFIGPP}) #Need rt for Utilites get time
target_link_libraries(test_lcdcontroller gtest rt pqxx ${LIBCONFIGPP})
target_link_libraries(test_weathersensor gtest pqxx ${LIBCONFIGPP})
target_link_libraries(test_retrievesendata gtest pqxx pq ${LIBCONFIGPP})

set_target_properties(test_btncontrol PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${MAIN_PROJECT_DIR}/test/unit/bin)
set_target_properties(test_configparser PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${MAIN_PROJECT_DIR}/test/unit/bin)
set_target_properties(test_yangparser PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${MAIN_PROJECT_DIR}/test/unit/bin)
set_target_properties(test_dynamsensorfact PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${MAIN_PROJECT_DIR}/test/unit/bin)
set_target_properties(test_all_unit PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${MAIN_PROJECT_DIR}/test/unit/bin)
set_target_properties(test_lcdcontroller PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${MAIN_PROJECT_DIR}/test/unit/bin)
set_target_properties(test_retrievesendata PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${MAIN_PROJECT_DIR}/test/unit/bin)
set_target_properties(test_weathersensor PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${MAIN_PROJECT_DIR}/test/unit/bin)

set(COVERAGE_DIR ${MAIN_PROJECT_DIR}/coverage)

add_custom_target(cov_all
        COMMAND mkdir -p ${COVERAGE_DIR}/test_all_unit
        COMMAND test_all_unit
        COMMAND lcov --capture --directory ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/test_all_unit.dir/__/src --output-file ${COVERAGE_DIR}/test_all_unit/coverage.info
        COMMAND genhtml ${COVERAGE_DIR}/test_all_unit/coverage.info --output-directory ${COVERAGE_DIR}/test_all_unit
        )

add_dependencies(cov_all test_all_unit)

add_custom_target(cov_manual_run
        COMMAND mkdir -p ${COVERAGE_DIR}/manual
        COMMAND lcov --capture --directory ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles --output-file ${COVERAGE_DIR}/manual/coverage.info
        COMMAND genhtml ${COVERAGE_DIR}/manual/coverage.info --output-directory ${COVERAGE_DIR}/manual
        )
