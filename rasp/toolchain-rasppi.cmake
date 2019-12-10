# THIS DIDNT WORK ONCE I NEEDED PQXX. NOT SURE WHY.
# WOULD NOT LINK CORRECTLY AT END OF BUILD.
# LITTLE DOCUMENTATION AROUND CROSS COMPILING THROUGH CMAKE.
# SWITCHED TO CROSS COMPILE WITH A DOCKER IMAGE OF RASPBIAN 10. SAME AS REAL PI.
# USES qemu-user-static TO RUN ON A X86 MACHINE HOST
# LEFT FOR REFERENCE ONLY


SET(CMAKE_SYSTEM_NAME Linux)
SET(CMAKE_SYSTEM_VERSION 1)

set(CMAKE_CXX_COMPILER_WORKS TRUE)

# Specify the cross compiler
SET(CMAKE_C_COMPILER $ENV{HOME}/SoftwareDevelopment/Libaries/raspberrypi-toolchains/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin/arm-linux-gnueabihf-gcc)
SET(CMAKE_CXX_COMPILER $ENV{HOME}/SoftwareDevelopment/Libaries/raspberrypi-toolchains/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin/arm-linux-gnueabihf-g++)

# Where is the target environment
#SET(CMAKE_FIND_ROOT_PATH $ENV{HOME}/SoftwareDevelopment/Libaries/raspberrypi-toolchains/arm-bcm2708/arm-rpi-4.9.3-linux-gnueabihf/arm-linux-gnueabihf/sysroot/)
SET(CMAKE_FIND_ROOT_PATH $ENV{HOME}/SoftwareDevelopment/Libaries/raspberry_sysroot/realraspbian10/sysroot)
SET(INCLUDE_LINKS "-I${CMAKE_FIND_ROOT_PATH}/usr/include -I${CMAKE_FIND_ROOT_PATH}/usr/include/arm-linux-gnueabihf -Wl,-rpath-link,${CMAKE_FIND_ROOT_PATH}/usr/lib/arm-linux-gnueabihf,-rpath-link,${CMAKE_FIND_ROOT_PATH}/lib/arm-linux-gnueabihf")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wextra -DELPP_NO_DEFAULT_LOG_FILE -D_DEBUG ${INCLUDE_LINKS}")

#set(ENV{LDFLAGS} "-L${CMAKE_FIND_ROOT_PATH}/lib -L${CMAKE_FIND_ROOT_PATH}/usr/ -L${CMAKE_FIND_ROOT_PATH}/lib/arm-linux-gnueabihf -L${CMAKE_FIND_ROOT_PATH}/usr/lib -L${CMAKE_FIND_ROOT_PATH}/usr/lib/arm-linux-gnueabihf -Wl,-rpath-link,${CMAKE_FIND_ROOT_PATH}/usr/lib/arm-linux-gnueabihf,-rpath-link,${CMAKE_FIND_ROOT_PATH}/lib/arm-linux-gnueabihf")

SET(LINKER_FLAGS "-L${CMAKE_FIND_ROOT_PATH}/lib/arm-linux-gnueabihf -L${CMAKE_FIND_ROOT_PATH}/usr/lib/arm-linux-gnueabihf")
#SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --sysroot=${CMAKE_FIND_ROOT_PATH}")
SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} --sysroot=${CMAKE_FIND_ROOT_PATH} ${LINKER_FLAGS}")
#SET(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} --sysroot=${CMAKE_FIND_ROOT_PATH}")

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --sysroot=${CMAKE_FIND_ROOT_PATH}" CACHE INTERNAL "" FORCE)
#set(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} --sysroot=${CMAKE_FIND_ROOT_PATH}" CACHE INTERNAL "" FORCE)


# Search for programs only in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# Search for libraries and headers only in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)