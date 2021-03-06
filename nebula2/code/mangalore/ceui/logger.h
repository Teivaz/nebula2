//------------------------------------------------------------------------------
//  ceui/logger.h
//  (c) 2006 Nebula2 Community
//------------------------------------------------------------------------------
#ifndef CEUI_LOGGER_H
#define CEUI_LOGGER_H

#include "CEGUILogger.h"

namespace CEUI
{

class Logger : public CEGUI::Logger
{
public:
    /// constructor
    Logger();
    /// destructor
    virtual ~Logger();
    ///
    virtual void logEvent(const CEGUI::String& message, CEGUI::LoggingLevel level);
    ///
    virtual void setLogFilename(const CEGUI::String& filename, bool append);
};

} // namespace CEUI
//------------------------------------------------------------------------------
#endif
