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

 
SRCEXT := cpp
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
PROTOSOURCES := $(PROTOTYPESRC)/GPIOTestMain.cpp
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
OBJECTSEXCMAIN := $(filter-out build/main.o, $(OBJECTS))
PROTOOBJECTS := $(patsubst $(PROTOTYPESRC)/%,$(PROTOBUILD)/%,$(PROTOSOURCES:.$(SRCEXT)=.o))
CFLAGS := -g # -Wall
# LIB := -pthread -lmongoclient -L lib -lboost_thread-mt -lboost_filesystem-mt -lboost_system-mt
# LIB :=
INC := -I include

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(@D)
	@echo " $(CC) $(CFLAGS) $(INC) -c -o $@ $<"; $(CC) $(CFLAGS) $(INC) -c -o $@ $<

$(PROTOBUILD)/%.o: $(PROTOTYPESRC)/%.$(SRCEXT)
	@mkdir -p $(@D)
	@echo " $(CC) $(CFLAGS) $(INC) -c -o $@ $<"; $(CC) $(CFLAGS) $(INC) -c -o $@ $<

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



.PHONY: clean