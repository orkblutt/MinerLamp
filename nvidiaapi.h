#ifndef NVIDIAAPI_H
#define NVIDIAAPI_H

#include <QMutex>
#include <QLibrary>
#include <QByteArray>

typedef unsigned long NvU32;

#define NV_MAX_GPU 64


typedef struct {
    NvU32   version;
    NvU32   ClockType:2;
    NvU32   reserved:22;
    NvU32   reserved1:8;
    struct {
        NvU32   bIsPresent:1;
        NvU32   reserved:31;
        NvU32   frequency;
    }domain[32];
} NV_GPU_CLOCK_FREQUENCIES_V2;

typedef struct {
    int value;
    struct {
        int   mindelta;
        int   maxdelta;
    } valueRange;
} NV_GPU_PERF_PSTATES20_PARAM_DELTA;

typedef struct {
    NvU32   domainId;
    NvU32   typeId;
    NvU32   bIsEditable:1;
    NvU32   reserved:31;
    NV_GPU_PERF_PSTATES20_PARAM_DELTA   freqDelta_kHz;
    union {
        struct {
            NvU32   freq_kHz;
        } single;
        struct {
            NvU32   minFreq_kHz;
            NvU32   maxFreq_kHz;
            NvU32   domainId;
            NvU32   minVoltage_uV;
            NvU32   maxVoltage_uV;
        } range;
    } data;
} NV_GPU_PSTATE20_CLOCK_ENTRY_V1;

typedef struct {
    NvU32   domainId;
    NvU32   bIsEditable:1;
    NvU32   reserved:31;
    NvU32   volt_uV;
    int     voltDelta_uV;
} NV_GPU_PSTATE20_BASE_VOLTAGE_ENTRY_V1;

typedef struct {
    NvU32   version;
    NvU32   bIsEditable:1;
    NvU32   reserved:31;
    NvU32   numPstates;
    NvU32   numClocks;
    NvU32   numBaseVoltages;
    struct {
        NvU32                                   pstateId;
        NvU32                                   bIsEditable:1;
        NvU32                                   reserved:31;
        NV_GPU_PSTATE20_CLOCK_ENTRY_V1          clocks[8];
        NV_GPU_PSTATE20_BASE_VOLTAGE_ENTRY_V1   baseVoltages[4];
    } pstates[16];
} NV_GPU_PERF_PSTATES20_INFO_V1;


class nvidiaAPI
{
public:
    nvidiaAPI();
    ~nvidiaAPI();

    void overClock(unsigned int gpu, unsigned int mem);


private:
    typedef void *(*NvAPI_QueryInterface_t)(unsigned int offset);
    typedef int (*NvAPI_Initialize_t)();
    typedef int (*NvAPI_Unload_t)();
    typedef int (*NvAPI_EnumPhysicalGPUs_t)(int **handles, int *count);
    typedef int (*NvAPI_GPU_GetSystemType_t)(int *handle, int *systype);
    typedef int (*NvAPI_GPU_GetFullName_t)(int *handle, char *sysname);
    typedef int (*NvAPI_GPU_GetPhysicalFrameBufferSize_t)(int *handle, int *memsize);
    typedef int (*NvAPI_GPU_GetRamType_t)(int *handle, int *memtype);
    typedef int (*NvAPI_GPU_GetVbiosVersionString_t)(int *handle, char *biosname);
    typedef int (*NvAPI_GPU_GetAllClockFrequencies_t)(int *handle, NV_GPU_PERF_PSTATES20_INFO_V1 *pstates_info);
    typedef int (*NvAPI_GPU_GetPstates20_t)(int *handle, NV_GPU_PERF_PSTATES20_INFO_V1 *pstates_info);
    typedef int (*NvAPI_GPU_SetPstates20_t)(int *handle, int *pstates_info);

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


    int* _gpuHandles[NV_MAX_GPU];


};

#endif // NVIDIAAPI_H
