from glob import glob
from setuptools import setup
from pybind11.setup_helpers import Pybind11Extension
nlohmann_directory = "/Users/conway.e/repos"
odepp_directory = "/Users/conway.e/repos/odeplusplus/include"
cpp_directory = "../cpp/src/"
simulation_objects = [
    glob(cpp_directory+"PlasX/Vivax/White/*.cpp"), cpp_directory+"PlasX/random.cpp"]
print(simulation_objects)
CC = "gcc"
ext_modules = [
    Pybind11Extension("pyPlasX.mosquito._mosquito",
                      sources=[
                          "pyPlasX/mosquito/mosquito_wrapper.cpp"],
                      include_dirs=["../cpp/include",
                                    nlohmann_directory, odepp_directory],
                      library_dirs=["../lib"],
                      libraries=["plasx"],
                      extra_compile_args=['-std=c++2b']
                      ),
    Pybind11Extension("pyPlasX.mosquito._mosquito",
                      sources=[
                          "pyPlasX/mosquito/mosquito_wrapper.cpp"],
                      include_dirs=["../cpp/include",
                                    nlohmann_directory, odepp_directory],
                      library_dirs=["../lib"],
                      libraries=["plasx"],
                      extra_compile_args=['-std=c++2b']),
    Pybind11Extension("pyPlasX.pvibm._model",
                      sources=["pyPlasX/pvibm/model_wrapper.cpp"],
                      include_dirs=["../cpp/include",
                                    nlohmann_directory, odepp_directory],
                      library_dirs=["../lib"],
                      libraries=["plasx"],
                      extra_compile_args=['-std=c++2b']
                      ),
    Pybind11Extension("pyPlasX.pvibm._status",
                      sources=["pyPlasX/pvibm/status_wrapper.cpp"],
                      include_dirs=["../cpp/include",
                                    nlohmann_directory, odepp_directory],
                      library_dirs=["../lib"],
                      libraries=["plasx"],
                      extra_compile_args=['-std=c++2b']
                      ),
    Pybind11Extension("pyPlasX.pvibm._parameters",
                      sources=["pyPlasX/pvibm/parameters_wrapper.cpp"],
                      include_dirs=["../cpp/include",
                                    nlohmann_directory, odepp_directory],
                      library_dirs=["../lib"],
                      libraries=["plasx"],
                      extra_compile_args=['-std=c++2b']
                      ),
    Pybind11Extension("pyPlasX.pvibm._population",
                      sources=["pyPlasX/pvibm/population_wrapper.cpp"],
                      include_dirs=["../cpp/include",
                                    nlohmann_directory, odepp_directory],
                      library_dirs=["../lib"],
                      libraries=["plasx"],
                      extra_compile_args=['-std=c++2b']
                      ),
    Pybind11Extension("pyPlasX.pvibm._equilibrium",
                      sources=["pyPlasX/pvibm/equilibrium_wrapper.cpp"],
                      include_dirs=["../cpp/include",
                                    nlohmann_directory, odepp_directory],
                      library_dirs=["../lib"],
                      libraries=["plasx"],
                      extra_compile_args=['-std=c++2b']
                      ),
]
setup(ext_modules=ext_modules)
