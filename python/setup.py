from distutils.core import setup
from distutils.extension import Extension
from Cython.Build import cythonize

sourcefiles = ["nj.pyx","../src/nj.c"]

extensions = [Extension("nj", sourcefiles)]

setup(
	ext_modules=cythonize(extensions)
)
