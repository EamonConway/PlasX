# Define flags for compilation. 
CXX = g++ -Wall -Wpedantic -g 
# -fprofile-arcs -ftest-coverage 
INCLUDE = include
CPPFLAGS = -std=c++2a -DDUMP_INPUT -I$(INCLUDE)
CFLAGS = 
OBJ = build
SRC = src
TEST = test

# SOURCES := $(wildcard $(SRC)/**/*.cpp) 
SOURCES := $(shell ls ${SRC}/**/*.cpp)
SOURCES := $(shell find $(SRC) -name "*.cpp")
OBJECTS := $(patsubst $(SRC)/%.cpp, $(OBJ)/%.o, $(SOURCES))
TEST_SOURCES := $(wildcard $(TEST)/*.cpp) 
TEST_OBJECTS := $(patsubst $(TEST)/%.cpp, $(OBJ)/%.o, $(TEST_SOURCES))

tests: $(OBJECTS) $(TEST_OBJECTS)
	$(CXX) $(CPPFLAGS) -o build/TEST_runner $(TEST_OBJECTS) $(OBJECTS) -lgtest -pthread

clean: 
	$(RM) -r -f html
	$(RM) -r -f latex
	$(RM) -r -f build/

# Makefile rules for compilation. 
$(OBJ)/%.o: $(SRC)/%.cpp
	@mkdir -p $(@D)
	$(CXX) -c $(CPPFLAGS) $< -o $@

$(OBJ)/%.o: $(TEST)/%.cpp
	@mkdir -p $(@D)
	$(CXX) -c $(CPPFLAGS) $< -o $@
