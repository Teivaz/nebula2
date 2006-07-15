#ifndef N_LW_TOOLKIT_GLOBAL_H
#define N_LW_TOOLKIT_GLOBAL_H
//----------------------------------------------------------------------------
class nKernelServer;
class nLogHandler;

//----------------------------------------------------------------------------
/**
*/
class nLWToolkitGlobal
{
public:
	nLWToolkitGlobal();
	~nLWToolkitGlobal();

	/// needs to be called by any toolkit plugin when it is created
	static void PluginCreated();
	/// needs to be called by any toolkit plugin when it is killed
	static void PluginDestroyed();

private:
	nKernelServer* kernelServer;
	nLogHandler* logHandler;
};

//----------------------------------------------------------------------------
#endif // N_LW_TOOLKIT_GLOBAL_H
