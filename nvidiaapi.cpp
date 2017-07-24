#include "nvidiaapi.h"
#include <QDebug>
nvidiaAPI::nvidiaAPI()
{

    for(int i = 0; i < NV_MAX_GPU; i++) _gpuHandles[i] = 0;

    QLibrary nvapilib("nvapi64");
    nvapilib.load();

    NvQueryInterface = (NvAPI_QueryInterface_t)nvapilib.resolve("nvapi_QueryInterface");

    qDebug() << NvQueryInterface;

    if(NvQueryInterface != Q_NULLPTR)
    {
        NvInit          = (NvAPI_Initialize_t)NvQueryInterface(0x0150E828);
        NvUnload        = (NvAPI_Unload_t)NvQueryInterface(0xD22BDD7E);
        NvEnumGPUs      = (NvAPI_EnumPhysicalGPUs_t)NvQueryInterface(0xE5AC921F);
        NvGetSysType    = (NvAPI_GPU_GetSystemType_t)NvQueryInterface(0xBAAABFCC);
        NvGetName       = (NvAPI_GPU_GetFullName_t)NvQueryInterface(0xCEEE8E9F);
        NvGetMemSize    = (NvAPI_GPU_GetPhysicalFrameBufferSize_t)NvQueryInterface(0x46FBEB03);
        NvGetMemType    = (NvAPI_GPU_GetRamType_t)NvQueryInterface(0x57F7CAAC);
        NvGetBiosName   = (NvAPI_GPU_GetVbiosVersionString_t)NvQueryInterface(0xA561FD7D);
        NvGetFreq       = (NvAPI_GPU_GetAllClockFrequencies_t)NvQueryInterface(0xDCB616C3);
        NvGetPstates    = (NvAPI_GPU_GetPstates20_t)NvQueryInterface(0x6FF81213);
        NvSetPstates    = (NvAPI_GPU_SetPstates20_t)NvQueryInterface(0x0F4DAE6B);
    }

    NvInit();



}

nvidiaAPI::~nvidiaAPI()
{
    NvUnload();
}



void nvidiaAPI::overClock(unsigned int gpu, unsigned int mem)
{
    int nGPU=0, userfreq = 100000, systype=0, memsize=0, memtype=0;
    int *hdlGPU[64]={0}, *buf=0;
    char sysname[64]={0}, biosname[64]={0};
    NV_GPU_PERF_PSTATES20_INFO_V1 pstates_info;
    pstates_info.version = 0x11c94;


    NvEnumGPUs(hdlGPU, &nGPU);
    NvGetSysType(hdlGPU[0], &systype);
    NvGetName(hdlGPU[0], sysname);
    NvGetMemSize(hdlGPU[0], &memsize);
    NvGetMemType(hdlGPU[0], &memtype);
    NvGetBiosName(hdlGPU[0], biosname);
    NvGetPstates(hdlGPU[0], &pstates_info);

    qDebug("Name: %s", sysname);
    qDebug("VRAM: %dMB GDDR%d", memsize/1024, memtype<=7?3:5);
    qDebug("BIOS: %s", biosname);
    qDebug("GPU: %dMHz", (int)((pstates_info.pstates[0].clocks[0]).data.range.maxFreq_kHz)/1000);
    qDebug("RAM: %dMHz", (int)((pstates_info.pstates[0].clocks[1]).data.single.freq_kHz)/1000);
    qDebug("Current GPU OC: %dMHz", (int)((pstates_info.pstates[0].clocks[0]).freqDelta_kHz.value)/1000);
    qDebug("Current RAM OC: %dMHz", (int)((pstates_info.pstates[0].clocks[1]).freqDelta_kHz.value)/1000);

    buf = (int*)malloc(0x1c94);
    memset(buf, 0, 0x1c94);
    buf[0] = 0x11c94; buf[2] = 1; buf[3] = 1;
    buf[7] = 4; buf[10] = memtype<=7?userfreq:userfreq*2;
    qDebug() << buf[10];
    NvSetPstates(hdlGPU[0], buf)? qDebug("VRAM OC failed!\n") : qDebug("RAM OC OK: %d MHz\n", userfreq/1000);
    free(buf);
}


