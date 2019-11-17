# Build

## MongoDB (Optional)
This is used to store the weather information so that it can be used and graphed over a period of time.

### Build the C Driver

[C Driver Instructions](http://mongoc.org/libmongoc/current/installing.html)

You can follow the above installation instructions or follow the summary below.

```
$ wget https://github.com/mongodb/mongo-c-driver/releases/download/1.15.2/mongo-c-driver-1.15.2.tar.gz
$ tar xzf mongo-c-driver-1.15.2.tar.gz
$ cd mongo-c-driver-1.15.2
$ mkdir cmake-build
$ cd cmake-build
$ cmake -DENABLE_AUTOMATIC_INIT_AND_CLEANUP=OFF ..
```

### Install C Driver

```
$ make
$ sudo make install
```

### Uninstall C Driver

```
sudo /usr/local/share/mongo-c-driver/uninstall.sh

OR from build directory (if left intact)

sudo make uninstall
```


## Build C++ Driver

Firstly the C driver needs to be installed.

[C++ Install Instructions](http://mongocxx.org/mongocxx-v3/installation/)

This project uses C++ 11 so needs to use an extra option.

```
git clone https://github.com/mongodb/mongo-cxx-driver.git \                                                                                    
    --branch releases/stable --depth 1
cd mongo-cxx-driver/build
```

`cmake -DCMAKE_BUILD_TYPE=Release -DBSONCXX_POLY_USE_MNMLSTC=1 -DCMAKE_INSTALL_PREFIX=/usr/local ..`

So you don't have to run the whole make as sudo (Because we are installing to /usr/local)

`sudo make EP_mnmlstc_core`

`make && sudo make install`

### Testing the Driver



## CMake

The project has been changed to use cmake and therefore it is more portable than previously.

```
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
```

If you want to build for the raspberry pi and then move the executable over to the pi later then pass this option to cmake.

`-DCMAKE_TOOLCHAIN_FILE=rasp/toolchain-rasppi.cmake`

Eg: `cmake -DCMAKE_TOOLCHAIN_FILE=rasp/toolchain-rasppi.cmake`

Now run make to build the binary. There are quite a few build targers the but main program is "weatherStationPlus"

`cmake --build . --target weatherStationPlus`

Now inside the build directory the executable should be the available. Sudo is needed for access to the i2c interfaces.

Run with `sudo ./weatherStationPlus`

## Tests

To build the tests. There are several test targets but the `test_all_unit` will test everything.

```
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..

cmake --build . --target test_all_unit

cd test
sudo ./test_all_unit --gtest_output="xml:report.xml"
```

### Coverage Report

Run this after executing the test binary. 

`make cov`

To manually run the coverage.

```
mkdir coverage
lcov --capture --directory CMakeFiles/test_all_unit.dir/ --output-file coverage/coverage.info
genhtml coverage/coverage.info --output-directory coverage
```

HTML report is located within the coverage directory.

# Cross Platform Builds (Gitlab Ci)

Unfortunately I had quite a few problems regarding this; I did eventually get it working but felt it would be more reliable to just have the coverage via the local raspberry pi builds and runs these on merge requests only.

## Issues

- GCDA files produced in the wrong directory, based on the previous runner that built the binaries.
	+ `GCOV_PREFIX and GCOV_PREFIX_STRIP` did not work when set on the raspberry pi running the binaries.
	+ **FIX**: Had to set the gitlab runner to build in a specific directory. https://docs.gitlab.com/runner/configuration/advanced-configuration.html#the-runnerscustom_build_dir-section & https://docs.gitlab.com/ee/ci/yaml/README.html#custom-build-directories

- Cannot open source file `......raspberrypi-toolchains/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/arm-linux-gnueabihf/include/c++/4.8.3`.
	+ **Fix**: Needed to copy the raspberry pi cross compiling tools over to the pi (Just this folder). Potential issue was with generating coverage for standard library files.  
	
- GCDA version mismatch. Expected 603* got 408* etc. 
	+ **No FIX**: Could not get this to work as expected. Used many different compilers for armv6. Most useful looking: https://sourceforge.net/projects/raspberry-pi-cross-compilers/
	+ **Work Around**: Don't use version 6 gcc, just use the standard raspberry pi cross platform tools (version 4.8.3) and force the raspberry pi default version of gcc to be 4.8.x. `sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/arm-linux-gnueabihf-gcc-4.8 60 --slave /usr/bin/g++ g++ /usr/bin/arm-linux-gnueabihf-g++-4.8 --slave /usr/bin/gcov gcov /usr/bin/arm-linux-gnueabihf-gcov-4.8 --slave /usr/bin/gcc-ar gcc-ar /usr/bin/arm-linux-gnueabihf-gcc-ar-4.8 --slave /usr/bin/gcc-nm gcc-nm /usr/bin/arm-linux-gnueabihf-gcc-nm-4.8`. This command will make gcc 4.8.x to be default. Must make sure to include the gcc-nm for gcda to work.
	+ Can also add GCC 6 so we can easily change back and forth. `sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/arm-linux-gnueabihf-gcc-6 80 --slave /usr/bin/g++ g++ /usr/bin/arm-linux-gnueabihf-g++-6 --slave /usr/bin/gcov gcov /usr/bin/arm-linux-gnueabihf-gcov-6 --slave /usr/bin/gcc-ar gcc-ar /usr/bin/arm-linux-gnueabihf-gcc-ar-6 --slave /usr/bin/gcc-nm gcc-nm /usr/bin/arm-linux-gnueabihf-gcc-nm-6`
	+ Switch between them with `sudo update-alternatives --config gcc`

# References

1. https://raspberrypi.stackexchange.com/questions/93597/problem-with-gcc-g-versions
1. https://askubuntu.com/questions/534658/undo-update-alternatives
1. https://stackoverflow.com/questions/3004255/why-is-gcov-creating-code-coverage-data-for-stl-headers
1. https://docs.gitlab.com/ee/ci/yaml/#onlyexcept-basic
1. https://docs.gitlab.com/ee/ci/junit_test_reports.html


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
