# Build

## CMake

The project has been changed to use cmake and therefore it is more portable than previously.

```
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ../
```

If you want to build for the raspberry pi and then move the executable over to the pi later then pass this option to cmake.

`-DCMAKE_TOOLCHAIN_FILE=rasp/toolchain-rasppi.cmake`

Eg: `cmake -DCMAKE_TOOLCHAIN_FILE=rasp/toolchain-rasppi.cmake`

Now run make to build the binary. There are quite a few build targers the but main program is "weatherStationPlus"

`make weatherStationPlus`

Now inside the binary directory at the top level `../bin` should be the executable.

Run with `./bin/weatherStationPlus`

## Tests

To build the tests. 

```
mkdir build
cd build
cmake ../test

make test_all_unit

./test/unit/bin/test_all_unit
```

### Coverage Report

Run this after executing the test binary. 

`make cov`

HTML report is located within the coverage directory.

# References

## GPIO Manually

```
root@raspdev:/home/pi# echo "4" > /sys/class/gpio/export
root@raspdev:/home/pi# echo "17" > /sys/class/gpio/export
root@raspdev:/home/pi# echo "out" > /sys/class/gpio/gpio4/direction
root@raspdev:/home/pi# echo "in" > /sys/class/gpio/gpio17/direction
root@raspdev:/home/pi# echo "1" > /sys/class/gpio/gpio4/value
root@raspdev:/home/pi# echo "0" > /sys/class/gpio/gpio4/value
root@raspdev:/home/pi# cat /sys/class/gpio/gpio17/value
1
root@raspdev:/home/pi# cat /sys/class/gpio/gpio17/value
0
root@raspdev:/home/pi# echo "4" > /sys/class/gpio/unexport
root@raspdev:/home/pi# echo "17" > /sys/class/gpio/unexport
```
