#ifndef N_DIRECTORY_WATCH_HANDLER_H
#define N_DIRECTORY_WATCH_HANDLER_H
//------------------------------------------------------------------------------
/**
    @class nDirectoryWatchHandler
    @ingroup File

    notified when a directory is modified in some way

    handling of directory modification notification events

    (C) 2006 Anthony Whitaker
*/

#include "kernel/nref.h"
#include "kernel/nthread.h"
#include "util/nstring.h"

class nDirectory;

//------------------------------------------------------------------------------
class nDirectoryWatchHandler
{
public:
    /// constructor
    nDirectoryWatchHandler();
    /// destructor
    virtual ~nDirectoryWatchHandler();

    /// Begin watching a directory for file changes
    void Watch(nString& directory);
    /// Stop watching the directory
    void Unwatch();
    /// Returns current state
    bool IsWatching() const;

    /// called when a directory is changed
    virtual void OnChange(nString& filename) = 0;

protected:
    /// the file watching thread function
    static int N_THREADPROC WatchThreadFunc(nThread* thread);

private:
    nThread* watchThread;
    nString path;
#ifdef __WIN32__
    HANDLE directoryHandle;
#endif
};

//------------------------------------------------------------------------------
/**
*/
inline
bool
nDirectoryWatchHandler::IsWatching() const
{
    return !!watchThread;
}

//------------------------------------------------------------------------------
#endif
