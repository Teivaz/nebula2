from distutils.core import setup
setup(name = "foo", version = "1.0",
      py_modules = ["nebula"],
      data_files = [("dlls",["../../../../bin/win32/pynebula.dll"])], 
)
