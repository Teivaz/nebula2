//------------------------------------------------------------------------------
//  nwin32stacktrace.cc
//
//  FIXME FIXME FIXME
//  This class is not integrated yet!!!
//
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "kernel/nwinstacktrace.h"

//------------------------------------------------------------------------------
/*
*/
nStackTrace::nStackTrace()
{
	this->process = GetCurrentProcess();
}

//------------------------------------------------------------------------------
/*
*/
nStackTrace::~nStackTrace()
{
}

//------------------------------------------------------------------------------
/*
*/
void
nStackTrace::TraceStack(FILE* file)
{
	this->file = file;
	HANDLE thread = GetCurrentThread();
    CONTEXT context;
	ZeroMemory(&context, sizeof(context));
	context.ContextFlags = CONTEXT_FULL;
	GetThreadContext(thread, &context);
	this->WalkStack(thread, context);
}

//------------------------------------------------------------------------------
/*
*/
void
nStackTrace::WalkStack(HANDLE thread, CONTEXT& context)
{
	SymInitialize(this->process, 0 , true);

	STACKFRAME64 stackFrame;
	ZeroMemory(&stackFrame, sizeof(stackFrame));
	stackFrame.AddrPC.Offset = context.Eip;
	stackFrame.AddrPC.Mode = AddrModeFlat;
	stackFrame.AddrFrame.Offset = context.Ebp;
	stackFrame.AddrFrame.Mode = AddrModeFlat;


	for (int frameNum = 0; StackWalk64(
		IMAGE_FILE_MACHINE_I386,
		this->process, 
		thread, 
		&stackFrame, 
		&context,
		0 /* ReadMemoryRoutine */,
		SymFunctionTableAccess64,
		SymGetModuleBase64,
		NULL /* TranslateAddress */); frameNum++)
	{
		fprintf(this->file, "%3d. ", frameNum);
		this->ShowFrame(stackFrame);
		fprintf(this->file, "\n");
	}

	SymCleanup(this->process);
}

//------------------------------------------------------------------------------
/*
*/
void
nStackTrace::ShowFrame(STACKFRAME64& frame)
{
	const int maxNameLen = 512;
	SYMBOL_INFO* symbol = (SYMBOL_INFO*)malloc(sizeof(SYMBOL_INFO) + maxNameLen);
	ZeroMemory(symbol, sizeof(SYMBOL_INFO) + maxNameLen);
	symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
	symbol->MaxNameLen = maxNameLen;

	IMAGEHLP_LINE64 line;
	ZeroMemory(&line, sizeof(line));
	line.SizeOfStruct = sizeof(line);

	if (frame.AddrPC.Offset == 0)
	{
		fprintf(this->file, "Error: EIP=0\n");
		free(symbol);
		return;
	}

	DWORD64 offset;
	if (SymFromAddr(this->process, frame.AddrPC.Offset, &offset, symbol ))
		fprintf(this->file, "%-20s\t", symbol->Name);
	else
		fprintf(this->file, "(no symbol name)\t");

	DWORD displacement;
	if (SymGetLineFromAddr64(this->process, frame.AddrPC.Offset, &displacement, &line))
		fprintf(this->file, "<%s@%d>\n", line.FileName, line.LineNumber);
	else
		fprintf(this->file, "(no file name)\n");

	this->ShowLocals(frame);

	free(symbol);
}

//------------------------------------------------------------------------------
/*
*/
void
nStackTrace::ShowLocals(STACKFRAME64& frame)
{
	IMAGEHLP_STACK_FRAME sf;
	ZeroMemory(&sf, sizeof(sf));
	sf.BackingStoreOffset	= frame.AddrBStore.Offset;
	sf.FrameOffset			= frame.AddrFrame.Offset;
	sf.FuncTableEntry		= (ULONG64)frame.FuncTableEntry;
	sf.InstructionOffset	= frame.AddrPC.Offset;
	sf.Params[0]			= frame.Params[0];
	sf.Params[1]			= frame.Params[1];
	sf.Params[2]			= frame.Params[2];
	sf.Params[3]			= frame.Params[3];
	sf.ReturnOffset			= frame.AddrReturn.Offset;
	sf.StackOffset			= frame.AddrStack.Offset;
	sf.Virtual				= frame.Virtual;

	if (SymSetContext(this->process, &sf, 0))
	{
		Params params = {this->file, frame.AddrStack.Offset};
		if (!SymEnumSymbols(
			this->process,
			0, /* Base of Dll */
			"[a-zA-Z0-9_]*", /* mask */
			this->EnumSymbolsCallback,
			&params /* User context */
			))
		{
			fprintf(this->file, "\t(no symbols available)\n");
		}
	}
	else
	{
		fprintf(this->file, "\t(no context information)\n");
	}
}

//------------------------------------------------------------------------------
/*
*/
BOOL CALLBACK 
nStackTrace::EnumSymbolsCallback(PSYMBOL_INFO symbol, ULONG symbolSize, PVOID userContext)
{
	Params* params = (Params*)userContext;
	DWORD64 addr = params->base + symbol->Address - 8;
	DWORD	data = *((DWORD*)addr);

	fprintf(params->file, "\t%c %-20s = %08x\n", 
		(symbol->Flags & IMAGEHLP_SYMBOL_INFO_PARAMETER) ? 'P' : ' ',
		symbol->Name,
		data);

	return true;
}
