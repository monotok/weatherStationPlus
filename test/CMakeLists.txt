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

add_executable(test_btncontrol
        unit/src/test_btncontrol.cpp
        ../src/hwcontrol/BtnControl.cpp
        ../src/logging/easylogging++.cpp
        ../src/hal/GPIOControl.cpp
        ../src/tools/Utilities.cpp
        unit/src/myGtestMain.cpp
)

target_link_libraries(test_btncontrol gtest)

add_custom_target(cov
        COMMAND mkdir -p ${COVERAGE_DIR}
        COMMAND test_btncontrol
        COMMAND lcov --capture --directory ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/test_* --output-file ${COVERAGE_DIR}/coverage.info
        COMMAND genhtml ${COVERAGE_DIR}/coverage.info --output-directory ${COVERAGE_DIR}
        )

add_dependencies(cov test_btncontrol)