//------------------------------------------------------------------------------
//  input/server.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "input/server.h"
#include "input/ninputserver.h"
#include "message/server.h"
#include "kernel/nscriptserver.h"
#include "gui/nguiserver.h"
#include "gui/nguidragbox.h"
#include "ui/server.h"
#ifdef USE_MCEGUI
#include "ceui/server.h"
#endif
#include "input/mapping.h"
#include "input/chain.h"
#include "input/event.h"
#include "input/source.h"
#include "input/axissource.h"
#include "input/mousesource.h"
#include "input/buttonsource.h"
#include "input/keysource.h"
#include "input/priority.h"

namespace Input
{
ImplementRtti(Input::Server, Foundation::RefCounted);
ImplementFactory(Input::Server);

Server* Server::Singleton = 0;

//------------------------------------------------------------------------------
/**
*/
Server::Server() :
    isOpen(false),
    time(0.0),
    isInTrigger(false)
{
    n_assert(0 == Singleton);
    Singleton = this;
}

//------------------------------------------------------------------------------
/**
*/
Server::~Server()
{
    n_assert(!this->isOpen);
    n_assert(0 != Singleton);
    Singleton = 0;
}

//------------------------------------------------------------------------------
/**
    Open the input subsystem.
*/
bool
Server::Open()
{
    n_assert(!this->isOpen);

    // Open the N2 input server here as we should have a window by now.
    nInputServer::Instance()->Open();

    // define input mapping by calling external script function
    nScriptServer* scriptServer = Foundation::Server::Instance()->GetScriptServer();
    nString result;
    scriptServer->Run("OnMapInput", result);

    // make sure the expected mappings exist to keep track of the mouse position
    if (!this->HasMapping("mousePosition"))
    {
        n_error("Input::Server::Open(): Warning: Could not keep track of the mouse position, does not find the expected mapping 'mousePosition'!");
    }

    // attach to the mouse input mapping, to keep track of the current mouse position
    this->AttachInputSink("mousePosition", InputPriority_MousePositionTracking, this);

    this->isOpen = true;
    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
Server::Accepts(Message::Msg* msg)
{
    n_assert(msg);
    if (msg->CheckId(Input::Event::Id)) return true;
    return Message::Port::Accepts(msg);
}

//------------------------------------------------------------------------------
/**
*/
void
Server::HandleMessage(Message::Msg* msg)
{
    n_assert(msg);
    if (msg->CheckId(Input::Event::Id))
    {
        Event* event = static_cast<Event*>(msg);
        if (Event::MouseMoved == event->GetType())
        {
            this->mousePosition = event->GetRelMousePosition();
        }
        // let the event unhandled, only keep track of the 2d mouse position
    }
    else
    {
        Message::Port::HandleMessage(msg);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
Server::Close()
{
    // cleanup the mappings
    while (this->mappings.Size() > 0)
    {
        this->mappings.Erase(0);
    }

    n_assert(this->isOpen);
    this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
    Set the current time stamp.
*/
void
Server::SetTime(double t)
{
    this->time = t;
}

//------------------------------------------------------------------------------
/**
*/
void
Server::CleanupEmptyMappings()
{
    if (!this->isInTrigger)
    {
        // remove empty elements
        for (int i = 0; i < this->mappings.Size(); /*empty*/)
        {
            if (this->mappings[i] == 0)
            {
                this->mappings.Erase(i);
            }
            else
            {
                // next
                i++;
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    The per frame trigger method of the input server.
*/
void
Server::Trigger()
{
    n_assert(this->isOpen);

    // try to cleanup empty mapping elements
    this->CleanupEmptyMappings();

    // lock mapping array
    this->isInTrigger = true;

    nInputServer* inputServer = nInputServer::Instance();
    nGuiServer* guiServer = nGuiServer::Instance();

    // trigger the input server
    inputServer->Trigger(this->time);

    // begin the frame for input sinks
    this->BeginInputFrame();
    // let the mappings consume the raw events
    this->Consume();
    // let the mappings emit input events
    this->Emit();
    // end the frame for input sinks
    this->EndInputFrame();

    // FIXME: trigger Nebula GUI server
    // THIS IS DIRTY, BUT CURRENTLY NECESSARY, BECAUSE THE InputServer FLUSHES
    // THE NEBULA2 INPUT EVENT AT THE END OF THE FRAME
    guiServer->Trigger();
    UI::Server::Instance()->Trigger();
#ifdef USE_MCEGUI
    CEUI::Server::Instance()->Trigger();
#endif
    // flush Nebula input events
    inputServer->FlushEvents();

    // unlock mapping array
    this->isInTrigger = false;
}

//------------------------------------------------------------------------------
/**
*/
void
Server::Consume()
{
    for (int i = 0; i < this->mappings.Size(); i++)
    {
        if (this->mappings[i] != 0)
        {
            this->mappings[i]->Consume();
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
Server::Emit()
{
    for (int i = 0; i < this->mappings.Size(); i++)
    {
        if (this->mappings[i] != 0)
        {
            this->mappings[i]->Emit();
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
Server::BeginInputFrame()
{
    for (int i = 0; i < this->mappings.Size(); i++)
    {
        if (this->mappings[i] != 0)
        {
            this->mappings[i]->BeginFrame();
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
Server::EndInputFrame()
{
    for (int i = 0; i < this->mappings.Size(); i++)
    {
        if (this->mappings[i] != 0)
        {
            this->mappings[i]->EndFrame();
        }
    }
}


//------------------------------------------------------------------------------
/**
    create a input source object that could handle the raw input events from
    the given device
*/
Ptr<Source>
Server::CreateInputSource(const nString& deviceChannel) const
{
    // separate device and channel
    nArray<nString> tokens;
    deviceChannel.Tokenize(":", tokens);
    n_assert(tokens.Size() == 2);

    // special case detection for the default mouse and the default keyboard,
    // the virtual devices that where used from the win proc
    bool isDefaultMouse = false;
    bool isDefaultKeyboard = false;

    // special case detection for the character input (raw characters for text input)
    bool isCharacterInput = false;

    // special case for the mouse move that is emitted from win proc without a channel
    bool isMousePosition = false;

    if (tokens[0] == "mouse0")
    {
        isDefaultMouse = true;

        if (tokens[1] == "position")
        {
            isMousePosition = true;
        }
    }
    else if (tokens[0] == "keyb0")
    {
        isDefaultKeyboard = true;

        if (tokens[1] == "characters")
        {
            isCharacterInput = true;
        }
    }

    Ptr<Source> source = 0;
    if (isDefaultKeyboard)
    {
        if (isCharacterInput)
        {
            // the default text/character input source (used to pipe the raw characters from win proc into text field)
            Ptr<KeySource> keySource = KeySource::Create();
            keySource->SetIsCharacterSource(true);
            keySource->SetDeviceId(N_INPUT_KEYBOARD(0));
            source = keySource;
        }
    }
    else if (isDefaultMouse)
    {
        if (isMousePosition)
        {
            // mouse move from win proc
            Ptr<MouseSource> mouseSource = MouseSource::Create();
            mouseSource->SetDeviceId(N_INPUT_MOUSE(0));
            source = mouseSource;
        }
    }

    if (source == 0)
    {
        // does this device channel exists?
        // (e.g. does the mouse have a wheel?)
        if (this->ExistsDeviceChannel(tokens[0], tokens[1]))
        {

            // handle the default input types
            switch ((nInputType) this->GetDeviceAttr(tokens[0], tokens[1], "type"))
            {
                case N_INPUT_AXIS_MOVE:
                {
                    // create axis input source
                    Ptr<AxisSource> axisSource = AxisSource::Create();
                    axisSource->SetAxis(this->GetDeviceAttr(tokens[0], tokens[1], "axis"));
                    source = axisSource;
                    break;
                }

                case N_INPUT_BUTTON_DOWN:
                case N_INPUT_BUTTON_UP:
                {
                    // create button source
                    Ptr<ButtonSource> buttonSource = ButtonSource::Create();
                    buttonSource->SetButtonId(this->GetDeviceAttr(tokens[0], tokens[1], "btn"));
                    source = buttonSource;
                    break;
                }

                case N_INPUT_KEY_DOWN:
                case N_INPUT_KEY_UP:
                {
                    // create key-button input
                    Ptr<KeySource> keySource = KeySource::Create();
                    keySource->SetKeyId(this->GetDeviceAttr(tokens[0], tokens[1], "key"));
                    source = keySource;
                    break;
                }

                default:
                    n_error("Unhandled nInputType of devicechannel '%s'!", deviceChannel.Get());
            }

            n_assert(source != 0);
            source->SetDeviceId(this->GetDeviceAttr(tokens[0], tokens[1], "devid"));
        }
    }

    return source;
}

//------------------------------------------------------------------------------
/**
*/
bool
Server::ExistsDeviceChannel(const nString& deviceName, const nString& channelName) const
{
    n_assert(channelName.IsValid());
    n_assert(deviceName.IsValid());

    nKernelServer* kernelServer = nKernelServer::Instance();

    // lookup the device from nebula
    nString deviceNodePath;
    deviceNodePath.Format("/sys/share/input/devs/%s", deviceName.Get());
    nRoot* deviceNode = kernelServer->Lookup(deviceNodePath.Get());
    if (deviceNode)
    {
        // lookup the channel type from nebula
        nString channelNodePath;
        channelNodePath.Format("channels/%s", channelName.Get());

        kernelServer->PushCwd(deviceNode);
        nEnv* channelNode = (nEnv *) kernelServer->Lookup(channelNodePath.Get());
        kernelServer->PopCwd();

        if (channelNode)
        {
            return true;
        }
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
int
Server::GetDeviceAttr(const nString& deviceName, const nString& channelName, const nString& attribute) const
{
    n_assert(channelName.IsValid());
    n_assert(deviceName.IsValid());
    n_assert(attribute.IsValid());

    nKernelServer* kernelServer = nKernelServer::Instance();

    // lookup the device from nebula
    nString deviceNodePath;
    deviceNodePath.Format("/sys/share/input/devs/%s", deviceName.Get());
    nRoot* deviceNode = kernelServer->Lookup(deviceNodePath.Get());
    n_assert(deviceNode);

    // lookup the channel type from nebula
    nString channelNodePath;
    channelNodePath.Format("channels/%s", channelName.Get());

    kernelServer->PushCwd(deviceNode);
    nEnv* channelNode = (nEnv *) kernelServer->Lookup(channelNodePath.Get());
    n_assert(channelNode);
    kernelServer->PopCwd();

    // get channel description
    nString channelDesciption = channelNode->GetS();
    n_assert(channelDesciption.IsValid());

    // tokenize attributes
    nArray<nString> attributes;
    channelDesciption.Tokenize(" ", attributes);

    // find attribute
    int value = -1;
    bool found = false;
    for (int i = 0; i < attributes.Size(); i++)
    {
        // extract key and value
        nArray<nString> attrTokens;
        attributes[i].Tokenize("=", attrTokens);
        n_assert(attrTokens.Size() == 2);

        if(attrTokens[0] == attribute)
        {
            value = attrTokens[1].AsInt();
            found = true;
        }
    }
    n_assert(found);

    return value;
}

//------------------------------------------------------------------------------
/**
*/
void
Server::AddMapping(const nString& mappingName, const nString& deviceChannel)
{
    n_assert(mappingName.IsValid());
    n_assert(deviceChannel.IsValid());
    n_assert(!this->HasMapping(mappingName));

    // create the mapping
    Ptr<Mapping> mapping = Mapping::Create();
    mapping->SetName(mappingName);

    // create a input source for this device channel
    Ptr<Source> source = this->CreateInputSource(deviceChannel);
    if (source != 0)
    {
        mapping->SetInputSource(source);
    }

    // append mapping
    this->mappings.Append(mapping);
}

//------------------------------------------------------------------------------
/**
*/
void
Server::RemMapping(const nString& mappingName)
{
    n_assert(mappingName.IsValid());
    n_assert(this->HasMapping(mappingName));

    // find mapping
    for (int i = 0; i < this->mappings.Size(); i++)
    {
        if (this->mappings[i] != 0 && this->mappings[i]->GetName() == mappingName)
        {
            // just set the ptr to 0, so the mapping could be removed in trigger
            this->mappings[i] = 0;
            break;
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
int
Server::GetNumMappings() const
{
    return this->mappings.Size();
}

//------------------------------------------------------------------------------
/**
*/
Mapping*
Server::GetMappingAt(int i)
{
    n_assert(i >= 0 && i < this->mappings.Size());
    return this->mappings[i];
}

//------------------------------------------------------------------------------
/**
*/
bool
Server::HasMapping(const nString& name) const
{
    n_assert(name.IsValid());
    for (int i = 0; i < this->mappings.Size(); i++)
    {
        if (this->mappings[i] != 0 && this->mappings[i]->GetName() == name)
        {
            return true;
        }
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
Mapping*
Server::GetMapping(const nString& name) const
{
    n_assert(name.IsValid());
    n_assert(this->HasMapping(name));

    for (int i = 0; i < this->mappings.Size(); i++)
    {
        if (this->mappings[i] != 0 && this->mappings[i]->GetName() == name)
        {
            return this->mappings[i];
        }
    }
    n_error("Server::GetMapping(): Did not found mapping '%s'!", name.Get());
    return 0; // silence the compiler
}

//------------------------------------------------------------------------------
/**
*/
Mapping*
Server::AttachInputSink(const nString& mappingName, int priority, Message::Port* sink)
{
    n_assert(mappingName.IsValid());
    n_assert(sink);
    n_assert(this->HasMapping(mappingName));

    Mapping* mapping = this->GetMapping(mappingName);
    n_assert(mapping);
    mapping->AttachInputSink(priority, sink);

    return mapping;
}

//------------------------------------------------------------------------------
/**
*/
void
Server::RemoveInputSink(const nString& mappingName, Message::Port* sink)
{
    n_assert(sink);
    n_assert(mappingName.IsValid());
    n_assert(this->HasMapping(mappingName));

    Mapping* mapping = this->GetMapping(mappingName);
    n_assert(mapping);
    mapping->RemoveInputSink(sink);
}

//------------------------------------------------------------------------------
/**
*/
void
Server::RemoveInputSink(const nString& mappingName, int priority, Message::Port* sink)
{
    n_assert(sink);
    n_assert(mappingName.IsValid());
    n_assert(this->HasMapping(mappingName));

    Mapping* mapping = this->GetMapping(mappingName);
    n_assert(mapping);
    mapping->RemoveInputSink(priority, sink);
}

//------------------------------------------------------------------------------
/**
*/
float
Server::GetSlider(const nString& mappingName) const
{
    n_assert(mappingName.IsValid());
    n_assert(this->isOpen);

    if (this->HasMapping(mappingName))
    {
        Mapping* mapping = this->GetMapping(mappingName);
        n_assert(mapping);
        if (mapping->HasInputSource())
        {
            Source* source = mapping->GetInputSource();
            n_assert(source);
            return source->GetAxisValue();
        }
        // there is no input source attached return default 0.f slider value
        return 0.f;
    }

    // TODO: remove this FALLBACK: use the old nebula system
    n_message("Server::GetSlider: fallback to nebula input for mapping '%s'!", mappingName.Get());
    return nInputServer::Instance()->GetSlider(mappingName.Get());
}

//------------------------------------------------------------------------------
/**
*/
bool
Server::GetButtonUp(const nString& mappingName) const
{
    n_assert(mappingName.IsValid());
    n_assert(this->isOpen);

    if (this->HasMapping(mappingName))
    {
        Mapping* mapping = this->GetMapping(mappingName);
        n_assert(mapping);
        if (mapping->HasInputSource())
        {
            Source* source = mapping->GetInputSource();
            n_assert(source);
            return source->GetButtonUp();
        }
        // there is no input source attached return default button value
        return false;
    }
    // there is no mapping
    n_error("Server::GetButtonUp: mapping '%s' does not exist!", mappingName.Get());
    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool
Server::GetButtonDown(const nString& mappingName) const
{
    n_assert(mappingName.IsValid());
    n_assert(this->isOpen);

    if (this->HasMapping(mappingName))
    {
        Mapping* mapping = this->GetMapping(mappingName);
        n_assert(mapping);
        if (mapping->HasInputSource())
        {
            Source* source = mapping->GetInputSource();
            n_assert(source);
            return source->GetButtonDown();
        }
        // there is no input source attached return default button value
        return false;
    }
    // there is no mapping
    n_error("Server::GetButtonDown: mapping '%s' does not exist!", mappingName.Get());
    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool
Server::GetButtonPressed(const nString& mappingName) const
{
    n_assert(mappingName.IsValid());
    n_assert(this->isOpen);

    if (this->HasMapping(mappingName))
    {
        Mapping* mapping = this->GetMapping(mappingName);
        n_assert(mapping);
        if (mapping->HasInputSource())
        {
            Source* source = mapping->GetInputSource();
            n_assert(source);
            return source->GetButtonPressed();
        }
        // there is no input source attached return default button value
        return false;
    }
    // there is no mapping
    n_error("Server::GetButtonPressed: mapping '%s' does not exist!", mappingName.Get());
    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool
Server::GetButtonClicked(const nString& mappingName) const
{
    n_assert(mappingName.IsValid());
    n_assert(this->isOpen);

    if (this->HasMapping(mappingName))
    {
        Mapping* mapping = this->GetMapping(mappingName);
        n_assert(mapping);
        if (mapping->HasInputSource())
        {
            Source* source = mapping->GetInputSource();
            n_assert(source);
            return source->GetButtonClicked();
        }
        // there is no input source attached return default button value
        return false;
    }
    // there is no mapping
    n_error("Server::GetButtonClicked: mapping '%s' does not exist!", mappingName.Get());
    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool
Server::GetButtonDoubleClicked(const nString& mappingName) const
{
    n_assert(mappingName.IsValid());
    n_assert(this->isOpen);

    if (this->HasMapping(mappingName))
    {
        Mapping* mapping = this->GetMapping(mappingName);
        n_assert(mapping);
        if (mapping->HasInputSource())
        {
            Source* source = mapping->GetInputSource();
            n_assert(source);
            return source->GetButtonDoubleClicked();
        }
        // there is no input source attached return default button value
    }
    // there is no mapping
    n_error("Server::GetButtonDoubleClicked: mapping '%s' does not exist!", mappingName.Get());
    return false;
}

//------------------------------------------------------------------------------
/**
*/
const vector2&
Server::GetMousePos() const
{
    return this->mousePosition;
}

} // namespace Graphics
