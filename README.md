# Build
Go to the src directory

This will create the object file for the testmain. Only create object file for things that have changed.

`g++ -c GPIOTestMain.cpp`

Now re-link all the objects into an executable.

`g++ *.o -o GPIOControl`

To build the GPIOTestMain:

`g++ -c prototype/GPIOTestMain.cpp -o build/GPIOTestMain`  
`g++ -c src/hal/GPIOControl.cpp -o build/hal/GPIOControl`  
`g++ build/logging/easylogging++.o build/hal/GPIOControl.o build/GPIOTestMain -o bin/GPIOControl`

## Running

You must run this from **BIN** as the log configuration file is referenced recursively.

`cd bin`

`sudo ./GPIOControl`

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

## Google Test 

To compile the object file

```
set GTEST_DIR /home/hammer/SoftwareDevelopment/Libaries/googletest/googletest/
g++ -isystem $GTEST_DIR/include -I$GTEST_DIR -pthread -c $GTEST_DIR/src/gtest-all.cc
ar -rv libgtest.a gtest-all.o
```