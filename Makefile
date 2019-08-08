#
# TODO: None
#

CC := g++ # This is the main compiler
# CC := clang --analyze # and comment out the linker last line for sanity
SRCDIR := src
TESTDIR := test
PROTOTYPESRC := prototype
BUILDDIR := build
PROTOBUILD := build/prototype
WEATHERTARGET := bin/weatherStationPlus
GPIOTESTTARGET := bin/GPIOTestMain
DYNAMTESTTARGET := bin/DynamTest
LCDTESTTARGET := bin/LCDTest


DYNAM_PROTO := DynamicSensors.cpp
GPIO_PROTO := GPIOTestMain.cpp
LCD_PROTO := testLCD.cpp

GCDA_FILES := $(shell find $(BUILDDIR) -type f -name *.gcda)
SRCEXT := cpp
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
PROTOSOURCES := $(PROTOTYPESRC)/$(LCD_PROTO)
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
OBJECTSEXCMAIN := $(filter-out build/main.o, $(OBJECTS))
PROTOOBJECTS := $(patsubst $(PROTOTYPESRC)/%,$(PROTOBUILD)/%,$(PROTOSOURCES:.$(SRCEXT)=.o))
ARGS := 
CFLAGS := -Wall -g -DELPP_NO_DEFAULT_LOG_FILE -DELPP_LOGGING_FLAGS_FROM_ARG $(ARGS) # -std=c++11 -fsanitize=address 
LDFLAGS := $(ARGS) #-fsanitize=address

# Put this in front of binary: LD_PRELOAD=/usr/lib/arm-linux-gnueabihf/libasan.so.3
# LIB := -pthread -lmongoclient -L lib -lboost_thread-mt -lboost_filesystem-mt -lboost_system-mt
# LIB :=
INC := -I include

# Builder will run this
all: dynamtest

# Builder will call this to install the application before running.
install:
	echo "Installing is not supported"

# Builder uses this target to run your application.
run:
	cd bin; ./DynamTest

unit:
	cd $(TESTDIR); make -f MakeFile all

all_unit_tests:
	cd $(TESTDIR); make -f MakeFile all_unit_tests

individual_tests:
	cd $(TESTDIR); make -f MakeFile individual_tests

unitclean:
	cd $(TESTDIR); make -f MakeFile clean

cleancoverage: $(GCDA_FILES)
	rm -rf $(GCDA_FILES)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(@D)
	@echo " $(CC) $(INC) -c -o $@ $< $(CFLAGS)"; $(CC) $(INC) -c -o $@ $< $(CFLAGS)

$(PROTOBUILD)/%.o: $(PROTOTYPESRC)/%.$(SRCEXT)
	@mkdir -p $(@D)
	@echo " $(CC) $(INC) -c -o $@ $< $(CFLAGS)"; $(CC) $(INC) -c -o $@ $< $(CFLAGS)

weather: $(OBJECTS)
	@echo " Linking..."
	@mkdir -p bin
	@echo " $(CC) $^ -o $(WEATHERTARGET)"; $(CC) $^ -o $(WEATHERTARGET)
#	@echo " $(CC) $^ -o $(WEATHERTARGET) $(LIB)"; $(CC) $^ -o $(WEATHERTARGET) $(LIB)

clean:
	@echo " Cleaning...Removing all unknown files of git and all ignored files. Excludes .vscode";
	git clean -f -d -x -e .vscode .idea 

# Prototype
gpiotestmain: $(OBJECTSEXCMAIN) $(PROTOOBJECTS)
	@echo " Linking..."
	@mkdir -p bin	
	@echo " $(CC) $^ -o $(GPIOTESTTARGET)"; $(CC) $^ -o $(GPIOTESTTARGET) $(LDFLAGS)

dynamtest: $(OBJECTSEXCMAIN) $(PROTOOBJECTS)
	@echo " Linking..."
	@mkdir -p bin	
	@echo " $(CC) $^ -o $(DYNAMTESTTARGET)"; $(CC) $^ -o $(DYNAMTESTTARGET) $(LDFLAGS)

lcdtest: $(OBJECTSEXCMAIN) $(PROTOOBJECTS)
	@echo " Linking..."
	@mkdir -p bin	
	@echo " $(CC) $^ -o $(LCDTESTTARGET)"; $(CC) $^ -o $(LCDTESTTARGET) $(LDFLAGS)


.PHONY: clean
