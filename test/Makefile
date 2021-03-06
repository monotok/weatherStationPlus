#SETUP
TEST_DIR := .
PROJECT_DIR := ..
#Build dir for all test code
TESTBUILDDIR := $(TEST_DIR)/build
# Where to find user code.
SRCDIR := $(PROJECT_DIR)/src
INC := -I $(PROJECT_DIR)/include
INCLUDE_DIR := $(PROJECT_DIR)/include

GTEST_BUILD_DIR := gtest
GMOCK_BUILD_DIR := gmock
UNIT_DIR := unit
COVERAGE_DIR := coverage
COVERAGEFLAGS := --coverage

#All src objects
SRCEXT := cpp
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(PROJECT_DIR)/build/%,$(SOURCES:.$(SRCEXT)=.o))
OBJECTSEXCMAIN := $(filter-out $(PROJECT_DIR)/build/main.o, $(OBJECTS))

#All test src objects
TEST_GCDA_FILES := $(shell find $(TESTBUILDDIR) -type f -name *.gcda)
TEST_UNIT_SRCEXT := cpp
TEST_UNIT_SOURCES_INC_MAIN := $(shell find $(UNIT_DIR)/src -type f -name *.$(TEST_UNIT_SRCEXT))
TEST_UNIT_SOURCES := $(filter-out $(UNIT_DIR)/src/myGtestMain.cpp, $(TEST_UNIT_SOURCES_INC_MAIN))
TEST_UNIT_OBJECTS := $(patsubst $(UNIT_DIR)/src/%,$(TESTBUILDDIR)/%,$(TEST_UNIT_SOURCES:.$(TEST_UNIT_SRCEXT)=.o))
nameOnly = $(basename $(notdir $(TEST_UNIT_OBJECTS)))

# House-keeping build targets.

all: gtestsuite all_unit_tests individual_tests

gtestsuite: gtestmain mygtestmain gmockmain

gtestmain: $(GTEST_BUILD_DIR)/gtest_main.a

mygtestmain: $(GTEST_BUILD_DIR)/my_gtest_main.a

gmockmain: $(GMOCK_BUILD_DIR)/gmock_main.a

clean:
	rm -fr $(UNIT_DIR)/bin $(GTEST_BUILD_DIR) $(GMOCK_BUILD_DIR) $(TESTBUILDDIR) $(COVERAGE_DIR)

cleancoverage:
	rm -fr $(COVERAGE_DIR)

cleangcda: $(TEST_GCDA_FILES)
	rm -rf $(TEST_GCDA_FILES)

coverage:
	@mkdir -p $(COVERAGE_DIR)
	lcov --capture --directory $(PROJECT_DIR)/build/ --output-file $(COVERAGE_DIR)/coverage.info
	genhtml $(COVERAGE_DIR)/coverage.info --output-directory $(COVERAGE_DIR)

$(TESTBUILDDIR)/%.o: $(UNIT_DIR)/src/%.$(TEST_UNIT_SRCEXT)
	@mkdir -p $(@D)
	@mkdir -p $(UNIT_DIR)/bin
	@echo "Building unit test objects into build folder"
	$(CXX) $(COVERAGEFLAGS) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

all_unit_tests: $(TEST_UNIT_OBJECTS) $(OBJECTSEXCMAIN)
	@echo "Building unit tests $@"
	$(CXX) $(COVERAGEFLAGS) $(CPPFLAGS) $(CXXFLAGS) -lpthread $^ $(GTEST_BUILD_DIR)/my_gtest_main.a $(GMOCK_BUILD_DIR)/gmock_main.a -o $(UNIT_DIR)/bin/$@

individual_tests: $(TEST_UNIT_OBJECTS) $(OBJECTSEXCMAIN)
	@echo "Building unit tests $<"
	for testObject in $(nameOnly); do \
		$(CXX) $(COVERAGEFLAGS) $(CPPFLAGS) $(CXXFLAGS) -lpthread $(TESTBUILDDIR)/$$testObject.o $(OBJECTSEXCMAIN) $(GTEST_BUILD_DIR)/my_gtest_main.a $(GMOCK_BUILD_DIR)/gmock_main.a -o $(UNIT_DIR)/bin/$$testObject; \
	done	

test_btncontrol: $(TESTBUILDDIR)/test_btncontrol.o $(OBJECTSEXCMAIN)
	@echo "Building unit tests $@"
	$(CXX) $(COVERAGEFLAGS) $(CPPFLAGS) $(CXXFLAGS) -lpthread $^ $(GTEST_BUILD_DIR)/gtest_main.a $(GMOCK_BUILD_DIR)/my_gmock_main.a -o $(UNIT_DIR)/bin/$@


# Points to the root of Google Test, relative to where this file is.
# Remember to tweak this if you move this file.
GTEST_DIR = /home/$(USER)/SoftwareDevelopment/Libaries/googletest/googletest
GMOCK_DIR := $(GTEST_DIR)/../googlemock


# Flags passed to the preprocessor.
# Set Google Test's header directory as a system directory, such that
# the compiler doesn't generate warnings in Google Test headers.
CPPFLAGS += -isystem $(GTEST_DIR)/include -isystem $(GMOCK_DIR)/include

# Flags passed to the C++ compiler.
CXXFLAGS += -g -Wall -Wextra -pthread

# All Google Test headers.  Usually you shouldn't change this
# definition.
GTEST_HEADERS = $(GTEST_DIR)/include/gtest/*.h \
                $(GTEST_DIR)/include/gtest/internal/*.h

# All Google Mock headers. Note that all Google Test headers are
# included here too, as they are #included by Google Mock headers.
# Usually you shouldn't change this definition.	
GMOCK_HEADERS = $(GMOCK_DIR)/include/gmock/*.h \
                $(GMOCK_DIR)/include/gmock/internal/*.h \
				$(GTEST_HEADERS)

# Builds gtest.a and gtest_main.a.

# Usually you shouldn't tweak such internal variables, indicated by a
# trailing _.
GTEST_SRCS_ = $(GTEST_DIR)/src/*.cc $(GTEST_DIR)/src/*.h $(GTEST_HEADERS)
GMOCK_SRCS_ = $(GMOCK_DIR)/src/*.cc $(GMOCK_HEADERS)

# For simplicity and to avoid depending on Google Test's
# implementation details, the dependencies specified below are
# conservative and not optimized.  This is fine as Google Test
# compiles fast and for ordinary users its source rarely changes.
gtest-all.o : $(GTEST_SRCS_)
	@mkdir -p $(GTEST_BUILD_DIR)
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c \
            $(GTEST_DIR)/src/gtest-all.cc -o $(GTEST_BUILD_DIR)/$@

gtest_main.o : $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c \
            $(GTEST_DIR)/src/gtest_main.cc -o $(GTEST_BUILD_DIR)/$@

my_gtest_main.o : $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c \
            $(UNIT_DIR)/src/myGtestMain.cpp -o $(GTEST_BUILD_DIR)/$@

$(GTEST_BUILD_DIR)/gtest.a : gtest-all.o
	cd $(GTEST_BUILD_DIR); $(AR) $(ARFLAGS) ../$@ $^

$(GTEST_BUILD_DIR)/gtest_main.a : gtest-all.o gtest_main.o
	cd $(GTEST_BUILD_DIR); $(AR) $(ARFLAGS) ../$@ $^

$(GTEST_BUILD_DIR)/my_gtest_main.a : gtest-all.o my_gtest_main.o
	cd $(GTEST_BUILD_DIR); $(AR) $(ARFLAGS) ../$@ $^


# GMOCK Creation

gmock-all.o : $(GMOCK_SRCS_)
	@mkdir -p $(GMOCK_BUILD_DIR)
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) -I$(GMOCK_DIR) $(CXXFLAGS) \
            -c $(GMOCK_DIR)/src/gmock-all.cc -o $(GMOCK_BUILD_DIR)/$@

gmock_main.o : $(GMOCK_SRCS_)
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) -I$(GMOCK_DIR) $(CXXFLAGS) \
            -c $(GMOCK_DIR)/src/gmock_main.cc -o $(GMOCK_BUILD_DIR)/$@

$(GMOCK_BUILD_DIR)/gmock.a : gmock-all.o gtest-all.o
	cd $(GMOCK_BUILD_DIR); $(AR) $(ARFLAGS) ../$@ gmock-all.o ../$(GTEST_BUILD_DIR)/gtest-all.o

$(GMOCK_BUILD_DIR)/gmock_main.a : gmock-all.o gtest-all.o gmock_main.o
	cd $(GMOCK_BUILD_DIR); $(AR) $(ARFLAGS) ../$@ gmock-all.o ../$(GTEST_BUILD_DIR)/gtest-all.o gmock_main.o