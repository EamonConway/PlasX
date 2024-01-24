from glob import glob
from setuptools import setup
from pybind11.setup_helpers import Pybind11Extension
nlohmann_directory = "/Users/conway.e/repos"
odepp_directory = "/Users/conway.e/repos/odeplusplus/include"
ext_modules = [
    Pybind11Extension(
        "python_example",
        sources=glob("../cpp/src/**/*.cpp", recursive=True),
        include_dirs=["../cpp/include", nlohmann_directory, odepp_directory],
        extra_compile_args=['-std=c++2b']
    ),
]
setup(ext_modules=ext_modules)
