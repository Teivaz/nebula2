#ifndef N_STACKTRACE_H
#define N_STACKTRACE_H
//------------------------------------------------------------------------------
/**
    FIXME FIXME FIMXE

    This class implements experimental stacktracing under Win32, and 
    hasn't been integrated so far with the rest of the code. Should be
    unified with Bruce's Linux stacktracer (can we use a similar approach
    like the nLogHandler class? One base class with platform specific
    subclasses? May require some work on the interface though.

    -Floh.

    (C) 2003 RadonLabs GmbH
*/
#include <stdio.h>
#include <windows.h>
#include <dbghelp.h>

class nStackTrace
{
public:
	/// constructor
	nStackTrace();
	/// destructor
	~nStackTrace();

	/// write stack trace to given file
	void TraceStack(FILE* file);
protected:
	/// trace stack of given thread
	void WalkStack(HANDLE thread, CONTEXT& context);
	/// show single frame
	void ShowFrame(STACKFRAME64& frame);
	/// show local symbols of frame
	void ShowLocals(STACKFRAME64& frame);

	/// callback for local symbols
	static BOOL CALLBACK EnumSymbolsCallback(PSYMBOL_INFO symbol, ULONG symbolSize, PVOID userContext);

	/// handle of current process
	HANDLE process;
	/// file for output
	FILE* file;

	/// helper structfor parameter passing
	struct Params {
		FILE* file;
		DWORD64 base;
	};
};

#endif
