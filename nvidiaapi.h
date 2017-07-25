#ifndef NVIDIAAPI_H
#define NVIDIAAPI_H

#include <QMutex>
#include <QLibrary>
#include <QByteArray>

#include "nvapi.h"


#define NV_MAX_GPU 64



class nvidiaAPI : QLibrary
{
public:
    nvidiaAPI();
    ~nvidiaAPI();

    void overClock(unsigned int gpu, unsigned int mem);

    void setLED();


    int setMemClock(unsigned int clock);


private:

    typedef NvAPI_Status (__cdecl * NVAPI_QUERYINTERFACE)                   (unsigned int);

    NVAPI_QUERYINTERFACE                   nvapi_QueryInterface;


    typedef NvAPI_Status (__cdecl *NvAPI_DLL_SetPstates20_t)(NvPhysicalGpuHandle handle, NV_GPU_PERF_PSTATES20_INFO_V1 *pSet);
    NvAPI_DLL_SetPstates20_t NvAPI_DLL_SetPstates20;


    NvPhysicalGpuHandle _gpuHandles[NVAPI_MAX_PHYSICAL_GPUS];

    NvU32 _gpuCount;


    bool _libLoaded;


};

#endif // NVIDIAAPI_H
