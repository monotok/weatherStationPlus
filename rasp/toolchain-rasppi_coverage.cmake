SET(CMAKE_SYSTEM_NAME Linux)
SET(CMAKE_SYSTEM_VERSION 1)

# Specify the cross compiler
SET(CMAKE_C_COMPILER $ENV{HOME}/SoftwareDevelopment/Libaries/raspberrypi-toolchains/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin/arm-linux-gnueabihf-gcc)
SET(CMAKE_CXX_COMPILER $ENV{HOME}/SoftwareDevelopment/Libaries/raspberrypi-toolchains/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin/arm-linux-gnueabihf-g++)

# Where is the target environment
SET(CMAKE_FIND_ROOT_PATH $ENV{HOME}/SoftwareDevelopment/Libaries/raspberrypi-toolchains/arm-bcm2708/arm-linux-gnueabihf)
#SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --sysroot=${CMAKE_FIND_ROOT_PATH}")
#SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} --sysroot=${CMAKE_FIND_ROOT_PATH}")
#SET(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} --sysroot=${CMAKE_FIND_ROOT_PATH}")

SET(CMAKE_CXX_FLAGS "-DELPP_NO_DEFAULT_LOG_FILE -g -O0 -Wall -fprofile-arcs -ftest-coverage" CACHE STRING "" FORCE)
SET(CMAKE_C_FLAGS "-DELPP_NO_DEFAULT_LOG_FILE -g -O0 -Wall -W -fprofile-arcs -ftest-coverage" CACHE STRING "" FORCE)
SET(CMAKE_EXE_LINKER_FLAGS "-fprofile-arcs -ftest-coverage" CACHE STRING "" FORCE)

# Search for programs only in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# Search for libraries and headers only in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
