#include "nvidiaapi.h"
#include <QDebug>


nvidiaAPI::nvidiaAPI():
    QLibrary("nvapi64"),
    _gpuHandles{0},
    _gpuCount(0),
    _libLoaded(false),
    NvQueryInterface(NULL),
    NvInit(NULL),
    NvUnload(NULL),
    NvEnumGPUs(NULL),
    NvGetSysType(NULL),
    NvGetName(NULL),
    NvGetMemSize(NULL),
    NvGetMemType(NULL),
    NvGetBiosName(NULL),
    NvGetFreq(NULL),
    NvGetPstates(NULL),
    NvSetPstates(NULL),
    NvGetPStatesInfoEx(NULL),
    NvQueryIlluminationSupport(NULL),
    NvGetIllumination(NULL),
    NvSetIllumination(NULL)
{

    NvQueryInterface = (NvAPI_QueryInterface_t)resolve("nvapi_QueryInterface");
    if(NvQueryInterface)
    {
        NvInit = (NvAPI_Initialize_t)NvQueryInterface(0x0150E828);
        NvUnload = (NvAPI_Unload_t)NvQueryInterface(0xD22BDD7E);
        NvEnumGPUs = (NvAPI_EnumPhysicalGPUs_t)NvQueryInterface(0xE5AC921F);
        NvGetSysType = (NvAPI_GPU_GetSystemType_t)NvQueryInterface(0xBAAABFCC);
        NvGetName = (NvAPI_GPU_GetFullName_t)NvQueryInterface(0xCEEE8E9F);
        NvGetMemSize = (NvAPI_GPU_GetPhysicalFrameBufferSize_t)NvQueryInterface(0x46FBEB03);
        NvGetMemType = (NvAPI_GPU_GetRamType_t)NvQueryInterface(0x57F7CAAC);
        NvGetBiosName = (NvAPI_GPU_GetVbiosVersionString_t)NvQueryInterface(0xA561FD7D);
        NvGetFreq = (NvAPI_GPU_GetAllClockFrequencies_t)NvQueryInterface(0xDCB616C3);
        NvGetPstates = (NvAPI_GPU_GetPstates20_t)NvQueryInterface(0x6FF81213);
        NvSetPstates = (NvAPI_GPU_SetPstates20_t)NvQueryInterface(0x0F4DAE6B);
        NvGetPStatesInfoEx  = (NvAPI_GPU_GetPstatesInfoEx_t)NvQueryInterface(0x843C0256);

        NvClientPowerPoliciesGetStatus = (NvAPI_DLL_ClientPowerPoliciesGetStatus_t)NvQueryInterface(0x70916171);
        NvClientPowerPoliciesGetInfo = (NvAPI_DLL_ClientPowerPoliciesGetInfo_t)NvQueryInterface(0x34206D86);
        NvClientPowerPoliciesSetStatus = (NvAPI_DLL_ClientPowerPoliciesSetStatus_t)NvQueryInterface(0xAD95F5ED);

        //let's have some fun with LED
        NvQueryIlluminationSupport = (NvAPI_GPU_QueryIlluminationSupport_t)NvQueryInterface(0xA629DA31);
        NvGetIllumination = (NvAPI_GPU_GetIllumination_t)NvQueryInterface(0x9A1B9365);
        NvSetIllumination = (NvAPI_GPU_SetIllumination_t)NvQueryInterface(0x254A187);

        NvAPI_Status ret = NvInit();

        qDebug() << "NVAPI success " << ret;

        NvEnumGPUs(_gpuHandles, &_gpuCount);
        qDebug() << _gpuCount;

    }
}

nvidiaAPI::~nvidiaAPI()
{


}



void nvidiaAPI::overClock(unsigned int gpu, unsigned int mem)
{

}

void nvidiaAPI::setLED(unsigned int gpu, int color)
{
    NvAPI_Status ret = NVAPI_OK;


    NV_GPU_QUERY_ILLUMINATION_SUPPORT_PARM illu;
    illu.bSupported = false;
    illu.version = NV_GPU_QUERY_ILLUMINATION_SUPPORT_PARM_VER;
    illu.hPhysicalGpu = _gpuHandles[gpu];
    illu.Attribute = NV_GPU_IA_LOGO_BRIGHTNESS;
    ret = NvQueryIlluminationSupport(&illu);
    if (!ret && illu.bSupported)
    {
        NV_GPU_GET_ILLUMINATION_PARM led;
        led.version = NV_GPU_GET_ILLUMINATION_PARM_VER;
        led.hPhysicalGpu = _gpuHandles[gpu];
        led.Attribute = NV_GPU_IA_LOGO_BRIGHTNESS;
        NvGetIllumination(&led);

        qDebug( "GPU %x: Led level was %d, set to %d", (int) _gpuHandles[gpu], led.Value, color);

        led.Value = (uint32_t) color;
        ret = NvSetIllumination((NV_GPU_SET_ILLUMINATION_PARM*)&led);
    }
    else
    {
        qDebug() << "Unsupported device";
    }
}

int nvidiaAPI::getGPUOffset(unsigned int gpu)
{
    NvAPI_Status ret;


    NV_GPU_PERF_PSTATES20_INFO pset1 = { 0 };
    pset1.version = NV_GPU_PERF_PSTATES20_INFO_VER1;
    pset1.numPstates = 1;
    pset1.numClocks = 1;
    // Ok on both 1080 and 970
    //pset1.pstates[0].clocks[0].domainId = NVAPI_GPU_PUBLIC_CLOCK_GRAPHICS;

    ret = NvGetPstates(_gpuHandles[gpu], &pset1);
    if (ret == NVAPI_OK) {
        qDebug() << pset1.pstates[0].clocks[0].freqDelta_kHz.value;
    }

    return ret;
}



unsigned int nvidiaAPI::getGpuClock(unsigned int gpu)
{
    NvAPI_Status ret = NVAPI_OK;
    unsigned int freq = 0;
    NV_GPU_CLOCK_FREQUENCIES freqs = { 0 };
    freqs.version = NV_GPU_CLOCK_FREQUENCIES_VER;
    freqs.ClockType = NV_GPU_CLOCK_FREQUENCIES_CURRENT_FREQ;
    ret = NvGetFreq(_gpuHandles[gpu], &freqs);
    if (ret == NVAPI_OK) {
        freq = freqs.domain[NVAPI_GPU_PUBLIC_CLOCK_GRAPHICS].frequency / 1000;
    }

    return freq; // in MHz
}

unsigned int nvidiaAPI::getPowerLimit(unsigned int gpu)
{
    NvAPI_Status ret = NVAPI_OK;
    NVAPI_GPU_POWER_STATUS pol = { 0 };
    pol.version = NVAPI_GPU_POWER_STATUS_VER;
    if ((ret = NvClientPowerPoliciesGetStatus(_gpuHandles[gpu], &pol)) != NVAPI_OK)
       return 0;

    return (uint8_t) (pol.entries[0].power / 1000); // in percent

}

int nvidiaAPI::setPowerLimitPercent(unsigned int gpu, unsigned int percent)
{
    NvAPI_Status ret = NVAPI_OK;

    uint32_t val = percent * 1000;

    NVAPI_GPU_POWER_INFO nfo = { 0 };
    nfo.version = NVAPI_GPU_POWER_INFO_VER;
    ret = NvClientPowerPoliciesGetInfo(_gpuHandles[gpu], &nfo);
    if (ret == NVAPI_OK) {
        if (val == 0)
            val = nfo.entries[0].def_power;
        else if (val < nfo.entries[0].min_power)
            val = nfo.entries[0].min_power;
        else if (val > nfo.entries[0].max_power)
            val = nfo.entries[0].max_power;
    }

    NVAPI_GPU_POWER_STATUS pol = { 0 };
    pol.version = NVAPI_GPU_POWER_STATUS_VER;
    pol.flags = 1;
    pol.entries[0].power = val;
    if ((ret = NvClientPowerPoliciesSetStatus(_gpuHandles[gpu], &pol)) != NVAPI_OK)
    {


        return -1;
    }
    return ret;

}

int nvidiaAPI::setMemClockOffset(unsigned int gpu, unsigned int clock)
{
    NvAPI_Status ret;
    NvS32 deltaKHz = clock * 1000;
    // todo: bounds check with GetPstates20

    NV_GPU_PERF_PSTATES20_INFO_V1 pset1 = { 0 };
    pset1.version = NV_GPU_PERF_PSTATES20_INFO_VER1;
    pset1.numPstates = 1;
    pset1.numClocks = 1;
    pset1.pstates[0].clocks[0].domainId = NVAPI_GPU_PUBLIC_CLOCK_MEMORY;
    pset1.pstates[0].clocks[0].freqDelta_kHz.value = deltaKHz;
    ret = NvSetPstates(_gpuHandles[gpu], &pset1);
    if (ret == NVAPI_OK)
        qDebug("GPU #%u: Memory clock offset set to %+d MHz", gpu, deltaKHz / 1000);

    return ret;
}

int nvidiaAPI::setGPUOffset(unsigned int gpu, unsigned int offset)
{
    NvAPI_Status ret;
    NvS32 deltaKHz = offset * 1000;

    NV_GPU_PERF_PSTATES20_INFO_V1 pset1 = { 0 };
    pset1.version = NV_GPU_PERF_PSTATES20_INFO_VER1;
    pset1.numPstates = 1;
    pset1.numClocks = 1;
    // Ok on both 1080 and 970
    pset1.pstates[0].clocks[0].domainId = NVAPI_GPU_PUBLIC_CLOCK_GRAPHICS;
    pset1.pstates[0].clocks[0].freqDelta_kHz.value = deltaKHz;
    ret = NvSetPstates(_gpuHandles[gpu], &pset1);
    if (ret == NVAPI_OK) {
        qDebug("GPU #%u: pu clock offset set to %d MHz", gpu, deltaKHz/1000);
    }
    return ret;

}



int nvidiaAPI::setTempLimitOffset(unsigned int gpu, unsigned int offset)
{
    return 0;
}

void nvidiaAPI::setAllLED(int color)
{
    for(unsigned int i = 0; i < _gpuCount; i++)
        setLED(i, color);
}


/*
int nvidiaAPI::setMemClock(unsigned int gpu, unsigned int clock)
{



    NvAPI_Status ret;
    NvS32 delta = 0;

    // wrong to get default base clock (when modified) on maxwell (same as cuda props one)
    NV_GPU_CLOCK_FREQUENCIES freqs = { 0 };
    freqs.version = NV_GPU_CLOCK_FREQUENCIES_VER;
    freqs.ClockType = NV_GPU_CLOCK_FREQUENCIES_BASE_CLOCK;

    ret =  NvGetFreq(_gpuHandles[gpu], &freqs);
    if (ret == NVAPI_OK)  {
        delta = (clock * 1000) - freqs.domain[NVAPI_GPU_PUBLIC_CLOCK_MEMORY].frequency;

        qDebug() << delta;
    }

    // seems ok on maxwell and pascal for the mem clocks
    NV_GPU_PERF_PSTATES_INFO deffreqs = { 0 };
    deffreqs.version = NV_GPU_PERF_PSTATES_INFO_VER;
    ret = NvGetPStatesInfoEx(_gpuHandles[gpu], &deffreqs, 0x1); // deprecated but req for def clocks
    if (ret == NVAPI_OK) {
        if (deffreqs.pstates[0].clocks[0].domainId == NVAPI_GPU_PUBLIC_CLOCK_MEMORY)
            delta = (clock * 1000) - deffreqs.pstates[0].clocks[0].freq;
    }

    if (delta == (clock * 1000))
        return ret;

    NV_GPU_PERF_PSTATES20_INFO_V1 pset1 = { 0 };
    pset1.version = NV_GPU_PERF_PSTATES20_INFO_VER1;
    pset1.numPstates = 1;
    pset1.numClocks = 1;
    pset1.pstates[0].clocks[0].domainId = NVAPI_GPU_PUBLIC_CLOCK_MEMORY;
    pset1.pstates[0].clocks[0].freqDelta_kHz.value = delta;

    ret = NvSetPstates(_gpuHandles[gpu], &pset1);
    if (ret == NVAPI_OK)
    {
        qDebug("GPU #%u: Boost mem clock set to %u (delta %d)", _gpuCount, clock, delta/1000);
    }
    return ret;

}
*/

/*
 *

unsigned int nvapi_get_gpu_clock(unsigned int devNum)
{
    NvAPI_Status ret = NVAPI_OK;
    unsigned int freq = 0;
    NV_GPU_CLOCK_FREQUENCIES *freqs;
    NV_INIT_STRUCT_ALLOC(NV_GPU_CLOCK_FREQUENCIES, freqs);
    freqs->ClockType = NV_GPU_CLOCK_FREQUENCIES_CURRENT_FREQ;
    ret = NvAPI_GPU_GetAllClockFrequencies(phys[devNum], freqs);
    if (ret == NVAPI_OK) {
        freq = freqs->domain[NVAPI_GPU_PUBLIC_CLOCK_GRAPHICS].frequency / 1000;
    }
    free(freqs);
    return freq; // in MHz
}

uint8_t nvapi_get_plimit(unsigned int devNum)
{
    NvAPI_Status ret = NVAPI_OK;
    NVAPI_GPU_POWER_STATUS pol = { 0 };
    pol.version = NVAPI_GPU_POWER_STATUS_VER;
    if ((ret = NvAPI_DLL_ClientPowerPoliciesGetStatus(phys[devNum], &pol)) != NVAPI_OK) {
        NvAPI_ShortString string;
        NvAPI_GetErrorMessage(ret, string);
        if (opt_debug)
            applog(LOG_DEBUG, "NVAPI PowerPoliciesGetStatus: %s", string);
        return 0;
    }
    return (uint8_t) (pol.entries[0].power / 1000); // in percent
}

int nvapi_set_plimit(unsigned int devNum, uint16_t percent)
{
    NvAPI_Status ret = NVAPI_OK;
    uint32_t val = percent * 1000;

    NVAPI_GPU_POWER_INFO nfo = { 0 };
    nfo.version = NVAPI_GPU_POWER_INFO_VER;
    ret = NvAPI_DLL_ClientPowerPoliciesGetInfo(phys[devNum], &nfo);
    if (ret == NVAPI_OK) {
        if (val == 0)
            val = nfo.entries[0].def_power;
        else if (val < nfo.entries[0].min_power)
            val = nfo.entries[0].min_power;
        else if (val > nfo.entries[0].max_power)
            val = nfo.entries[0].max_power;
    }

    NVAPI_GPU_POWER_STATUS pol = { 0 };
    pol.version = NVAPI_GPU_POWER_STATUS_VER;
    pol.flags = 1;
    pol.entries[0].power = val;
    if ((ret = NvAPI_DLL_ClientPowerPoliciesSetStatus(phys[devNum], &pol)) != NVAPI_OK) {
        NvAPI_ShortString string;
        NvAPI_GetErrorMessage(ret, string);
        if (opt_debug)
            applog(LOG_DEBUG, "NVAPI PowerPoliciesSetStatus: %s", string);
        return -1;
    }
    return ret;
}

int nvapi_set_tlimit(unsigned int devNum, uint8_t limit)
{
    NvAPI_Status ret;
    uint32_t val = limit;

    if (devNum >= nvapi_dev_cnt)
        return -ENODEV;

    NV_GPU_THERMAL_SETTINGS tset = { 0 };
    NVAPI_GPU_THERMAL_INFO tnfo = { 0 };
    NVAPI_GPU_THERMAL_LIMIT tlim = { 0 };
    tset.version = NV_GPU_THERMAL_SETTINGS_VER;
    NvAPI_GPU_GetThermalSettings(phys[devNum], 0, &tset);
    tnfo.version = NVAPI_GPU_THERMAL_INFO_VER;
    NvAPI_DLL_ClientThermalPoliciesGetInfo(phys[devNum], &tnfo);
    tlim.version = NVAPI_GPU_THERMAL_LIMIT_VER;
    if ((ret = NvAPI_DLL_ClientThermalPoliciesGetLimit(phys[devNum], &tlim)) == NVAPI_OK) {
        tlim.entries[0].value = val << 8;
        tlim.flags = 1;
        ret = NvAPI_DLL_ClientThermalPoliciesSetLimit(phys[devNum], &tlim);
        if (ret == NVAPI_OK) {
            applog(LOG_INFO, "GPU #%u: thermal limit set to %u, current Tc %d, range [%u-%u]",
                devNum, val, tset.sensor[0].currentTemp,
                tnfo.entries[0].min_temp >> 8, tnfo.entries[0].max_temp >> 8);
        } else {
            NvAPI_ShortString string;
            NvAPI_GetErrorMessage(ret, string);
            applog(LOG_WARNING, "GPU #%u: thermal limit: %s, valid range is [%u-%u]", devNum, string,
                tnfo.entries[0].min_temp >> 8, tnfo.entries[0].max_temp >> 8);
        }
    }
    return (int) ret;
}

int nvapi_set_gpuclock(unsigned int devNum, uint32_t clock)
{
    NvAPI_Status ret;
    NvS32 delta = 0;

    if (devNum >= nvapi_dev_cnt)
        return -ENODEV;
#if 0
    // wrong api to get default base clock when modified, cuda props seems fine
    NV_GPU_CLOCK_FREQUENCIES freqs = { 0 };
    freqs.version = NV_GPU_CLOCK_FREQUENCIES_VER;
    freqs.ClockType = NV_GPU_CLOCK_FREQUENCIES_BASE_CLOCK;
    ret = NvAPI_GPU_GetAllClockFrequencies(phys[devNum], &freqs);
    if (ret == NVAPI_OK)  {
        delta = (clock * 1000) - freqs.domain[NVAPI_GPU_PUBLIC_CLOCK_GRAPHICS].frequency;
    }

    NV_GPU_PERF_PSTATES_INFO deffreqs = { 0 };
    deffreqs.version = NV_GPU_PERF_PSTATES_INFO_VER;
    ret = NvAPI_GPU_GetPstatesInfoEx(phys[devNum], &deffreqs, 0); // we want default clock grr!
    if (ret == NVAPI_OK) {
        if (deffreqs.pstates[0].clocks[1].domainId == NVAPI_GPU_PUBLIC_CLOCK_GRAPHICS)
            delta = (clock * 1000) - deffreqs.pstates[0].clocks[1].freq*2;
    }
#endif

    cudaDeviceProp props = { 0 };
    NvU32 busId = 0xFFFF;
    ret = NvAPI_GPU_GetBusId(phys[devNum], &busId);
    for (int d=0; d < (int) nvapi_dev_cnt; d++) {
         // unsure about devNum, so be safe
        cudaGetDeviceProperties(&props, d);
        if (props.pciBusID == busId) {
            delta = (clock * 1000) - props.clockRate;
            break;
        }
    }

    if (delta == (clock * 1000))
        return ret;

    NV_GPU_PERF_PSTATES20_INFO_V1 pset1 = { 0 };
    pset1.version = NV_GPU_PERF_PSTATES20_INFO_VER1;
    pset1.numPstates = 1;
    pset1.numClocks = 1;
    // Ok on both 1080 and 970
    pset1.pstates[0].clocks[0].domainId = NVAPI_GPU_PUBLIC_CLOCK_GRAPHICS;
    pset1.pstates[0].clocks[0].freqDelta_kHz.value = delta;
    ret = NvAPI_DLL_SetPstates20v1(phys[devNum], &pset1);
    if (ret == NVAPI_OK) {
        applog(LOG_INFO, "GPU #%u: boost gpu clock set to %u (delta %d)", devNum, clock, delta/1000);
    }
    return ret;
}



 */



