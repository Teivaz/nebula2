#ifndef INPUT_SERVER_H
#define INPUT_SERVER_H
//------------------------------------------------------------------------------
/**
    @class Input::Server

    The Input::Server object is the central object of the input
    subsystem.

    (C) 2003 RadonLabs GmbH
*/
#include "message/port.h"
#include "input/ninputevent.h"

class nInputServer;

namespace Message
{
    class Port;
};

//------------------------------------------------------------------------------
namespace Input
{
class Mapping;
class Source;

class Server : public Message::Port
{
    DeclareRtti;
	DeclareFactory(Server);

public:
    /// constructor
    Server();
    /// destructor
    virtual ~Server();
    /// get instance pointer
    static Server* Instance();
    /// initialize the input subsystem
    virtual bool Open();
    /// close the input subsystem
    virtual void Close();
    /// set the current time
    virtual void SetTime(nTime t);
    /// per-frame trigger method, generates input events
    virtual void Trigger();

    /// listen to mouse move messages, to keep track of the current mouse position
    virtual bool Accepts(Message::Msg* msg);
    /// handle mouse move input events
    virtual void HandleMessage(Message::Msg* msg);

    /// add a mapping that listens to the given device:channel
    void AddMapping(const nString& mapping, const nString& deviceChannel);
    /// remove mapping
    void RemMapping(const nString& mapping);

    /// get number of mappings
    int GetNumMappings() const;
    /// get mapping a index i
    Mapping* GetMappingAt(int i);

    /// has a mapping with the given name
    bool HasMapping(const nString& name) const;
    /// get the mapping with the given name
    Mapping* GetMapping(const nString& name) const;

    /// attach port to mapping (return ptr to mapping, so a sink that is attached to multiple mapping could know which one sends now)
    Mapping* AttachInputSink(const nString& mappingName, int priority, Message::Port* sink);
    /// remove input sink from mapping (removes the sink from all priorities)
    void RemoveInputSink(const nString& mappingName, Message::Port* sink);
    /// remove input sink with given priority from mapping
    void RemoveInputSink(const nString& mappingName, int priority, Message::Port* sink);

    // DO NOT USE THOSE POLLING FUNCTIONS: attach a input sink to the mapping and react on events!
    /// get a slider value
    virtual float GetSlider(const nString& mappingName) const;

    /// is a button gone up in the last frame?
    bool GetButtonUp(const nString& mappingName) const;
    /// is a button gone down in the last frame?
    bool GetButtonDown(const nString& mappingName) const;
    /// is a button pressed?
    bool GetButtonPressed(const nString& mappingName) const;
    /// was a button clicked in the last frame?
    bool GetButtonClicked(const nString& mappingName) const;
    /// was a button double clicked in the last frame?
    bool GetButtonDoubleClicked(const nString& mappingName) const;

    /// convenience - keep track of current mouse position
    /// get current mouse position
    virtual const vector2& GetMousePos() const;

protected:
    /// create an appropriate input source for the given device channel
    Ptr<Source> CreateInputSource(const nString& deviceChannel) const;

    /// cleanup empty elements from mapping array
    void CleanupEmptyMappings();

    /// begin the input frame
    void BeginInputFrame();
    /// consume the raw events of this frame
    void Consume();
    /// emit the generated input events of this frame
    void Emit();
    /// end the input frame
    void EndInputFrame();

    /// helper to get a attribute from from device channel
    int GetDeviceAttr(const nString& deviceName, const nString& channelName, const nString& attribute) const;
    /// does a exists this device channel combination
    bool ExistsDeviceChannel(const nString& deviceName, const nString& channelName) const;

    /// TODO: parse the player profile and map key to game input states (the application has to declare the input and default mappings before)
    //void LoadBindingsFromPlayerProfile();
    /// TODO: save the current mapped keys into the player profile
    //void SaveBindingsToPlayerProfile();

private:
    static Server* Singleton;

    bool isOpen;
    nRef<nInputServer> refInputServer;
    nTime time;

    vector2 mousePosition;
    bool isInTrigger;
    nArray<Ptr<Mapping> > mappings;
};

RegisterFactory(Server);

//------------------------------------------------------------------------------
/**
*/
inline
Server*
Server::Instance()
{
    n_assert(0 != Server::Singleton);
    return Server::Singleton;
}

} // namespace Input
//------------------------------------------------------------------------------
#endif
