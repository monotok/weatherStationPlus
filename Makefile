#
# TODO: None
#

CC := g++ # This is the main compiler
# CC := clang --analyze # and comment out the linker last line for sanity
SRCDIR := src
BUILDDIR := build
TARGET := bin/weatherStationPlus
 
SRCEXT := cpp
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
CFLAGS := -g # -Wall
# LIB := -pthread -lmongoclient -L lib -lboost_thread-mt -lboost_filesystem-mt -lboost_system-mt
# LIB :=
INC := -I include

$(TARGET): $(OBJECTS)
	@echo " Linking..."
	@echo " $(CC) $^ -o $(TARGET)"; $(CC) $^ -o $(TARGET)
#	@echo " $(CC) $^ -o $(TARGET) $(LIB)"; $(CC) $^ -o $(TARGET) $(LIB)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(@D)
	@echo " $(CC) $(CFLAGS) $(INC) -c -o $@ $<"; $(CC) $(CFLAGS) $(INC) -c -o $@ $<

clean:
	@echo " Cleaning..."; 
	@echo " $(RM) -r $(BUILDDIR) $(TARGET)"; $(RM) -r $(BUILDDIR) $(TARGET)

# Tests
# tester:
#   $(CC) $(CFLAGS) test/tester.cpp $(INC) $(LIB) -o bin/tester

# Prototype
gpiotestmain: gpiocontrol.o gpiotestmain.o
	$(CC) $(CFLAGS) build/logging/easylogging++.o build/hal/GPIOControl.o build/GPIOTestMain.o -o bin/GPIOControl

gpiotestmain.o:	prototype/GPIOTestMain.cpp include/GPIOControl.hpp
	$(CC) $(CFLAGS) -c prototype/GPIOTestMain.cpp -o build/GPIOTestMain.o

gpiocontrol.o: easylogging++.o src/hal/GPIOControl.cpp include/GPIOControl.hpp
	$(CC) $(CFLAGS) -c src/hal/GPIOControl.cpp -o build/hal/GPIOControl.o

easylogging++.o: src/logging/easylogging++.cpp include/easylogging++.hpp
	$(CC) $(CFLAGS) -c src/logging/easylogging++.cpp -o build/logging/easylogging++.o


.PHONY: clean