# Status

[![pipeline status](https://sds.monotok.org/WeatherStationProject/weatherStationPlus/badges/master/pipeline.svg)](https://sds.monotok.org/WeatherStationProject/weatherStationPlus/commits/master)

[![coverage report](https://sds.monotok.org/WeatherStationProject/weatherStationPlus/badges/master/coverage.svg)](https://sds.monotok.org/WeatherStationProject/weatherStationPlus/commits/master)

# Build

## Timescaledb (PostgresSQL Extension)

### Required Packages

- libpq-dev
- libpqxx-dev

### Install DB (Ubuntu 18.04 LTS)

This can be a remote database or a local one. A remote database is probably best as it reduces writes to the sd card of the pi. 

- Install postgres from the Ubuntu repo.
	+ `sudo apt install postgresql`
- Add timescaledb repo and install
	+ `sudo add-apt-repository ppa:timescale/timescaledb-ppa`
	+ `sudo apt-get update`
	+ `sudo apt install timescaledb-postgresql-10`
	+ `sudo timescaledb-tune` (Optional, read the official documentation on tuning [Tuning](https://docs.timescale.com/latest/getting-started/configuring))


### Configuring the DB

- Login to the DB user `sudo su - postgres`
- Now enter the command line utility `psql`
- Create the weather station user
	+ `create role weather login password 'password goes here';`
- Create the database
	+ `create database weather with owner = 'weather';`
- Quit the utility `\q`
- Test logging in with the new user `psql -h localhost -d weather -U weather`

### Configure Remote Access

We want to allow the remote user and others access to the database over the network.

Edit `sudo vim /etc/postgresql/10/main/pg_hba.conf`

Now add this towards the top of the file. Replace the network range with your LAN.

`host  all  all 192.168.1.0/24 md5`

Now edit the postgresql.conf file. `sudo vim /etc/postgresql/10/main/postgresql.conf`

Change the listen address `listen_addresses = '*' `

### Restart service

Restart the service `sudo service postgresql restart` to apply the changes.

### Enable timescaledb extension on the new database

This must be done via the postgres account.

`sudo -u postgres psql`

`\c weather`

`create extension if not exists timescaledb cascade;`

This should show.

```
WARNING:  
WELCOME TO
 _____ _                               _     ____________  
|_   _(_)                             | |    |  _  \ ___ \ 
  | |  _ _ __ ___   ___  ___  ___ __ _| | ___| | | | |_/ / 
  | | | |  _ ` _ \ / _ \/ __|/ __/ _` | |/ _ \ | | | ___ \ 
  | | | | | | | | |  __/\__ \ (_| (_| | |  __/ |/ /| |_/ /
  |_| |_|_| |_| |_|\___||___/\___\__,_|_|\___|___/ \____/
               Running version 1.5.1
For more information on TimescaleDB, please visit the following links:

 1. Getting started: https://docs.timescale.com/getting-started
 2. API reference documentation: https://docs.timescale.com/api
 3. How TimescaleDB is designed: https://docs.timescale.com/introduction/architecture

Note: TimescaleDB collects anonymous reports to better understand and assist our users.
For more information and how to disable, please see our docs https://docs.timescaledb.com/using-timescaledb/telemetry.

CREATE EXTENSION

```

### Create the DB Schema

```sql
create schema sensors;
create table sensors.data
(
    sensorid    text                     not null,
    time        timestamp with time zone not null,
    temperature double precision default 0,
    humidity    double precision default 0,
    battery     integer
);

SELECT create_hypertable('sensors.data', 'time');

create index data_sensorid_time_idx
    on sensors.data (sensorid asc, time desc);

create index data_temp_index
    on sensors.data (time desc, temperature asc)
    where (temperature IS NOT NULL);

create index data_hum_index
    on sensors.data (time desc, humidity asc)
    where (humidity IS NOT NULL);

```


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

## Setup (old)

- Download the compiler toolchain [Git Hub](https://github.com/abhiTronix/raspberry-pi-cross-compilers/wiki/Cross-Compiler:-Installation-Instructions)
- Copy the sysroot files from raspberry pi to a folder on pc

```
rsync -avz root@dev_root:/lib sysroot
rsync -avz root@dev_root:/usr/include sysroot/usr
rsync -avz root@dev_root:/usr/lib sysroot/usr
rsync -avz root@dev_root:/opt/vc sysroot/opt 
```
Down this python file to repoint the symlinks in the root directory. Otherwise they will point to the hosts root
`wget https://raw.githubusercontent.com/riscv/riscv-poky/master/scripts/sysroot-relativelinks.py`

`./sysroot-relativelinks.py sysroot`

- Modify the toolchain file to point to correct dirs


## Setup (Docker)

- Create a .env variable with (Replace user with matching host user and uid. Can then optionally bind git directory inside the container):

```
	USERNAME=gitlab-runner
	USERID=999
	GROUPID=998
```

- sudo apt install qemu-user-static
- docker-compose up -d
- cp the git dir into the container or git clone
	+ docker cp . arm_v6_raspberrypi_one:/home/gitlab-runner/code/
- docker exec -i --user gitlab-runner arm_v6_raspberrypi_one bash -c "cd /home/gitlab-runner/code && mkdir -p build && cd build && cmake -DCMAKE_BUILD_TYPE=Debug .."

To build the tests we run:

```
docker exec -i --user gitlab-runner arm_v6_raspberrypi_one bash -c "cd /home/gitlab-runner/code && mkdir -p build && cd build && cmake --build . --target test_all_unit"
docker cp arm_v6_raspberrypi_one:/home/gitlab-runner/code/test/unit/bin/test_all_unit test/unit/bin/test_all_unit
```

or to build the main application we run:

```
docker exec -i --user gitlab-runner arm_v6_raspberrypi_one bash -c "cd /home/gitlab-runner/code && mkdir -p build && cd build && cmake --build . --target weatherStationPlus"
mkdir -p bin/debug
docker cp arm_v6_raspberrypi_one:/home/gitlab-runner/code/bin/debug/weatherStationPlus bin/debug/
```

- docker-compose down
- upload as an artifact
- via next job via pi run and optionally collect cov
- upload test report



## Issues

- GCDA files produced in the wrong directory, based on the previous runner that built the binaries.
	+ `GCOV_PREFIX and GCOV_PREFIX_STRIP` did not work when set on the raspberry pi running the binaries.
	+ **FIX**: ~~Had to set the gitlab runner to build in a specific directory. https://docs.gitlab.com/runner/configuration/advanced-configuration.html#the-runnerscustom_build_dir-section & https://docs.gitlab.com/ee/ci/yaml/README.html#custom-build-directories~~
	+ **FIX**: Need to pass the -E option to sudo so it picks up the environment variables.

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
