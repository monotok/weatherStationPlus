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
	@echo " $(CC) $^ -o $(WEATHERTARGET)"; $(CC) $^ -o $(WEATHERTARGET)
#	@echo " $(CC) $^ -o $(WEATHERTARGET) $(LIB)"; $(CC) $^ -o $(WEATHERTARGET) $(LIB)

clean:
	@echo " Cleaning..."; 
	@echo " $(RM) -r $(BUILDDIR) $(WEATHERTARGET)"; $(RM) -r $(BUILDDIR) $(WEATHERTARGET)
	@echo " $(RM) -r $(BUILDDIR) $(GPIOTESTTARGET)"; $(RM) -r $(BUILDDIR) $(GPIOTESTTARGET)

# Tests
# tester:
#   $(CC) $(CFLAGS) test/tester.cpp $(INC) $(LIB) -o bin/tester


# Prototype
gpiotestmain: $(OBJECTSEXCMAIN) $(PROTOOBJECTS)
	@echo " Linking..."
	@echo " $(CC) $^ -o $(GPIOTESTTARGET)"; $(CC) $^ -o $(GPIOTESTTARGET)	



.PHONY: clean