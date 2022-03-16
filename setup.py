import numpy
from distutils.core import setup, Extension

def main():
    setup(name="julia_set_c",
          version="1.0.0",
          description= "Python interface for generating Julia set fractals",
          author="",
          author_email="",
          ext_modules=[Extension(
			"julia_set_c",
			["julia_set.c"],
			include_dirs=[numpy.get_include()])])

if __name__ == "__main__":
    main()

