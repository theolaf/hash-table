from setuptools import setup, Extension, find_packages
import os
import sys

# Define the extension module
ext_modules = [
    Extension(
        '_hashtable',
        sources=[
            '_hashtablemodule.c',
            '../src/hash_table.c',
            '../src/utils.c'
        ],
        include_dirs=[
            '../include',
            '../src'
        ],
        extra_compile_args=[],
        extra_link_args=[],
        define_macros=[],
        undef_macros=[],
    ),
]

# Setup configuration
setup(
    name='hashtable',
    version='0.1.0',
    description='Python C extension for hash table implementation',
    author='TheOlaf',
    packages=find_packages(),
    ext_modules=ext_modules,
    python_requires='>=3.6',
    classifiers=[
        'Development Status :: 3 - Alpha',
        'Intended Audience :: Developers',
        'License :: OSI Approved :: MIT License',
        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: 3.6',
        'Programming Language :: Python :: 3.7',
        'Programming Language :: Python :: 3.8',
        'Programming Language :: Python :: 3.9',
        'Programming Language :: Python :: 3.10',
        'Programming Language :: Python :: 3.11',
        'Programming Language :: C',
    ],
)