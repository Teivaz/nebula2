#ifndef N_LW_CUSTOM_OBJECT_HANDLER_H
#define N_LW_CUSTOM_OBJECT_HANDLER_H
//----------------------------------------------------------------------------
extern "C" 
{
#include <lwsdk/lwserver.h>
#include <lwsdk/lwcustobj.h>
}

//----------------------------------------------------------------------------
/**
    @class nLWCustomObjectHandlerBase
    @brief Pointless remnant of the initial design?
*/
class nLWCustomObjectHandlerBase
{
public:
    virtual ~nLWCustomObjectHandlerBase();
    virtual LWError OnLoad(const LWLoadState*);
    virtual LWError OnSave(const LWSaveState*);
    virtual const char* OnGetDescription() = 0;
    virtual const LWItemID* OnUseItems();
    virtual void OnChangeID(const LWItemID* idList);
    virtual void OnEvaluate(const LWCustomObjAccess*);
    virtual unsigned int OnFlags();
    virtual LWError OnDisplayUI();
};

//----------------------------------------------------------------------------
#endif // N_LW_CUSTOM_OBJECT_HANDLER_H
