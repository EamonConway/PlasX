# Define flags for compilation.
COMPILER = /usr/local/Cellar/llvm/17.0.6_1/bin/clang++
# COMPILER = g++
# CXX = $(COMPILER)  -Wall -Wpedantic -Werror  -Wextra -Wconversion -fPIC -g -O3 -std=c++2b
CXX = $(COMPILER)  -Wall -Wpedantic -Werror  -Wextra -fPIC -g -O3 -std=c++2b
# NLOHMANN = ../json/single_include/
GTEST = /usr/local/Cellar/googletest/1.14.0/include
NLOHMANN = extern/json/include
ODEPP = extern/odeplusplus/include
PYBIND = extern/pybind11/include
INCLUDE = cpp/include
OBJ = cpp/build
SRC = cpp/src
TEST = cpp/test
PY_API = python
CPPFLAGS = -I$(INCLUDE) -I$(NLOHMANN) -I$(ODEPP)
CFLAGS =

SOURCES := $(shell find $(SRC) -name "*.cpp")
SOURCES := $(shell find $(SRC) -type f -name '*.cpp' -not -path '$(SRC)/PlasX/Falciparum/*')
OBJECTS := $(patsubst $(SRC)/%.cpp, $(OBJ)/%.o, $(SOURCES))
TEST_SOURCES := $(shell find $(TEST) -name "*.cpp")
TEST_OBJECTS := $(patsubst $(TEST)/%.cpp, $(OBJ)/%.o, $(TEST_SOURCES))
PYTHON_H := $(shell python3-config --includes)
PYTHON_EXTENSION := $(shell python3-config --extension-suffix)

PYBIND_WRAPPERS := $(shell find $(PY_API) -name "*_wrapper.cpp")
PYBIND_SO_FILES := $(patsubst %_wrapper.cpp, %_$(PYTHON_EXTENSION), $(PYBIND_WRAPPERS))
PYBIND_OBJ := $(patsubst $(PY_API)/%.cpp, $(PY_OBJ)/%.o, $(PYBIND_WRAPPERS))
PYBIND_LIBS:= $(patsubst $(PY_API)/%_wrapper.cpp, $(PY_API)/%$(PYTHON_EXTENSION), $(PYBIND_SRC))

python-libs: tests objects $(PYBIND_SO_FILES)

tests: objects test_objects
	@mkdir -p bin
	$(CXX) $(CPPFLAGS) -L/usr/local/lib -o bin/TEST_runner $(TEST_OBJECTS) $(OBJECTS)  -lgtest -pthread

objects: $(OBJECTS)
test_objects: $(TEST_OBJECTS)

clean:
	$(RM) -r -f html
	$(RM) -r -f latex
	$(RM) -r -f cpp/build/
	$(RM) -r -f pybuild/
	$(RM) -r -f pyPlasX/bin/
	$(RM) -r -f bin/
	$(RM) $(PYBIND_SO_FILES)

# Makefile rules for compilation.
$(PY_API)/%_wrapper.o: $(PY_API)/%_wrapper.cpp
	$(CXX) -c $(CPPFLAGS) $(PYTHON_H) -I$(PYBIND) $< -o $@

$(OBJ)/%.o: $(SRC)/%.cpp
	@mkdir -p $(@D)
	$(CXX) -c $(CPPFLAGS) $< -o $@

$(OBJ)/%.o: $(TEST)/%.cpp
	@mkdir -p $(@D)
	$(CXX) -c $(CPPFLAGS) -I$(GTEST) $< -o $@

$(PY_API)/%_$(PYTHON_EXTENSION): $(PY_API)/%_wrapper.o objects
	$(CXX) -shared -undefined dynamic_lookup $(PYTHON_H) -I$(PYBIND) -o $@ $< $(OBJECTS)
