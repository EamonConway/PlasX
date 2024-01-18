from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext
import sys
import subprocess
import os

# List of C++ source files
cpp_sources = ['mypackage/mymodule/mymodule.cpp']

# List of object files to be generated
object_files = ['build/temp.linux-x86_64-3.8/mymodule.o']

# Define the Pybind11 extension module without sources
ext_module = Extension(
    'mypackage.mymodule',
    sources=[],  # No sources here, as we will compile separately
    include_dirs=['path/to/your/pybind11/headers'],
    # Add other compiler/linker flags as needed
)


class BuildExtCommand(build_ext):
    def run(self):
        # Compile C++ source files to object files
        self.compile_objects()

        # Link the extension module to the object files
        self.link_extension_module()

    def compile_objects(self):
        for source_file, object_file in zip(cpp_sources, object_files):
            cmd = ['g++', '-c', source_file, '-o', object_file]
            self.spawn(cmd)

    def link_extension_module(self):
        cmd = ['g++', '-shared', '-o', ext_module.name + '.so'] + object_files
        self.spawn(cmd)


setup(
    name='mypackage',
    version='0.1',
    ext_modules=[ext_module],
    cmdclass={'build_ext': BuildExtCommand},
)
