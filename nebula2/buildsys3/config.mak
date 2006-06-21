#---------------------------------------------------------------------
#   config.mak
#   ==========
#   Configuration file for the make process
#   
#   27-Mar-2003 cubejk    copy&paste from nebula1 config.mak & defines.mak
#   22-Jan-2005 jurquhart should now compile on win32 and linux
#---------------------------------------------------------------------

#---------------------------------------------------------------------
# Generic options:
# N_DEBUG    - Build debug version
# N_PROFILE  - Incorporate profiling code
# VC_RUNTIME - Override default runtime library for vc++ (e.g. /MD)
#---------------------------------------------------------------------
N_DEBUG    = false
N_PROFILE  = false
VC_RUNTIME = /MT

#---------------------------------------------------------------------
# N_PLATFORM = __LINUX__, __WIN32__
# Automatically try to determine the platform we are running
# on. WinNT and Linux offer the 'OSTYPE' env var, Win9x not.
#---------------------------------------------------------------------
N_PLATFORM = __WIN32__
ifeq ($(OSTYPE),linux)
  N_PLATFORM  = __LINUX__
endif 
ifeq ($(OSTYPE),linux-gnu)
  N_PLATFORM = __LINUX__
endif
ifeq ($(OSTYPE),Linux)
  N_PLATFORM = __LINUX__
endif
ifeq ($(OSTYPE),darwin)
  N_PLATFORM = __MACOSX__
endif
ifeq ($(OSTYPE),darwin8.0)
  N_PLATFORM = __MACOSX__
endif

#---------------------------------------------------------------------
# N_TARGETDIR - where the exe and the lib files should stay
# N_INTERDIR  - where the object files should stay
#---------------------------------------------------------------------
# First, calculate end prefix
ifeq ($(N_DEBUG),true)
    N_POSTDIR = d
else
    N_POSTDIR = 
endif

# Now dump the directory paths
# HACK: also add on "-D<Base Platform>" to fix header kludge
ifeq ($(N_PLATFORM),__LINUX__)
    N_TARGETDIR = ../../bin/linux$(N_POSTDIR)/
    N_INTERDIR  = ./inter/linux$(N_POSTDIR)/
    BASECFLAGS += $(SYM_OPT)LINUX
endif
ifeq ($(N_PLATFORM),__WIN32__)
    N_TARGETDIR = ../../bin/win32$(N_POSTDIR)/
    N_INTERDIR  = ./inter/win32$(N_POSTDIR)/
    BASECFLAGS += $(SYM_OPT)WIN32
endif
ifeq ($(N_PLATFORM),__MACOSX__)
    N_TARGETDIR = ../../bin/macosx$(N_POSTDIR)/
    N_INTERDIR  = ./inter/macosx$(N_POSTDIR)/
    BASECFLAGS += $(SYM_OPT)MACOSX
endif

#---------------------------------------------------------------------
# N_COMPILER = __VC__, __GNUC__
# Based on N_PLATFORM, set the compiler type macro.
#---------------------------------------------------------------------
ifeq ($(N_COMPILER)X,X)
  ifeq ($(N_PLATFORM),__WIN32__)
    N_COMPILER = __VC__
  else 
    N_COMPILER = __GNUC__
  endif
endif

#---------------------------------------------------------------------
# Tools
# Assign useful system tools here
#---------------------------------------------------------------------
ifeq ($(N_PLATFORM),__LINUX__)
    RM = rm -f
    CHM_COMPILER =
endif
ifeq ($(N_PLATFORM),__WIN32__)
    RM = del
    CHM_COMPILER = c:/Program\ Files/HTML\ Help\ Workshop/hhc.exe
endif
ifeq ($(N_PLATFORM),__MACOSX__)
    RM = rm -f
    CHM_COMPILER =
endif

#---------------------------------------------------------------------
# N_NOTHREADS = true/false (default=false, except on OS X)
# Set to 'true' when you want to compile without multithreading
# support. This is mainly for debugging reasons. Please note
# that all sockets based code uses multithreading and will
# not function when multithreading is turned off.
#---------------------------------------------------------------------
#
ifeq ($(N_PLATFORM),__MACOSX__)
  N_NOTHREADS = true
else
  N_NOTHREADS = false
endif

#---------------------------------------------------------------------
# N_PYTHON_INCLUDES
# Defines include directory for Python
#---------------------------------------------------------------------
ifeq ($(N_PLATFORM),__LINUX__)
  N_PYTHON_INCLUDES = /usr/include/python2.4/
endif

ifeq ($(N_PLATFORM),__MACOSX__)
  N_PYTHON_INCLUDES = /usr/include/python2.2/
endif

#---------------------------------------------------------------------
# N_PYTHON_LIBS
# Defines lib directory for Python
#---------------------------------------------------------------------
ifeq ($(N_PLATFORM),__LINUX__)
  N_PYTHON_LIBS = /usr/lib/python2.4/config/
endif

#---------------------------------------------------------------------
# N_MEMMANAGER = true/false
# Turn on/off Nebula's own memory manager. The mem manager optimizes
# small block allocations, and offers some debugging support
# (bounds and mem leak checking).
# GCC doesn't like it to overload the global 'new' operator for
# some reason.
#---------------------------------------------------------------------
#
ifeq ($(N_COMPILER),__GNUC__)
  N_MEMMANAGER = false
else
  N_MEMMANAGER = true
endif

#---------------------------------------------------------------------
ifeq ($(N_COMPILER),__VC__)
# VisualC under Win32
  RC          = rc
  CC          = cl /TC
  CXX         = cl /TP
  TCL         = tclsh84
  AR          = link /lib
  LD          = link
  DOCTOOL     = doxygen
  IPATH_OPT   = /I
  LPATH_OPT   = /LIBPATH:
  TAR_OPT     =
  LIB_OPT     =
  LIB_OPT_POST= .lib
  EXE         = .exe
  OBJ         = .obj
  DLL_PRE     =
  DLL_POST    = .dll
  LIB_PRE     =
  LIB_POST    = .lib
  OBJ_OPT     = /Fo
  OUT_OPT     = /Fe
  NOLINK_OPT  = /c
  SYM_OPT     = /D
  #flags
  N_WARNFLAGS     = /W3
  N_WARNFLAGS_C   = /W3
  N_OPTIMIZEFLAGS = $(VC_RUNTIME) /Ox /GF /D"_WINDOWS" /D"NT_PLUGIN" /nologo
  N_OPTIMIZELFLAGS= /NOLOGO #/OPT:REF /INCREMENTAL:NO
  N_PROFILEFLAGS  = 
  N_DEBUGFLAGS    = $(VC_RUNTIME)d /Od /Zi /GF /D"_WINDOWS" /D"NT_PLUGIN" /nologo
  N_DEBUGLFLAGS   = /NOLOGO /DEBUG #/INCREMENTAL:YES /NOLOGO
  INC_PATH        = ../../code/nebula2/inc/
  RCFLAGS         =
endif
ifeq ($(N_COMPILER),__GNUC__)
# GCC in Linux
  ifeq ($(N_PLATFORM),__MACOSX__)
    CXX       = c++
    CC        = c++ -x c
  else
    CXX       = g++
    CC        = gcc -x c
  endif
  RC          =
  TCL         = tclsh
  AR          = ar
  LD          = ld
  DOCTOOL     = doxygen
  IPATH_OPT   = -I
  LPATH_OPT   = -L
  FWORK_OPT   = -framework 
  TAR_OPT     = -o
  LIB_OPT     = -l
  LIB_OPT_POST= 
  EXE         = 
  OBJ         = .o
  DLL_PRE     = lib
  DLL_POST    = .so
  LIB_PRE     = lib
  LIB_POST    = .a
  OBJ_OPT     = -o
  OUT_OPT     = -o
  NOLINK_OPT  = -c
  SYM_OPT     = -D
  #flags
  N_WARNFLAGS     = -W -Wall -Wno-multichar -Wno-reorder
  N_WARNFLAGS_C   = -W -Wall -Wno-multichar 
  N_OPTIMIZEFLAGS = -O3 -ffast-math -fomit-frame-pointer
  N_OPTIMIZELFLAGS= 
  N_PROFILEFLAGS  = -p
  N_DEBUGFLAGS    = -g
  N_DEBUGLFLAGS   = 
  INC_PATH        = ../../code/nebula2/inc/
  RCFLAGS         =
endif

#---------------------------------------------------------------------
# system specific setup
#---------------------------------------------------------------------
ifeq ($(N_COMPILER),__GNUC__)
  INCDIR += $(IPATH_OPT)$(INC_PATH)
  INCDIR += $(IPATH_OPT)$(N_PYTHON_INCLUDES)
  
  LIBDIR += $(LPATH_OPT)$(N_TARGETDIR)
  ifneq (X$(N_PYTHON_LIBS),X)
    LIBDIR += $(LPATH_OPT)$(N_PYTHON_LIBS)
  endif
  
  ifeq ($(N_PLATFORM),__LINUX__)
    LIBDIR += $(LPATH_OPT)/usr/X11R6/lib
  endif
  
  ifeq ($(N_PLATFORM),__MACOSX__)
    OSX_CFLAGS = -fno-common
  endif
  
  ifeq ($(N_MEMMANAGER),true)
    BASECFLAGS += $(SYM_OPT)__NEBULA_MEM_MANAGER__
  endif
  
  LIBS += $(LIB_OPT)m
  
  #threads
  ifeq ($(N_NOTHREADS),false) 
    BASECFLAGS += $(SYM_OPT)_REENTRANT
    LIBS   += $(LIB_OPT)pthread
  else
    BASECFLAGS += $(SYM_OPT)__NEBULA_NO_THREADS__
  endif
  
endif

ifeq ($(N_COMPILER),__VC__)
  INCDIR = $(IPATH_OPT)$(INC_PATH)
  LIBDIR = $(LPATH_OPT)"$(N_TARGETDIR)" $(LPATH_OPT)"../../code/lib/win32_vc_i386"

  ifeq ($(N_MEMMANAGER),true)
    BASECFLAGS += $(SYM_OPT)__NEBULA_MEM_MANAGER__
  endif
  ifeq ($(N_NOTHREADS),false) 
    BASECFLAGS += $(SYM_OPT)_REENTRANT
  else
    BASECFLAGS += $(SYM_OPT)__NEBULA_NO_THREADS__
  endif

endif

#---------------------------------------------------------------------
# CFLAGS / CXXFLAGS
# release - default , debug , profile
#---------------------------------------------------------------------
BASECFLAGS += $(SYM_OPT)$(N_PLATFORM) $(SYM_OPT)N_STATIC

# Add on Debug, Optimization or Profiling flags to base
ifeq ($(N_DEBUG),true)
  BASECFLAGS   += $(SYM_OPT)_DEBUG $(N_DEBUGFLAGS)
  LFLAGS       += $(N_DEBUGLFLAGS)
else
  BASECFLAGS   += $(N_OPTIMIZEFLAGS)
  LFLAGS       += $(N_OPTIMIZELFLAGS)
endif
ifeq ($(N_PROFILE),true)
  BASECFLAGS   += $(N_PROFILEFLAGS)
endif

# C flags
CFLAGS += $(BASECFLAGS) $(N_WARNFLAGS_C) $(INCDIR)

# C++ flags
CXXFLAGS += $(BASECFLAGS) $(N_WARNFLAGS) $(INCDIR)

#---------------------------------------------------------------------
# Generic compiler targets
#---------------------------------------------------------------------
ifeq ($(N_COMPILER),__VC__)
%.res: %.rc
	$(RC) $(RCFLAGS) $(OBJ_OPT)$@ $(<)
else
    %.res:
endif

