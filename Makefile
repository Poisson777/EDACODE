CC = g++
FLAGS = -Wall -g -std=c++20

TARGET = a.out
TEST_TARGET = test.out


SAME_SOURCES = layout/layout.cpp\
	layout/get_networks.cpp\

# List of source files
SOURCES = $(SAME_SOURCES)\
	main.cpp\

# test source
TEST_SOURCES = $(SAME_SOURCES)\
	test.cpp\

OBJECTS = $(SOURCES:.cpp=.o)
TEST_OBJECTS = $(TEST_SOURCES:.cpp=.o)

# Default target
all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(FLAGS) -o $(TARGET) $(OBJECTS)

# Test target
tests: $(TEST_TARGET)

$(TEST_TARGET): $(TEST_OBJECTS)
	$(CC) $(FLAGS) -o $(TEST_TARGET) $(filter-out main.o, $(TEST_OBJECTS))

%.o: %.cpp
	$(CC) $(FLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET) $(TEST_TARGET) *.o
