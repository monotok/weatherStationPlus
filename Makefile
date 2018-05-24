#
# TODO: None
#

CC := g++ # This is the main compiler
# CC := clang --analyze # and comment out the linker last line for sanity
SRCDIR := src
PROTOTYPESRC := prototype
BUILDDIR := build
PROTOBUILD := build/prototype
WEATHERTARGET := bin/weatherStationPlus
GPIOTESTTARGET := bin/GPIOTestMain
DYNAMTESTTARGET := bin/DynamTest

DYNAM_PROTO := DynamicSensors.cpp
GPIO_PROTO := GPIOTestMain.cpp
 
SRCEXT := cpp
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
PROTOSOURCES := $(PROTOTYPESRC)/$(DYNAM_PROTO)
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
OBJECTSEXCMAIN := $(filter-out build/main.o, $(OBJECTS))
PROTOOBJECTS := $(patsubst $(PROTOTYPESRC)/%,$(PROTOBUILD)/%,$(PROTOSOURCES:.$(SRCEXT)=.o))
CFLAGS := -g -DELPP_NO_DEFAULT_LOG_FILE -DELPP_LOGGING_FLAGS_FROM_ARG # -Wall
# LIB := -pthread -lmongoclient -L lib -lboost_thread-mt -lboost_filesystem-mt -lboost_system-mt
# LIB :=
INC := -I include

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
	git clean -f -d -x -e .vscode 

# Prototype
gpiotestmain: $(OBJECTSEXCMAIN) $(PROTOOBJECTS)
	@echo " Linking..."
	@mkdir -p bin	
	@echo " $(CC) $^ -o $(GPIOTESTTARGET)"; $(CC) $^ -o $(GPIOTESTTARGET)	

dynamtest: $(OBJECTSEXCMAIN) $(PROTOOBJECTS)
	@echo " Linking..."
	@mkdir -p bin	
	@echo " $(CC) $^ -o $(DYNAMTESTTARGET)"; $(CC) $^ -o $(DYNAMTESTTARGET)	


.PHONY: clean