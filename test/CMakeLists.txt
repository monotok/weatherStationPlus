project(Google_Test_Suit)

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

add_test(
        NAME run_btncontrol_tests
        COMMAND test_btncontrol
)