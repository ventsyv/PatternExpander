EXE_FILE:=patexp
FLAGS:= -c -I./src -I/usr/include
OBJECTS:= src/Expander.o

GTEST_DIR:= googletest/googletest/include
#TEST_FLAGS:= -isystem $(GTEST_DIR)/include
TEST_FLAGS:= -I$(GTEST_DIR) -fPIC -fprofile-arcs -ftest-coverage

CXXFLAGS += -g -Wall -Wextra -pthread

GTEST_HEADERS = $(GTEST_DIR)/include/gtest/*.h \
                $(GTEST_DIR)/include/gtest/internal/*.h

TEST_OBJS:= test/TestExpander.o

.PHONY: clean

src/%.o: src/%.cpp
	g++ $(FLAGS) $(TEST_FLAGS) $(CXXFLAGS) $< -o $@

test/%.o: test/%.cpp
	g++ $(FLAGS) $(TEST_FLAGS) $(CXXFLAGS) $< -o $@


.PHONY: .default clean build test all coverage report install
.default: all

all: build test report

build: $(OBJECTS) src/main.o
	g++ $(OBJECTS) src/main.o -o bin/$(EXE_FILE)
	
clean:
	@find . -name "*.o" -delete
	@rm bin/* coverage/* report/* 2> /dev/null || true
	@find . -name "*.gcno" -delete
	@find . -name "*.gcda" -delete
	@find . -name "*.gcov" -delete


test: $(OBJECTS) $(TEST_OBJS)
	g++ $(TEST_FLAGS) $(OBJECTS) $(TEST_OBJS) -lgtest -Lgoogletest/build/lib -o bin/unittests
	bin/unittests
	
coverage: ## Run code coverage
	@gcov src/Expander.cpp src/main.cpp test/TestExpander.cpp
	
report: coverage ## Generate gcovr report
	@mkdir gcovr-report 2> /dev/null || true
	@gcovr --root . --html --html-details --output gcovr-report/coverage.html
	
deps: ## Install dependencies
	sudo apt-get install libgtest-dev gcovr

install: build
	sudo cp ./bin/$(EXE_FILE) /usr/local/bin/

