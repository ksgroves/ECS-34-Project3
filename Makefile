
# Tools
AR=ar
CC=gcc
CXX=g++

# Directories
INC_DIR         = ./include
SRC_DIR         = ./src
BIN_DIR         = ./bin
OBJ_DIR         = ./obj
LIB_DIR         = ./lib
TESTSRC_DIR     = ./testsrc
TESTOBJ_DIR     = ./testobj
TESTBIN_DIR     = ./testbin
TESTCOVER_DIR   = ./htmlcov


# Flags

DEFINES         =
INCLUDE         = -I $(INC_DIR)

CPPFLAGS        = -std=c++20
CFLAGS          =
LDFLAGS         =

# Coverage-friendly test flags
TEST_CFLAGS     = $(CFLAGS) -O0 -g --coverage
TEST_CPPFLAGS   = $(CPPFLAGS) -fno-inline
TEST_LDFLAGS = /usr/lib/libgtest_main.a /usr/lib/libgtest.a -lpthread

# Expat for XML reader/writer
XML_LDFLAGS     = -lexpat


# Object lists


# Core/source objects used by tests
STRUTILS_OBJ        = $(OBJ_DIR)/StringUtils.o
STRSRC_OBJ          = $(OBJ_DIR)/StringDataSource.o
STRSINK_OBJ         = $(OBJ_DIR)/StringDataSink.o
DSVREADER_OBJ       = $(OBJ_DIR)/DSVReader.o
DSVWRITER_OBJ       = $(OBJ_DIR)/DSVWriter.o
XMLREADER_OBJ       = $(OBJ_DIR)/XMLReader.o
XMLWRITER_OBJ       = $(OBJ_DIR)/XMLWriter.o

# Test objects
STRUTILS_TEST_OBJ   = $(TESTOBJ_DIR)/StringUtilsTest.o
STRSRC_TEST_OBJ     = $(TESTOBJ_DIR)/StringDataSourceTest.o
STRSINK_TEST_OBJ    = $(TESTOBJ_DIR)/StringDataSinkTest.o
DSV_TEST_OBJ        = $(TESTOBJ_DIR)/DSVTest.o
XML_TEST_OBJ        = $(TESTOBJ_DIR)/XMLTest.o


# Test executables

TEST_STR_TARGET      = $(TESTBIN_DIR)/teststrutils
TEST_STRSRC_TARGET   = $(TESTBIN_DIR)/teststrdatasource
TEST_STRSINK_TARGET  = $(TESTBIN_DIR)/teststrdatasink
TEST_DSV_TARGET      = $(TESTBIN_DIR)/testdsv
TEST_XML_TARGET      = $(TESTBIN_DIR)/testxml

# Default: build dirs, run tests in order, generate coverage
all: directories runtests gencoverage

# Run tests in required order
runtests: $(TEST_STR_TARGET) $(TEST_STRSRC_TARGET) $(TEST_STRSINK_TARGET) $(TEST_DSV_TARGET) $(TEST_XML_TARGET)
	@echo "Running tests in required order..."
	$(TEST_STR_TARGET)
	$(TEST_STRSRC_TARGET)
	$(TEST_STRSINK_TARGET)
	$(TEST_DSV_TARGET)
	$(TEST_XML_TARGET)

# Coverage report
gencoverage:
	@echo "Generating coverage report..."
	lcov --capture --directory . --output-file $(TESTCOVER_DIR)/coverage.info --ignore-errors inconsistent,source
	lcov --remove $(TESTCOVER_DIR)/coverage.info '/usr/*' '*/testsrc/*' --output-file $(TESTCOVER_DIR)/coverage.info
	genhtml $(TESTCOVER_DIR)/coverage.info --output-directory $(TESTCOVER_DIR)

# Link test executables

# 1) StringUtils tests
$(TEST_STR_TARGET): $(STRUTILS_TEST_OBJ) $(STRUTILS_OBJ)
	$(CXX) $(TEST_CFLAGS) $(TEST_CPPFLAGS) $^ $(TEST_LDFLAGS) -o $@

# 2) StringDataSource tests
$(TEST_STRSRC_TARGET): $(STRSRC_TEST_OBJ) $(STRSRC_OBJ)
	$(CXX) $(TEST_CFLAGS) $(TEST_CPPFLAGS) $^ $(TEST_LDFLAGS) -o $@

# 3) StringDataSink tests
$(TEST_STRSINK_TARGET): $(STRSINK_TEST_OBJ) $(STRSINK_OBJ)
	$(CXX) $(TEST_CFLAGS) $(TEST_CPPFLAGS) $^ $(TEST_LDFLAGS) -o $@

# 4) DSV tests (Reader + Writer + StringSource/Sink)
$(TEST_DSV_TARGET): $(DSV_TEST_OBJ) $(DSVREADER_OBJ) $(DSVWRITER_OBJ) $(STRSRC_OBJ) $(STRSINK_OBJ) $(STRUTILS_OBJ)
	$(CXX) $(TEST_CFLAGS) $(TEST_CPPFLAGS) $^ $(TEST_LDFLAGS) -o $@

# 5) XML tests (Reader + Writer + StringSource/Sink) + Expat
$(TEST_XML_TARGET): $(XML_TEST_OBJ) $(XMLREADER_OBJ) $(XMLWRITER_OBJ) $(STRSRC_OBJ) $(STRSINK_OBJ) $(STRUTILS_OBJ)
	$(CXX) $(TEST_CFLAGS) $(TEST_CPPFLAGS) $^ $(TEST_LDFLAGS) $(XML_LDFLAGS) -o $@


# Compile source .cpp -> obj/*.o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(TEST_CFLAGS) $(TEST_CPPFLAGS) $(DEFINES) $(INCLUDE) -c $< -o $@

# Compile test .cpp -> testobj/*.o
$(TESTOBJ_DIR)/%.o: $(TESTSRC_DIR)/%.cpp
	$(CXX) $(TEST_CFLAGS) $(TEST_CPPFLAGS) $(DEFINES) $(INCLUDE) -c $< -o $@

# Directories
.PHONY: directories
directories:
	mkdir -p $(BIN_DIR)
	mkdir -p $(OBJ_DIR)
	mkdir -p $(LIB_DIR)
	mkdir -p $(TESTBIN_DIR)
	mkdir -p $(TESTOBJ_DIR)
	mkdir -p $(TESTCOVER_DIR)

# Clean
clean::
	rm -rf $(BIN_DIR) $(OBJ_DIR) $(LIB_DIR) $(TESTBIN_DIR) $(TESTOBJ_DIR) $(TESTCOVER_DIR)
	rm -f *.gcda *.gcno

.PHONY: clean