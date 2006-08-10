//----------------------------------------------------------------------------
//  %(appNameL)s.cc
//
//  (C)%(curYear)s %(author)s
//----------------------------------------------------------------------------
#ifdef __WIN32__
#include "tools/nwinmaincmdlineargs.h"
#else
#include "tools/ncmdlineargs.h"
#endif
#include "%(subDirL)s/%(classNameL)s.h"

//----------------------------------------------------------------------------
/**
*/
#ifdef __WIN32__
int WINAPI
WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    nWinMainCmdLineArgs args(lpCmdLine);
#else
int
main(int argc, const char** argv)
{
    nCmdLineArgs args(argc, argv);
#endif

    // create the application.
    %(className)s *%(appObjName)s = n_new(%(className)s);

    // Uncomment and add your own project directory if it is necessary.
    //%(appObjName)s->SetProjectDirectory(projDir);
    // Uncomment and add your own startup script if it is necessary.
    //%(appObjName)s->SetStartupScript("home:<your own startup script>");

    if (%(appObjName)s->Open())
    {
        %(appObjName)s->Run();
    }

    %(appObjName)s->Close();
    n_delete(%(appObjName)s);
    return 0;
}

