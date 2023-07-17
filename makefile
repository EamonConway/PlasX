# Define flags for compilation. 
CXX = g++ -Wall -Wpedantic -Werror -O3
# -fprofile-arcs -ftest-coverage 
INCLUDE = include
NLOHMANN = ../
CPPFLAGS = -std=c++2a -DDUMP_INPUT -I$(INCLUDE) -I$(NLOHMANN)
CFLAGS = 
OBJ = build
SRC = src
TEST = test

# SOURCES := $(shell ls ${SRC}/**/*.cpp)
SOURCES := $(shell find $(SRC) -name "*.cpp")
OBJECTS := $(patsubst $(SRC)/%.cpp, $(OBJ)/%.o, $(SOURCES))
TEST_SOURCES := $(shell find $(TEST) -name "*.cpp")
TEST_OBJECTS := $(patsubst $(TEST)/%.cpp, $(OBJ)/%.o, $(TEST_SOURCES))

pvibm-equilibrium: tests objects
	@mkdir -p bin
	$(CXX) $(CPPFLAGS) -o bin/pvibm-equilibrium pvibm-equilibrium.cpp $(OBJECTS)

# main: tests objects
# 	@mkdir -p bin
# 	$(CXX) $(CPPFLAGS) -o bin/plasx main.cpp $(OBJECTS)

tests: objects test_objects
	@mkdir -p bin
	$(CXX) $(CPPFLAGS) -o bin/TEST_runner $(TEST_OBJECTS) $(OBJECTS) -lgtest -pthread

objects: $(OBJECTS)
test_objects: $(TEST_OBJECTS)
clean: 
	$(RM) -r -f html
	$(RM) -r -f latex
	$(RM) -r -f build/
	$(RM) -r -f bin/

# Makefile rules for compilation. 
$(OBJ)/%.o: $(SRC)/%.cpp
	@mkdir -p $(@D)
	$(CXX) -c $(CPPFLAGS) $< -o $@

$(OBJ)/%.o: $(TEST)/%.cpp
	@mkdir -p $(@D)
	$(CXX) -c $(CPPFLAGS) $< -o $@
