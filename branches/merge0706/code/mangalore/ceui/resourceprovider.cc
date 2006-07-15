#include "ceui/resourceprovider.h"
#include "kernel/nfile.h"
#include "kernel/nfileserver2.h"

namespace CEUI
{

//------------------------------------------------------------------------------
/**
*/
ResourceProvider::ResourceProvider() {
}

//------------------------------------------------------------------------------
/**
*/
ResourceProvider::~ResourceProvider() {
}

//------------------------------------------------------------------------------
/**
*/
void ResourceProvider::loadRawDataContainer(const CEGUI::String& fileName, CEGUI::RawDataContainer& output, const CEGUI::String& resGroup) {
    nFile* file = nFileServer2::Instance()->NewFileObject();
    n_verify(file->Open(fileName.c_str(), "r"));
    int size = file->GetSize();
    uchar* data = n_new_array(uchar, size);
    file->Read(data, size);
    file->Close();
    file->Release();
    output.setSize(size);
    output.setData(data);
}

//------------------------------------------------------------------------------
/**
*/
void ResourceProvider::unloadRawDataContainer(CEGUI::RawDataContainer& data) {
    n_delete_array(data.getDataPtr());
    data.setData(0);
    data.setSize(0);
}

} // namespace CEUI
