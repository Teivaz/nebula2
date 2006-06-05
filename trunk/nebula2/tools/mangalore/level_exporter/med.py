#-------------------------------------------------------------------------------
# med.py
#-------------------------------------------------------------------------------

import sys
if not hasattr(sys, 'hexversion') or sys.hexversion < 0x020400f0:
    sys.exit('Python 2.4.0 or later is required to run mangalore level editor')

try:
    import wx
except ImportError:
    sys.exit('wxPython 2.5.0 or later is required to run mangalore level editor.')
else:
    if wx.VERSION < (2, 5):
        sys.exit('wxPython 2.5.0 or later is required to run mangalore level editor.')

try:
    import pysqlite2.dbapi2 as sqlite
except ImportError:
    sys.exit('PySQLite 2.1.0 or later is required to run mangalore level editor.')
else:
    if sqlite.version < '2.1':
        sys.exit('PySQLite 2.1.0 or later is required to run mangalore level editor.')

from med.gui.mainapp import MainApp

def main():
    app = MainApp()
    app.MainLoop()

if __name__ == '__main__':
    main()

#-------------------------------------------------------------------------------
# Eof
#-------------------------------------------------------------------------------