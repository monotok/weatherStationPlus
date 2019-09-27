project(Google_Test_Suit)

#Set the flags for testing
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -DELPP_NO_DEFAULT_LOG_FILE -fprofile-arcs -ftest-coverage")

#Stop gcno being prepended with file type as GOV can't find them
set(CMAKE_CXX_OUTPUT_EXTENSION_REPLACE 1)

#Output folders
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/test/unit/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(COVERAGE_DIR coverage)

enable_testing()

set(GTEST $ENV{HOME}/SoftwareDevelopment/Libaries/googletest)
add_subdirectory(${GTEST} build)

get_filename_component(MAIN_PROJECT_DIR ../ ABSOLUTE)

file(GLOB_RECURSE ALL_SOURCE_FILES
        ${MAIN_PROJECT_DIR}/src/data/*.cpp
        ${MAIN_PROJECT_DIR}/src/hal/*.cpp
        ${MAIN_PROJECT_DIR}/src/hwcontrol/*.cpp
        ${MAIN_PROJECT_DIR}/src/logging/*.cpp
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
        unit/src/myGtestMain.cpp
        )

add_executable(test_lcdcontroller
        unit/src/test_lcdcontroller.cpp
        ${MAIN_PROJECT_DIR}/src/data/WeatherSensor.cpp
        ${MAIN_PROJECT_DIR}/src/controller/lcdController.cpp
        ${MAIN_PROJECT_DIR}/src/hal/i2cControl.cpp
        ${MAIN_PROJECT_DIR}/src/hal/lcdDriver.cpp
        ${MAIN_PROJECT_DIR}/src/logging/easylogging++.cpp
        unit/src/myGtestMain.cpp
        )

target_link_libraries(test_btncontrol gtest)
target_link_libraries(test_dynamsensorfact gtest)
target_link_libraries(test_all_unit gtest)
target_link_libraries(test_lcdcontroller gtest)

add_custom_target(cov
        COMMAND mkdir -p ${COVERAGE_DIR}
        COMMAND test_btncontrol
        COMMAND lcov --capture --directory ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/test_* --output-file ${COVERAGE_DIR}/coverage.info
        COMMAND genhtml ${COVERAGE_DIR}/coverage.info --output-directory ${COVERAGE_DIR}
        )

add_dependencies(cov test_btncontrol)