EXE_FILE:=patexp
SRC_FILES:= Expander.o
MAIN_FILE:= main.o
TEST_FILES:= TestExpander.o

# Build directories - allows for separate release and debug executables
DEBUG_DIR = build/debug
RELEASE_DIR = build/release
TEST_DIR = build/test

DEBUG_OBJS = $(addprefix $(DEBUG_DIR)/, $(SRC_FILES))
DEBUG_MAIN_OBJ = $(addprefix $(DEBUG_DIR)/, $(MAIN_FILE))
RELEASE_OBJS = $(addprefix $(RELEASE_DIR)/, $(SRC_FILES) $(MAIN_FILE) )
TEST_OBJS = $(addprefix $(TEST_DIR)/, $(TEST_FILES))

CXXFLAGS +=  -c -Wall -Wextra -Isrc/


.PHONY: .default clean build test all coverage report install debug
.default: build
    
$(RELEASE_DIR)/%.o: src/%.cpp
	 @mkdir -p $(@D)
	g++ $(CXXFLAGS) -o $@ $<
	
$(DEBUG_DIR)/%.o: src/%.cpp
	@mkdir -p $(@D)
	g++ $(CXXFLAGS) -o $@ $<

$(TEST_DIR)/%.o: test/%.cpp
	@mkdir -p $(@D)
	g++ $(CXXFLAGS) -o $@ $<

# Builds the release executable
build: CXXFLAGS += -O3
build: $(RELEASE_OBJS)
	-mkdir bin
	g++ $(RELEASE_OBJS) -o bin/$(EXE_FILE)
	

clean:
	@find . -name "*.o" -delete
	@rm bin/* coverage/* report/* 2> /dev/null || true
	@rm -rf bin/ build/
	@find . -name "*.gcno" -delete
	@find . -name "*.gcda" -delete
	@find . -name "*.gcov" -delete
	

# Builds with debug data and coverage enabled
debug: CXXFLAGS += -DDEBUG -g -O0 -fPIC -fprofile-arcs -ftest-coverage
debug: $(DEBUG_OBJS) $(DEBUG_MAIN_OBJ)
	-@mkdir bin
	g++ $(DEBUG_OBJS) $(DEBUG_MAIN_OBJ) -Lgcov -fPIC -fprofile-arcs -ftest-coverage -o bin/$(EXE_FILE)d

# Builds the test using the debug objects - debug data is useful when debugging failing unit tests
test: CXXFLAGS += -DDEBUG -g -fPIC -fprofile-arcs -ftest-coverage 
test: $(DEBUG_OBJS) $(TEST_OBJS)
	-mkdir bin
	g++ $(DEBUG_OBJS) $(TEST_OBJS) -lgtest -Lgoogletest/build/lib -Lgcov -fPIC -fprofile-arcs -ftest-coverage  -o bin/unittests
	bin/unittests
	
coverage: test ## Run code coverage
	@gcov -r -v src/Expander.cpp src/main.cpp test/TestExpander.cpp -o $(DEBUG_DIR) -o $(TEST_DIR)
	
report: coverage ## Generate gcovr report
	@mkdir gcovr-report 2> /dev/null || true
	@gcovr --root . --html --html-details --output gcovr-report/coverage.html
	
deps: ## Install dependencies
	sudo apt-get install build-essential libgtest-dev gcovr

install: build
	sudo cp ./bin/$(EXE_FILE) /usr/local/bin/

