#include "nvidiaapi.h"
#include <QDebug>

fanSpeedThread::fanSpeedThread(nvidiaAPI *nvapi, QObject */*pParent*/) :
    _nvapi(nvapi),
    _downLimit(30),
    _upLimit(65)
{

}

void fanSpeedThread::run()
{
    unsigned int gpuCount = _nvapi->getGPUCount();
    while(!_needToStop)
    {
        int previous = 0;
        for(uint i = 0; i < gpuCount; i++)
        {
            int gpuTemp = _nvapi->getGpuTemperature(i);
            if(gpuTemp > _downLimit)
            {
                float step = 100 / (float)(_upLimit - _downLimit);
                float fanLevel = step * (gpuTemp - _downLimit);
                if(previous != (int)fanLevel)
                {
                    _nvapi->setFanSpeed(i, (int)fanLevel);
                    previous = (int)fanLevel;
                }
            }
        }
        QThread::sleep(2);
    }
}

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
    NvSetIllumination(NULL),
    NvGetCoolersSettings(NULL),
    NvSetCoolerLevel(NULL),
    NvGetThermalSettings(NULL)
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

        NvGetCoolersSettings = (NvAPI_GPU_GetCoolersSettings_t)NvQueryInterface(0xDA141340);
        NvSetCoolerLevel =  (NvAPI_GPU_SetCoolerLevel_t)NvQueryInterface(0x891FA0AE);

        NvGetThermalSettings = (NvAPI_GPU_GetThermalSettings_t)NvQueryInterface(0xE3640A56);


        NvAPI_Status ret = NvInit();

        qDebug() << "NVAPI success " << ret;

        _libLoaded = true;
    }
}

nvidiaAPI::~nvidiaAPI()
{


}

unsigned int nvidiaAPI::getGPUCount()
{
    NvEnumGPUs(_gpuHandles, &_gpuCount);
    return _gpuCount;
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

int nvidiaAPI::getGpuTemperature(unsigned int gpu)
{
    NvAPI_Status ret;

    NV_GPU_THERMAL_SETTINGS thermal = {0};
  //  thermal.version = NV_GPU_THERMAL_SETTINGS_VER;

    thermal.version = NV_GPU_THERMAL_SETTINGS_VER;
    thermal.sensor[0].controller = NVAPI_THERMAL_CONTROLLER_GPU_INTERNAL;
    thermal.sensor[0].target = NVAPI_THERMAL_TARGET_GPU;

    ret = NvGetThermalSettings(_gpuHandles[gpu], 0, &thermal);
    if (ret != NVAPI_OK)
    {
        qDebug() << "NVAPI NvAPI_GPU_GetThermalSettings error " << ret;
        return -1;
    }

    return thermal.sensor[0].currentTemp;

}

int nvidiaAPI::getGPUOffset(unsigned int gpu)
{
    NvAPI_Status ret;

    NV_GPU_PERF_PSTATES20_INFO pset1 = { 0 };
    pset1.version = NV_GPU_PERF_PSTATES20_INFO_VER1;
    pset1.numPstates = 1;
    pset1.numClocks = 1;
    // Ok on both 1080 and 970
    pset1.pstates[0].clocks[0].domainId = NVAPI_GPU_PUBLIC_CLOCK_GRAPHICS;

    ret = NvGetPstates(_gpuHandles[gpu], &pset1);
    if (ret == NVAPI_OK) {
        return pset1.pstates[0].clocks[0].freqDelta_kHz.value / 1000;
    }

    return 0;
}

int nvidiaAPI::getMemOffset(unsigned int gpu)
{
    NvAPI_Status ret;

    NV_GPU_PERF_PSTATES20_INFO pset1 = { 0 };
    pset1.version = NV_GPU_PERF_PSTATES20_INFO_VER1;
    pset1.numPstates = 1;
    pset1.numClocks = 1;
    // Ok on both 1080 and 970
    pset1.pstates[0].clocks[0].domainId = NVAPI_GPU_PUBLIC_CLOCK_MEMORY;

    ret = NvGetPstates(_gpuHandles[gpu], &pset1);
    if (ret == NVAPI_OK) {
        return pset1.pstates[0].clocks[1].freqDelta_kHz.value / 1000;
    }

    return 0;
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
    {
        qDebug() << "error";
        return 0;
    }

    return (uint8_t) (pol.entries[0].power / 1000); // in percent

}

unsigned int nvidiaAPI::getFanSpeed(unsigned int gpu)
{
    NV_GPU_COOLER_SETTINGS coolerSettings;
    coolerSettings.version = NV_GPU_COOLER_SETTINGS_VER;

    NvAPI_Status ret = NvGetCoolersSettings(_gpuHandles[gpu], 0, &coolerSettings);
    if(ret == NVAPI_OK)
    {
        return coolerSettings.cooler[0].currentLevel;
    }
    return 0;
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

int nvidiaAPI::setMemClockOffset(unsigned int gpu, int clock)
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

int nvidiaAPI::setGPUOffset(unsigned int gpu, int offset)
{
    NvAPI_Status ret;
    NvS32 deltaKHz = offset * 1000;

    NV_GPU_PERF_PSTATES20_INFO_V1 pset1 = { 0 };
    pset1.version = NV_GPU_PERF_PSTATES20_INFO_VER1;
    pset1.numPstates = 1;
    pset1.numClocks = 1;
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

int nvidiaAPI::setFanSpeed(unsigned int gpu, unsigned int percent)
{
    NV_GPU_COOLER_LEVELS coolerLvl;
    coolerLvl.cooler[0].policy = 1;
    coolerLvl.version = NV_GPU_COOLER_LEVELS_VER;
    coolerLvl.cooler[0].level = percent;

    NvAPI_Status ret = NvSetCoolerLevel(_gpuHandles[gpu], 0, &coolerLvl);

    return ret;
}

void nvidiaAPI::setAllLED(int color)
{
    for(unsigned int i = 0; i < _gpuCount; i++)
        setLED(i, color);
}

void nvidiaAPI::startFanThread()
{
    //if(!_fanThread->isRunning())
    _fanThread = new fanSpeedThread(this);
    connect(_fanThread, SIGNAL(finished()), _fanThread, SLOT(deleteLater()));
    connect(this, SIGNAL(stopFanThrdSignal()), _fanThread, SLOT(onStop()));
    _fanThread->start();
}

void nvidiaAPI::stopFanThread()
{
    //if(_fanThread->isRunning())
    emit stopFanThrdSignal();
    while(_fanThread->isRunning()) QThread::msleep(10);
    disconnect(this, SIGNAL(stopFanThrdSignal()), _fanThread, SLOT(onStop()));
}
