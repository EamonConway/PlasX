# Define flags for compilation.
CXX = g++  -Wall -Wpedantic -Werror -flto -fPIC
INCLUDE = include
NLOHMANN = ../json/single_include/
ODEPP = ../odeplusplus/include
PYBIND = ../pybind11/include
CPPFLAGS = -std=c++2a -I$(INCLUDE) -I$(NLOHMANN) -I$(ODEPP)
CFLAGS =
PY_OBJ = pybuild
OBJ = build
SRC = src
TEST = test
PY_API = pysrc

SOURCES := $(shell find $(SRC) -name "*.cpp")
OBJECTS := $(patsubst $(SRC)/%.cpp, $(OBJ)/%.o, $(SOURCES))
TEST_SOURCES := $(shell find $(TEST) -name "*.cpp")
TEST_OBJECTS := $(patsubst $(TEST)/%.cpp, $(OBJ)/%.o, $(TEST_SOURCES))
PYBIND_SRC := $(shell find $(PY_API) -name "*py*.cpp")
PYBIND_OBJ := $(patsubst $(PY_API)/%.cpp, $(PY_OBJ)/%.o, $(PYBIND_SRC))
PYTHON_H := $(shell python3-config --includes)
PYTHON_EXTENSION := $(shell python3-config --extension-suffix)

python-library: pvibm-mosquito objects pybind_objects
	@mkdir -p pybin
	@echo $(PYBIND_OBJ)
	g++ -O3 -Wall -shared -std=c++2a $(python3-config --includes) -I$(PYBIND) -o pybin/pyPlasX$(PYTHON_EXTENSION) $(PYBIND_OBJ) $(OBJECTS)
	#g++ -O3 -Wall -shared -std=c++2a -undefined dynamic_lookup $(python3-config --includes) -I$(PYBIND) -o pybin/pyPlasX$(PYTHON_EXTENSION) $(PYBIND_OBJ) $(OBJECTS)

pvibm-mosquito: pvibm-equilibrium tests objects
	@mkdir -p bin
	#$(CXX) $(CPPFLAGS) -o bin/plasx-pvibm-mosquito main_mosquito.cpp $(OBJECTS)

pvibm-equilibrium: tests objects
	@mkdir -p bin
	$(CXX) $(CPPFLAGS) -o bin/plasx-pvibm-equilibrium plasx-pvibm-equilibrium.cpp $(OBJECTS)

tests: objects test_objects
	@mkdir -p bin
	$(CXX) $(CPPFLAGS) -o bin/TEST_runner $(TEST_OBJECTS) $(OBJECTS) -lgtest -pthread

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
	$(CXX) -c $(CPPFLAGS) $< -o $@

$(PY_OBJ)/%.o: $(PY_API)/%.cpp
	@mkdir -p $(@D)
	$(CXX) -c $(CPPFLAGS) $(PYTHON_H) -I$(PYBIND) $< -o $@
