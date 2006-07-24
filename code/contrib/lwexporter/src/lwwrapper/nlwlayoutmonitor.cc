//----------------------------------------------------------------------------
// (c) 2006    Vadim Macagon
//----------------------------------------------------------------------------
#include "lwwrapper/nlwlayoutmonitor.h"

//----------------------------------------------------------------------------
/**
    @param title The title to be used for the monitor window.
    @param flags A bit-wise OR of any of the following:
                 - LMO_NOABORT:    Don't allow the user to abort.
                 - LMO_REVIEW:     Wait for the user to close the monitor
                                   window.
                 - LMO_HISTAPPEND: Append to the history file instead of
                                   overwritting.
                 - LMO_IMMUPD: Immediately update the monitor on every step.
    @param logFileName Filename of the log file to which messages displayed
                       to the user will be written. Can be NULL in which case
                       no log file will be generated.
*/
nLWLayoutMonitor::nLWLayoutMonitor(const nString& title, uint flags,
                                   const char* logFileName) :
    title(title),
    logFileName(logFileName),
    lwMonitor(0),
    userAborted(false)
{
    n_assert(this->monitorGlobal.IsValid());
    if (!this->monitorGlobal.IsValid())
        return;

    this->lwMonitor = this->monitorGlobal.Get()->create();
    this->monitorGlobal.Get()->setup(this->lwMonitor,
                                     const_cast<char*>(this->title.Get()),
                                     flags,
                                     logFileName ? this->logFileName.Get() : 0);
}

//----------------------------------------------------------------------------
/**
*/
nLWLayoutMonitor::~nLWLayoutMonitor()
{
    if (!this->lwMonitor)
        return;

    n_assert(this->monitorGlobal.IsValid());
    if (!this->monitorGlobal.IsValid())
        return;

    this->monitorGlobal.Get()->destroy(this->lwMonitor);
    this->lwMonitor = 0;
}

//----------------------------------------------------------------------------
/**
    @brief Set the position and size of the monitor window.
*/
void
nLWLayoutMonitor::SetBounds(int x, int y, int width, int height)
{
    n_assert(this->monitorGlobal.IsValid());
    if (!this->monitorGlobal.IsValid())
        return;

    this->monitorGlobal.Get()->setwinpos(this->lwMonitor, x, y, width, height);
}

//----------------------------------------------------------------------------
/**
    @brief Open the monitor window.
    @param totalSteps Total number of steps the progress bar should represent.
    @param firstMsg The first progress message.
*/
void
nLWLayoutMonitor::Open(uint totalSteps, const nString& firstMsg)
{
    n_assert(this->monitorGlobal.IsValid());
    if (!this->monitorGlobal.IsValid())
        return;

    this->curMsg = firstMsg;
    this->monitorGlobal.Get()->init(this->lwMonitor, totalSteps,
                                    this->curMsg.Get());
}

//----------------------------------------------------------------------------
/**
    @brief Advance the progress bar and/or display a new message.
    @param increment The number of steps the progress bar should be
                     advanced by. Can be zero if you only want to change the
                     message.
    @param msg A new message to display, can be empty in which case Lightwave
               will provide a generic progress message.
    @return false if the user requested that the task be aborted or if some
            kind of error occurred, true otherwise.
*/
bool
nLWLayoutMonitor::Step(uint increment, const nString& msg)
{
    n_assert(this->monitorGlobal.IsValid());
    if (!this->monitorGlobal.IsValid())
        return false;

    this->curMsg = msg;
    int abort = this->monitorGlobal.Get()->step(this->lwMonitor, increment,
                                                msg.IsEmpty() ? 0 : this->curMsg.Get());
    if (!this->userAborted)
    {
        this->userAborted = (1 == abort);
    }
    return this->userAborted;
}

//----------------------------------------------------------------------------
/**
    @brief Close the monitor window.

    If the monitor was created with the LMO_REVIEW flag set then this method
    will block until the user has closed the monitor window.
    If the LMO_REVIEW flag wasn't set the monitor window will be closed
    immediately.
*/
void
nLWLayoutMonitor::Close()
{
    n_assert(this->monitorGlobal.IsValid());
    if (!this->monitorGlobal.IsValid())
        return;

    this->monitorGlobal.Get()->done(this->lwMonitor);
}

//----------------------------------------------------------------------------
/**
    @brief Check if the user pressed the abort button.
    @return true if the user aborted, false otherwise.
*/
bool
nLWLayoutMonitor::UserAborted() const
{
    return this->userAborted;
}

//----------------------------------------------------------------------------
// EOF
//----------------------------------------------------------------------------
