EXE_FILE:=patexp
OBJECTS:= src/Expander.o

TEST_FLAGS:= $(CXXFLAGS) -fPIC -fprofile-arcs -ftest-coverage -Lgcov

CXXFLAGS +=  -c -I./src -I/usr/include -Wall -Wextra -pthread

TEST_OBJS:= test/TestExpander.o

.PHONY: clean

src/%.o: src/%.cpp
	g++ $(CXXFLAGS) $< -o $@

test/%.o: test/%.cpp
	g++ $(FLAGS) $(TEST_FLAGS) $(CXXFLAGS) $< -o $@


.PHONY: .default clean build test all coverage report install
.default: release

release: $(OBJECTS) src/main.o
	-mkdir bin
	g++ $(OBJECTS) src/main.o -o bin/$(EXE_FILE)
	

clean:
	@find . -name "*.o" -delete
	@rm bin/* coverage/* report/* 2> /dev/null || true
	@rm -rf bin/
	@find . -name "*.gcno" -delete
	@find . -name "*.gcda" -delete
	@find . -name "*.gcov" -delete


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

