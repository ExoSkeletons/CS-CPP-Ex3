CXX = g++
CXXFLAGS = -Wall -Wextra -g
SOURCES = $(wildcard *.cpp)
HEADERS = $(wildcard *.h)
MAIN_FILE = main
TEST_MAIN_FILE = doctest
MAIN_OBJ = $(filter-out $(TEST_MAIN_FILE).o, $(SOURCES:.cpp=.o))
TEST_OBJ = $(filter-out $(MAIN_FILE).o, $(SOURCES:.cpp=.o))
EXEC_MAIN = main.exe
EXEC_TEST = test.exe

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(EXEC_MAIN): $(MAIN_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(EXEC_TEST): $(TEST_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

all: $(EXEC_MAIN) $(EXEC_TEST)

clean:
	rm -f *.o $(EXEC_MAIN) $(EXEC_TEST)

main: $(EXEC_MAIN)
	./$<

test: $(EXEC_TEST)
	./$<

valgrind: $(EXEC_MAIN)
	sudo valgrind --leak-check=full ./$<

.PHONY: all clean valgrind Main test