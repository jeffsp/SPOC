from pybind11.setup_helpers import Pybind11Extension, build_ext
from setuptools import setup

__version__ = "0.1.1"

ext_modules = [
    Pybind11Extension("spocpy.spocpy_cpp",
                      ["spocpy/spocpy.cpp"],
                      include_dirs=['..'],
                      extra_compile_args=['-fopenmp'],
                      extra_link_args=['-lgomp', '-lz'],
                      define_macros=[('VERSION_INFO', __version__)],),
]

setup(
    name="spocpy",
    version=__version__,
    author="spocfile.xyz",
    author_email="spoc@spocfile.xyz",
    description="SPOC Py",
    long_description="SPOC file support for python",
    packages=['spocpy'],
    ext_modules=ext_modules,
    cmdclass={"build_ext": build_ext},
    zip_safe=False,  # Ensure a platform-specific wheel is created
    python_requires=">=3.6",
)
