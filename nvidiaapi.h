#ifndef NVIDIAAPI_H
#define NVIDIAAPI_H

#include <QMutex>
#include <QLibrary>
#include <QByteArray>

#include "nvapi.h"



typedef struct {
    NvU32 version;
    NvU32 flags;
    struct
    {
        NvU32 pstate; // Assumption
        NvU32 unknown1[2];
        NvU32 min_power;
        NvU32 unknown2[2];
        NvU32 def_power;
        NvU32 unknown3[2];
        NvU32 max_power;
        NvU32 unknown4; // 0
    } entries[4];
} NVAPI_GPU_POWER_INFO;
#define NVAPI_GPU_POWER_INFO_VER MAKE_NVAPI_VERSION(NVAPI_GPU_POWER_INFO, 1)

typedef struct {
    NvU32 version;
    NvU32 flags;
    struct {
        NvU32 unknown1;
        NvU32 unknown2;
        NvU32 power; // percent * 1000
        NvU32 unknown4;
    } entries[4];
} NVAPI_GPU_POWER_STATUS;
#define NVAPI_GPU_POWER_STATUS_VER MAKE_NVAPI_VERSION(NVAPI_GPU_POWER_STATUS, 1)

typedef struct {
    NvU32 version;
    NvU32 count;
    struct {
        NvU32 unknown1;
        NvU32 unknown2;
        NvU32 power; // unsure ?? 85536 to 95055 on 1080, 104825+ on 970
        NvU32 unknown4;
    } entries[4];
} NVAPI_GPU_POWER_TOPO;
#define NVAPI_GPU_POWER_TOPO_VER MAKE_NVAPI_VERSION(NVAPI_GPU_POWER_TOPO, 1)

typedef struct {
    NvU32 version;
    NvU32 flags;
    struct {
        NvU32 controller;
        NvU32 unknown;
        NvS32 min_temp;
        NvS32 def_temp;
        NvS32 max_temp;
        NvU32 defaultFlags;
    } entries[4];
} NVAPI_GPU_THERMAL_INFO;
#define NVAPI_GPU_THERMAL_INFO_VER MAKE_NVAPI_VERSION(NVAPI_GPU_THERMAL_INFO, 2)

typedef struct {
    NvU32 version;
    NvU32 flags;
    struct {
        NvU32 controller;
        NvU32 value;
        NvU32 flags;
    } entries[4];
} NVAPI_GPU_THERMAL_LIMIT;
#define NVAPI_GPU_THERMAL_LIMIT_VER MAKE_NVAPI_VERSION(NVAPI_GPU_THERMAL_LIMIT, 2)

// Maxwell gpu core voltage reading
typedef struct {
    NvU32 version;
    NvU32 flags;
    NvU32 count; // unsure
    NvU32 unknown;
    NvU32 value_uV;
    NvU32 buf1[30];
} NVAPI_VOLT_STATUS; // 140 bytes (1-008c)
#define NVAPI_VOLT_STATUS_VER MAKE_NVAPI_VERSION(NVAPI_VOLT_STATUS, 1)

// Pascal gpu core voltage reading
typedef struct {
    NvU32 version;
    NvU32 flags;
    NvU32 nul[8];
    NvU32 value_uV;
    NvU32 buf1[8];
} NVAPI_VOLTAGE_STATUS; // 76 bytes (1-004c)
#define NVAPI_VOLTAGE_STATUS_VER MAKE_NVAPI_VERSION(NVAPI_VOLTAGE_STATUS, 1)

typedef struct {
    NvU32 version;
    NvU32 numClocks; // unsure
    NvU32 nul[8];
    struct {
        NvU32 a;
        NvU32 clockType;
        NvU32 c;
        NvU32 d;
        NvU32 e;
        NvU32 f;
        NvU32 g;
        NvU32 h;
        NvU32 i;
        NvU32 j;
        NvS32 rangeMax;
        NvS32 rangeMin;
        NvS32 tempMax; // ? unsure
        NvU32 n;
        NvU32 o;
        NvU32 p;
        NvU32 q;
        NvU32 r;
    } entries[32]; // NVAPI_MAX_GPU_CLOCKS ?
} NVAPI_CLOCKS_RANGE; // 2344 bytes
#define NVAPI_CLOCKS_RANGE_VER MAKE_NVAPI_VERSION(NVAPI_CLOCKS_RANGE, 1)

// seems to return a clock table mask
typedef struct {
    NvU32 version;
    NvU32 mask[4]; // 80 bits mask
    NvU32 buf0[8];
    struct {
        NvU32 a;
        NvU32 b;
        NvU32 c;
        NvU32 d;
        NvU32 memDelta; // 1 for mem
        NvU32 gpuDelta; // 1 for gpu
    } clocks[80 + 23];
    NvU32 buf1[916];
} NVAPI_CLOCK_MASKS; // 6188 bytes
#define NVAPI_CLOCK_MASKS_VER MAKE_NVAPI_VERSION(NVAPI_CLOCK_MASKS, 1)

// contains the gpu/mem clocks deltas
typedef struct {
    NvU32 version;
    NvU32 mask[4]; // 80 bits mask (could be 8x 32bits)
    NvU32 buf0[12];
    struct {
        NvU32 a;
        NvU32 b;
        NvU32 c;
        NvU32 d;
        NvU32 e;
        NvS32 freqDelta; // 84000 = +84MHz
        NvU32 g;
        NvU32 h;
        NvU32 i;
    } gpuDeltas[80];
    NvU32 memFilled[23]; // maybe only 4 max
    NvS32 memDeltas[23];
    NvU32 buf1[1529];
} NVAPI_CLOCK_TABLE; // 9248 bytes
#define NVAPI_CLOCK_TABLE_VER MAKE_NVAPI_VERSION(NVAPI_CLOCK_TABLE, 1)

typedef struct {
    NvU32 version;
    NvU32 mask[4]; // 80 bits mask
    NvU32 buf0[12];
    struct {
        NvU32 a; // 0
        NvU32 freq_kHz;
        NvU32 volt_uV;
        NvU32 d;
        NvU32 e;
        NvU32 f;
        NvU32 g;
    } gpuEntries[80];
    struct {
        NvU32 a;  // 1 for idle values ?
        NvU32 freq_kHz;
        NvU32 volt_uV;
        NvU32 d;
        NvU32 e;
        NvU32 f;
        NvU32 g;
    } memEntries[23];
    NvU32 buf1[1064];
} NVAPI_VFP_CURVE; // 7208 bytes (1-1c28)
#define NVAPI_VFP_CURVE_VER MAKE_NVAPI_VERSION(NVAPI_VFP_CURVE, 1)

typedef struct {
    NvU32 version;
    NvS32 percent;
    NvU32 pad[8];
} NVAPI_VOLTBOOST_PERCENT; // 40 bytes (1-0028)
#define NVAPI_VOLTBOOST_PERCENT_VER MAKE_NVAPI_VERSION(NVAPI_VOLTBOOST_PERCENT, 1)

typedef struct {
    NvU32 version;
    NvU32 flags;
    NvU32 filled; // 1
    struct {
        NvU32 volt_uV;
        NvU32 unknown;
    } entries[128];
    // some empty tables then...
    NvU32 buf1[3888];
} NVAPI_VOLTAGES_TABLE; // 16588 bytes (1-40cc)
#define NVAPI_VOLTAGES_TABLE_VER MAKE_NVAPI_VERSION(NVAPI_VOLTAGES_TABLE, 1)

typedef struct {
    NvU32 version;
    NvU32 val1; // 7
    NvU32 val2; // 0x3F (63.)
    NvU32 pad[16];
} NVAPI_GPU_PERF_INFO; // 76 bytes (1-004c)
#define NVAPI_GPU_PERF_INFO_VER MAKE_NVAPI_VERSION(NVAPI_GPU_PERF_INFO, 1)

typedef struct {
    NvU32 version;
    NvU32 flags;     // 0
    NvU64 timeRef;   // increment with time
    NvU64 val1;      // seen 1 4 5 while mining, 16 else
    NvU64 val2;      // seen 7 and 3
    NvU64 values[3]; // increment with time
    NvU32 pad[326];  // empty
} NVAPI_GPU_PERF_STATUS; // 1360 bytes (1-0550)
#define NVAPI_GPU_PERF_STATUS_VER MAKE_NVAPI_VERSION(NVAPI_GPU_PERF_STATUS, 1)

typedef struct {
    NvU32 version;
    NvU32 val1;      // 4
    NvU32 val2;      // 2 or 0
    NvU32 val3;      // 2
    NvU32 val4;      // 3
    NV_GPU_PERF_PSTATE_ID pStateId;
    NvU32 val6;      // 0 or 2
    NvU32 val7;      // 4
    NvU32 val8;      // 0
    NvU32 memFreq1;  // 405000.
    NvU32 memFreq2;  // 405000.
    NvU32 memFreqMin;// 101250.
    NvU32 memFreqMax;// 486000.
    NvU32 zeros[3];
    NvU32 gpuFreq1;  // 696000. Unsure about those
    NvU32 gpuFreq2;  // 696000.
    NvU32 gpuFreqMin;// 174000.
    NvU32 gpuFreqMax;// 658000.
    NvU32 pad[2697];
} NVAPI_GPU_PERF_CLOCKS; // 10868 bytes (2-2a74)
#define NVAPI_GPU_PERF_CLOCKS_VER MAKE_NVAPI_VERSION(NVAPI_GPU_PERF_CLOCKS, 2)

typedef struct {
    NvU32 version;
    NvU32 level;
    NvU32 count;
    NvU32 pad[339]; // (4-0558)
} NVAPI_COOLER_SETTINGS;
#define NVAPI_COOLER_SETTINGS_VER MAKE_NVAPI_VERSION(NVAPI_COOLER_SETTINGS, 4)

typedef struct {
    NvU32 version;
    NvU32 level;   // 0 = auto ?
    NvU32 count;   // 1
    NvU32 pad[38]; // (1-00a4)
} NVAPI_COOLER_LEVEL;
#define NVAPI_COOLER_LEVEL_VER MAKE_NVAPI_VERSION(NVAPI_COOLER_LEVEL, 1)

#define NVAPI_MAX_COOLERS_PER_GPU   20


typedef struct {
    NvU32 version;
    NvU32 count;
    struct {
        NvS32 type;
        NvS32 controller;
        NvS32 defaultMin;
        NvS32 defaultMax;
        NvS32 currentMin;
        NvS32 currentMax;
        NvS32 currentLevel;
        NvS32 defaultPolicy;
        NvS32 currentPolicy;
        NvS32 target;
        NvS32 controlType;
        NvS32 active;
    } cooler[NVAPI_MAX_COOLERS_PER_GPU];
} NV_GPU_COOLER_SETTINGS_V2;
typedef NV_GPU_COOLER_SETTINGS_V2   NV_GPU_COOLER_SETTINGS;

#define NV_GPU_COOLER_SETTINGS_VER_2    MAKE_NVAPI_VERSION(NV_GPU_COOLER_SETTINGS_V2,2)
#define NV_GPU_COOLER_SETTINGS_VER      NV_GPU_COOLER_SETTINGS_VER_2


typedef struct {
    NvU32 version;
    struct {
        NvS32 level;
        NvS32 policy;
    } cooler[NVAPI_MAX_COOLERS_PER_GPU];
} NV_GPU_COOLER_LEVELS_V1;
typedef NV_GPU_COOLER_LEVELS_V1 NV_GPU_COOLER_LEVELS;


#define NV_GPU_COOLER_LEVELS_VER_1  MAKE_NVAPI_VERSION(NV_GPU_COOLER_LEVELS_V1,1)
#define NV_GPU_COOLER_LEVELS_VER    NV_GPU_COOLER_LEVELS_VER_1



class nvidiaAPI : public QLibrary
{
public:
    nvidiaAPI();
    ~nvidiaAPI();

    unsigned int getGPUCount();

    void setLED(unsigned int gpu, int color);

    int getGPUOffset(unsigned int gpu);
    int getMemOffset(unsigned int gpu);

    unsigned int getGpuClock(unsigned int gpu);
    unsigned int getPowerLimit(unsigned int gpu);
    unsigned int getFanSpeed(unsigned int gpu);


    int setMemClockOffset(unsigned int gpu,  int clock);
    int setGPUOffset(unsigned int gpu, int offset);
    int setPowerLimitPercent(unsigned int gpu, unsigned int percent);
    int setTempLimitOffset(unsigned int gpu, unsigned int offset);
    int setFanSpeed(unsigned int gpu, unsigned int percent);

    void setAllLED(int color);


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
    typedef NvAPI_Status (*NvAPI_DLL_ClientPowerPoliciesGetStatus_t)(NvPhysicalGpuHandle hPhysicalGpu, NVAPI_GPU_POWER_STATUS* pPolicies);
    typedef NvAPI_Status (*NvAPI_DLL_ClientPowerPoliciesGetInfo_t)(NvPhysicalGpuHandle hPhysicalGpu, NVAPI_GPU_POWER_INFO* pInfo);
    typedef NvAPI_Status (*NvAPI_DLL_ClientPowerPoliciesSetStatus_t)(NvPhysicalGpuHandle handle, NVAPI_GPU_POWER_STATUS* pPolicies);
    typedef NvAPI_Status (*NvAPI_GPU_GetCoolersSettings_t)(NvPhysicalGpuHandle hPhysicalGpu, NvU32 coolerIndex, NV_GPU_COOLER_SETTINGS* coolerSettings);
    typedef NvAPI_Status (*NvAPI_GPU_SetCoolerLevel_t)(NvPhysicalGpuHandle hPhysicalGpu, NvU32 coolerIndex, NV_GPU_COOLER_LEVELS* coolerLevel);

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
    NvAPI_DLL_ClientPowerPoliciesGetStatus_t NvClientPowerPoliciesGetStatus;
    NvAPI_DLL_ClientPowerPoliciesGetInfo_t NvClientPowerPoliciesGetInfo;
    NvAPI_DLL_ClientPowerPoliciesSetStatus_t NvClientPowerPoliciesSetStatus;
    NvAPI_GPU_GetCoolersSettings_t NvGetCoolersSettings;
    NvAPI_GPU_SetCoolerLevel_t NvSetCoolerLevel;

private:

    NvU32 _gpuCount;

    NvPhysicalGpuHandle _gpuHandles[NVAPI_MAX_PHYSICAL_GPUS];

    bool _libLoaded;

};

#endif // NVIDIAAPI_H
