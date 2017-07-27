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
  /*  typedef NvAPI_Status (*NvAPI_DLL_PerfPoliciesGetInfo_t)(NvPhysicalGpuHandle hPhysicalGpu, NVAPI_GPU_PERF_INFO* pInfo);
    typedef NvAPI_Status (*NvAPI_DLL_PerfPoliciesGetStatus_t)(NvPhysicalGpuHandle hPhysicalGpu, NVAPI_GPU_PERF_STATUS* pStatus);
    typedef NvAPI_Status (*NvAPI_DLL_ClientPowerPoliciesGetInfo_t)(NvPhysicalGpuHandle hPhysicalGpu, NVAPI_GPU_POWER_INFO* pInfo);
    typedef NvAPI_Status (*NvAPI_DLL_ClientPowerPoliciesGetStatus_t)(NvPhysicalGpuHandle hPhysicalGpu, NVAPI_GPU_POWER_STATUS* pPolicies);
    typedef NvAPI_Status (*NvAPI_DLL_ClientPowerPoliciesSetStatus_t)(NvPhysicalGpuHandle hPhysicalGpu, NVAPI_GPU_POWER_STATUS* pPolicies);
    typedef NvAPI_Status (*NvAPI_DLL_ClientPowerTopologyGetStatus_t)(NvPhysicalGpuHandle hPhysicalGpu, NVAPI_GPU_POWER_TOPO* topo);
    typedef NvAPI_Status (*NvAPI_DLL_ClientThermalPoliciesGetInfo_t)(NvPhysicalGpuHandle hPhysicalGpu, NV_GPU_THERMAL_SETTINGS* pInfo);
    typedef NvAPI_Status (*NvAPI_DLL_ClientThermalPoliciesGetLimit_t)(NvPhysicalGpuHandle hPhysicalGpu, NVAPI_GPU_THERMAL_LIMIT* pLimit);
    typedef NvAPI_Status (*NvAPI_DLL_ClientThermalPoliciesSetLimit_t)(NvPhysicalGpuHandle hPhysicalGpu, NVAPI_GPU_THERMAL_LIMIT* pLimit);
  */

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
   /* NvAPI_DLL_PerfPoliciesGetInfo_t NvAPI_DLL_PerfPoliciesGetInfo;
    NvAPI_DLL_PerfPoliciesGetStatus_t NvAPI_DLL_PerfPoliciesGetStatus;
    NvAPI_DLL_ClientPowerPoliciesGetInfo_t NvAPI_DLL_ClientPowerPoliciesGetInfo;
    NvAPI_DLL_ClientPowerPoliciesGetStatus_t NvAPI_DLL_ClientPowerPoliciesGetStatus;
    NvAPI_DLL_ClientPowerPoliciesSetStatus_t NvAPI_DLL_ClientPowerPoliciesSetStatus;
    NvAPI_DLL_ClientPowerTopologyGetStatus_t NvAPI_DLL_ClientPowerTopologyGetStatus;
    NvAPI_DLL_ClientThermalPoliciesGetInfo_t NvAPI_DLL_ClientThermalPoliciesGetInfo;
    NvAPI_DLL_ClientThermalPoliciesGetLimit_t NvAPI_DLL_ClientThermalPoliciesGetLimit;
    NvAPI_DLL_ClientThermalPoliciesSetLimit_t NvAPI_DLL_ClientThermalPoliciesSetLimit;
    */










    NvU32 _gpuCount;

    NvPhysicalGpuHandle _gpuHandles[NVAPI_MAX_PHYSICAL_GPUS];

    bool _libLoaded;


};

#endif // NVIDIAAPI_H
