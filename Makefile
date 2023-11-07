EXE_FILE:=patexp
OBJECTS:= Expander.o main.o

DEBUG_DIR = build/debug
RELEASE_DIR = build/release

DEBUG_OBJS = $(addprefix $(DEBUG_DIR)/, $(OBJECTS))
RELEASE_OBJS = $(addprefix $(RELEASE_DIR)/, $(OBJECTS))

TEST_FLAGS:= $(CXXFLAGS) -fPIC -fprofile-arcs -ftest-coverage -Lgcov

CXXFLAGS +=  -c -I./src -I/usr/include -Wall -Wextra -pthread

TEST_OBJS:= test/TestExpander.o

.PHONY: .default clean build test all coverage report install debug
.default: build
    
$(RELEASE_DIR)/%.o: src/%.cpp
	 @mkdir -p $(@D)
	g++ $(CXXFLAGS) -o $@ $<
	
$(DEBUG_DIR)/%.o: src/%.cpp
	@mkdir -p $(@D)
	g++ $(CXXFLAGS) -o $@ $<

test/%.o: test/%.cpp
	g++ $(FLAGS) $(TEST_FLAGS) $(CXXFLAGS) $< -o $@

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
	

debug: CXXFLAGS += -DDEBUG -g -O0
debug: $(DEBUG_OBJS) src/main.o
	-@mkdir bin
	g++ $(DEBUG_OBJS) -o bin/$(EXE_FILE)d


test: CXXFLAGS += -DDEBUG -g -fprofile-arcs -ftest-coverage
test: $(OBJECTS) $(TEST_OBJS)
	-mkdir bin
	g++ $(TEST_FLAGS) $(OBJECTS) $(TEST_OBJS) -lgtest -Lgoogletest/build/lib -o bin/unittests
	bin/unittests
	
coverage: ## Run code coverage
	@gcov src/Expander.cpp src/main.cpp test/TestExpander.cpp
	
report: coverage ## Generate gcovr report
	@mkdir gcovr-report 2> /dev/null || true
	@gcovr --root . --html --html-details --output gcovr-report/coverage.html
	
deps: ## Install dependencies
	sudo apt-get install build-essential libgtest-dev gcovr

install: build
	sudo cp ./bin/$(EXE_FILE) /usr/local/bin/

