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

    void setLED(unsigned int gpu, int color);

    unsigned int getGpuClock(unsigned int gpu);

    int setMemClock(unsigned int clock);



private:

    typedef void *(*NvAPI_QueryInterface_t)(unsigned int offset);
    typedef NvAPI_Status (*NvAPI_Initialize_t)();
    typedef NvAPI_Status (*NvAPI_Unload_t)();
    typedef NvAPI_Status (*NvAPI_EnumPhysicalGPUs_t)(NvPhysicalGpuHandle[NVAPI_MAX_PHYSICAL_GPUS], NvU32*);
    typedef NvAPI_Status (*NvAPI_GPU_GetSystemType_t)(NvPhysicalGpuHandle hPhysicalGpu, NV_SYSTEM_TYPE *pSystemType);
    typedef NvAPI_Status (*NvAPI_GPU_GetFullName_t)(NvPhysicalGpuHandle hPhysicalGpu, NvAPI_ShortString szName);
    typedef NvAPI_Status (*NvAPI_GPU_GetPhysicalFrameBufferSize_t)(NvPhysicalGpuHandle hPhysicalGpu, NvU32 *pSize);
    typedef NvAPI_Status (*NvAPI_GPU_GetRamType_t)(NvPhysicalGpuHandle hPhysicalGpu, int *memtype);
    typedef NvAPI_Status (*NvAPI_GPU_GetVbiosVersionString_t)(NvPhysicalGpuHandle hPhysicalGpu, NvAPI_ShortString biosname);
    typedef NvAPI_Status (*NvAPI_GPU_GetAllClockFrequencies_t)(NvPhysicalGpuHandle hPhysicalGpu, NV_GPU_CLOCK_FREQUENCIES *pstates_info);
    typedef NvAPI_Status (*NvAPI_GPU_GetPstates20_t)( NvPhysicalGpuHandle hPhysicalGpu,  NV_GPU_PERF_PSTATES20_INFO *pPstatesInfo);
    typedef NvAPI_Status (*NvAPI_GPU_SetPstates20_t)(NvPhysicalGpuHandle hPhysicalGpu, NV_GPU_PERF_PSTATES20_INFO_V1 *pstates_info);
    typedef NvAPI_Status (*NvAPI_GPU_GetPstatesInfoEx_t)(NvPhysicalGpuHandle hPhysicalGpu, NV_GPU_PERF_PSTATES_INFO *pPerfPstatesInfo, NvU32 inputFlags);
    typedef NvAPI_Status (*NvAPI_GPU_GetIllumination_t)(NV_GPU_GET_ILLUMINATION_PARM *pIlluminationInfo);
    typedef NvAPI_Status (*NvAPI_GPU_SetIllumination_t)(NV_GPU_SET_ILLUMINATION_PARM *pIlluminationInfo);
    typedef NvAPI_Status (*NvAPI_GPU_QueryIlluminationSupport_t)(NV_GPU_QUERY_ILLUMINATION_SUPPORT_PARM *pIlluminationSupportInfo);




    NvAPI_QueryInterface_t NvQueryInterface;
    NvAPI_Initialize_t NvInit;
    NvAPI_Unload_t NvUnload;
    NvAPI_EnumPhysicalGPUs_t NvEnumGPUs;
    NvAPI_GPU_GetSystemType_t NvGetSysType;
    NvAPI_GPU_GetFullName_t NvGetName;
    NvAPI_GPU_GetPhysicalFrameBufferSize_t NvGetMemSize;
    NvAPI_GPU_GetRamType_t NvGetMemType;
    NvAPI_GPU_GetVbiosVersionString_t NvGetBiosName;
    NvAPI_GPU_GetAllClockFrequencies_t NvGetFreq;
    NvAPI_GPU_GetPstates20_t NvGetPstates;
    NvAPI_GPU_SetPstates20_t NvSetPstates;
    NvAPI_GPU_GetPstatesInfoEx_t NvGetPStatesInfoEx;
    NvAPI_GPU_GetIllumination_t NvGetIllumination;
    NvAPI_GPU_SetIllumination_t NvSetIllumination;
    NvAPI_GPU_QueryIlluminationSupport_t NvQueryIlluminationSupport;


    NvU32 _gpuCount;

    NvPhysicalGpuHandle _gpuHandles[NVAPI_MAX_PHYSICAL_GPUS];

    bool _libLoaded;


};

#endif // NVIDIAAPI_H
