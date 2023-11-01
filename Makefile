EXE_FILE:=patexp
FLAGS:= -c -I./src -I/usr/include
OBJECTS:= src/Expander.o

GTEST_DIR:= googletest/googletest/include
#TEST_FLAGS:= -isystem $(GTEST_DIR)/include
TEST_FLAGS:= -I$(GTEST_DIR)

CXXFLAGS += -g -Wall -Wextra -pthread

GTEST_HEADERS = $(GTEST_DIR)/include/gtest/*.h \
                $(GTEST_DIR)/include/gtest/internal/*.h

TEST_OBJS:= test/TestExpander.o

.PHONY: clean

src/%.o: src/%.cpp
	g++ $(FLAGS) $(CXXFLAGS) $< -o $@

test/%.o: test/%.cpp
	g++ $(FLAGS) $(TEST_FLAGS) $(CXXFLAGS) $< -o $@


.PHONY: .default clean build test all
.default: all

all: build test

build: $(OBJECTS) src/main.o
	g++ $(OBJECTS) src/main.o -o bin/$(EXE_FILE)
	
clean:
	@find . -name "*.o" -delete
	@rm bin/* 2> /dev/null || true


test: $(OBJECTS) $(TEST_OBJS)
	g++ $(TEST_FLAGS) $(OBJECTS) $(TEST_OBJS) -lgtest -Lgoogletest/build/lib -o bin/unittests
	bin/unittests

install: build
	sudo cp ./bin/$(EXE_FILE) /usr/local/bin/

