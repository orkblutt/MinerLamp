#include "nvidiaapi.h"

nvidiaAPI::nvidiaAPI()
{
    NvQueryInterface = (void*)GetProcAddress(LoadLibrary("nvapi.dll"), "nvapi_QueryInterface");
        NvInit          = NvQueryInterface(0x0150E828);
        NvUnload        = NvQueryInterface(0xD22BDD7E);
        NvEnumGPUs      = NvQueryInterface(0xE5AC921F);
        NvGetSysType    = NvQueryInterface(0xBAAABFCC);
        NvGetName       = NvQueryInterface(0xCEEE8E9F);
        NvGetMemSize    = NvQueryInterface(0x46FBEB03);
        NvGetMemType    = NvQueryInterface(0x57F7CAAC);
        NvGetBiosName   = NvQueryInterface(0xA561FD7D);
        NvGetFreq       = NvQueryInterface(0xDCB616C3);
        NvGetPstates    = NvQueryInterface(0x6FF81213);
        NvSetPstates    = NvQueryInterface(0x0F4DAE6B);

}

void nvidiaAPI::initAPI()
{



}
