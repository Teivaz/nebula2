//------------------------------------------------------------------------------
//  (C) 2006 Anthony Whitaker
//------------------------------------------------------------------------------
#include "kernel/ndirectorywatchhandler.h"

//------------------------------------------------------------------------------
/**
*/
nDirectoryWatchHandler::nDirectoryWatchHandler() :
    watchThread(0)
#ifdef __WIN32__
    , directoryHandle(0)
#endif
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nDirectoryWatchHandler::~nDirectoryWatchHandler()
{
    n_assert(this->watchThread == 0);
}

//------------------------------------------------------------------------------
/**
    called when a directory is watched
*/
void
nDirectoryWatchHandler::Watch(nString& directory)
{
    n_assert(directory.Length());
    this->path = directory;

#ifdef __WIN32__
    // Get the handle of the directory itself.
    this->directoryHandle = CreateFile(this->path.Get(), FILE_LIST_DIRECTORY | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | 
                                        FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, NULL);

    if (this->directoryHandle)
    {
        this->watchThread = n_new(nThread(this->WatchThreadFunc, nThread::Normal, 0, 0, 0, (void*)this));
    }
#endif
}

//------------------------------------------------------------------------------
/**
    called when a directory is unwatched
*/
void
nDirectoryWatchHandler::Unwatch()
{
    n_assert(this->watchThread);
    n_delete(this->watchThread);
    this->watchThread = 0;

#ifdef __WIN32__
    if (this->directoryHandle) 
    {
        CloseHandle(this->directoryHandle);
        this->directoryHandle = NULL;
    }
#endif
}

//------------------------------------------------------------------------------
/**
    the file watching thread function
*/
int
N_THREADPROC
nDirectoryWatchHandler::WatchThreadFunc(nThread* thread)
{
    thread->ThreadStarted();

    nDirectoryWatchHandler* watchHandler = (nDirectoryWatchHandler *)thread->LockUserData();
    thread->UnlockUserData();

#ifdef __WIN32__
    FILE_NOTIFY_INFORMATION fileChanges[16];
    DWORD bytesReturned;
    OVERLAPPED overlapped = { 0, };
    HANDLE event = CreateEvent(0,0,1,0);
    overlapped.hEvent = event;

    // NOTE: We're using the asynchronous version of the function.
    while (ReadDirectoryChangesW(
        watchHandler->directoryHandle,                      // handle to a directory
        &fileChanges,                                       // out array for notification events
        sizeof(fileChanges),                                // sizeof array
        TRUE,                                               // watch subdirectories
        FILE_NOTIFY_CHANGE_LAST_WRITE,                      // Types of file events to listen for
        &bytesReturned,                                     // number of bytes returned
        &overlapped,                                        // OVERLAPPED structure (async only)
        0)                                                  // completion routine (not needed with this async method)
        && !thread->ThreadStopRequested())
    {
        if (WAIT_OBJECT_0 == WaitForSingleObject(event, 100))
        {
            int currentOffset = 0;
            FILE_NOTIFY_INFORMATION* notification = fileChanges;

            do
            {
                notification = (FILE_NOTIFY_INFORMATION*)((char*)notification + currentOffset);

                if (FILE_ACTION_MODIFIED == notification->Action)
                {
                    char pathName[MAX_PATH];
                    int nameLength = notification->FileNameLength / 2;

                    // This is a necessary evil. ReadDirectoryChangesW does not have an ascii equivalent. So we're going to convert the path 
                    // to ascii, and then we have to convert the ascii path to it's long path name, since it could be an 8.3 short filename.
                    if (WideCharToMultiByte(CP_ACP, 0, notification->FileName, nameLength, pathName, sizeof(pathName), NULL, NULL))
                    {
                        pathName[nameLength] = '\0';

                        // NOTE: Be weary of this, it doesn't specify if the src and dest buffers are allowed to be the same.
                        // This may also be redundant, but there doesn't appear to be a safe way to determine if we actually
                        // have the long file name or the 8.3 version.
                        GetLongPathName(pathName, pathName, sizeof(pathName));  // Just in case we get an 8.3 short filename.
                        watchHandler->OnChange(nString(pathName));
                    }
                }

                currentOffset += notification->NextEntryOffset;
            } while(notification->NextEntryOffset);

            ResetEvent(event);
        }
    }

    CloseHandle(event);
#endif

#if defined(__WIN32__) && !defined(_WIN32_WINNT)
#   error _WIN32_WINNT has to be defined as 0x400 or higher (Try _WIN32_WINNT=WINVER).
#endif

    thread->ThreadHarakiri();
    return 0;
}
