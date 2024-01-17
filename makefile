# Define flags for compilation.
COMPILER = /usr/local/Cellar/llvm/17.0.6/bin/clang++
# COMPILER = g++
# CXX = $(COMPILER)  -Wall -Wpedantic -Werror  -Wextra -Wconversion -fPIC -g -O3 -std=c++2b
CXX = $(COMPILER)  -Wall -Wpedantic -Werror  -Wextra -fPIC -g -O3 -std=c++2b
INCLUDE = include
# NLOHMANN = ../json/single_include/
GTEST = extern/googletest/googletest/include
NLOHMANN = extern/json/include
ODEPP = extern/odeplusplus/include
PYBIND = extern/pybind11/include
CPPFLAGS = -I$(INCLUDE) -I$(NLOHMANN) -I$(ODEPP)
CFLAGS =
PY_OBJ = pybuild
OBJ = build
SRC = src
TEST = test
PY_API = python/src

SOURCES := $(shell find $(SRC) -name "*.cpp")
SOURCES := $(shell find $(SRC) -type f -name '*.cpp' -not -path '$(SRC)/PlasX/Falciparum/*')
OBJECTS := $(patsubst $(SRC)/%.cpp, $(OBJ)/%.o, $(SOURCES))
TEST_SOURCES := $(shell find $(TEST) -name "*.cpp")
TEST_OBJECTS := $(patsubst $(TEST)/%.cpp, $(OBJ)/%.o, $(TEST_SOURCES))
PYBIND_SRC := $(shell find $(PY_API) -name "*py*.cpp")
PYBIND_OBJ := $(patsubst $(PY_API)/%.cpp, $(PY_OBJ)/%.o, $(PYBIND_SRC))
PYTHON_H := $(shell python3-config --includes)
PYTHON_EXTENSION := $(shell python3-config --extension-suffix)

python-library:  tests objects pybind_objects pvibm-equilibrium
	@mkdir -p pybin
	@echo $(PYBIND_OBJ)
	# g++ -O3 -Wall -shared -std=c++2a $(python3-config --includes) -I$(PYBIND) -o pybin/pyPlasX$(PYTHON_EXTENSION) $(PYBIND_OBJ) $(OBJECTS)
	$(CXX) -shared -undefined dynamic_lookup $(python3-config --includes) -I$(PYBIND) -o pybin/pyPlasX$(PYTHON_EXTENSION) $(PYBIND_OBJ) $(OBJECTS)

pvibm-mosquito: pvibm-equilibrium tests objects
	@mkdir -p bin
	#$(CXX) $(CPPFLAGS) -o bin/plasx-pvibm-mosquito main_mosquito.cpp $(OBJECTS)

pvibm-equilibrium:  tests objects
	@mkdir -p bin
	$(CXX) $(CPPFLAGS) -o bin/plasx-pvibm-equilibrium plasx-pvibm-equilibrium.cpp $(OBJECTS)

tests: objects test_objects
	@mkdir -p bin
	$(CXX) $(CPPFLAGS) -o bin/TEST_runner $(TEST_OBJECTS) $(OBJECTS)  -lgtest -pthread

objects: $(OBJECTS)
test_objects: $(TEST_OBJECTS)
pybind_objects: $(PYBIND_OBJ)

clean:
	$(RM) -r -f html
	$(RM) -r -f latex
	$(RM) -r -f build/
	$(RM) -r -f pybuild/
	$(RM) -r -f pybin/
	$(RM) -r -f bin/

# Makefile rules for compilation.
$(OBJ)/%.o: $(SRC)/%.cpp
	@mkdir -p $(@D)
	$(CXX) -c $(CPPFLAGS) $< -o $@

$(OBJ)/%.o: $(TEST)/%.cpp
	@mkdir -p $(@D)
	$(CXX) -c $(CPPFLAGS) -I$(GTEST) $< -o $@

$(PY_OBJ)/%.o: $(PY_API)/%.cpp
	@mkdir -p $(@D)
	$(CXX) -c $(CPPFLAGS) $(PYTHON_H) -I$(PYBIND) $< -o $@
