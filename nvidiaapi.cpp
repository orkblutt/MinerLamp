#include "nvidiaapi.h"
#include <QDebug>


nvidiaAPI::nvidiaAPI():
    QLibrary("nvapi64"),
    _gpuHandles{0},
    _gpuCount(0),
    _libLoaded(false),
    NvAPI_DLL_SetPstates20(NULL)
{

        nvapi_QueryInterface = (NVAPI_QUERYINTERFACE)resolve("nvapi_QueryInterface");
        if(nvapi_QueryInterface != Q_NULLPTR)
            _libLoaded = true;

        NvAPI_DLL_SetPstates20 = (NvAPI_DLL_SetPstates20_t)nvapi_QueryInterface(0x0F4DAE6B);

        qDebug() << NvAPI_DLL_SetPstates20;


    NvAPI_Status ret = NvAPI_Initialize();
    if (!ret == NVAPI_OK)
    {
        NvAPI_ShortString string;
        NvAPI_GetErrorMessage(ret, string);
        qDebug("NVAPI NvAPI_Initialize: %s", string);
        return;
    }

    qDebug() << "NVAPI success";

    ret = NvAPI_EnumPhysicalGPUs(_gpuHandles, &_gpuCount);
    if (ret != NVAPI_OK)
    {
        NvAPI_ShortString string;
        NvAPI_GetErrorMessage(ret, string);
        qDebug("NVAPI NvAPI_EnumPhysicalGPUs: %s", string);

    }

    qDebug() << _gpuCount;

    for(NvU32 i = 0; i < _gpuCount; i++)
    {
        NvAPI_ShortString name;
                ret = NvAPI_GPU_GetFullName(_gpuHandles[i], name);

               qDebug() << name;

    }


}

nvidiaAPI::~nvidiaAPI()
{
    unload();

    NvAPI_Unload();
}



void nvidiaAPI::overClock(unsigned int gpu, unsigned int mem)
{

}

int nvidiaAPI::setMemClock(unsigned int clock)
{
    NvAPI_Status ret;
    NvS32 delta = 0;

    // wrong to get default base clock (when modified) on maxwell (same as cuda props one)
    NV_GPU_CLOCK_FREQUENCIES freqs = { 0 };
    freqs.version = NV_GPU_CLOCK_FREQUENCIES_VER;
    freqs.ClockType = NV_GPU_CLOCK_FREQUENCIES_BASE_CLOCK;

    typedef NvAPI_Status (__cdecl * QNVAPI_GPU_GETALLCLOCKFREQUENCIES)       (NvPhysicalGpuHandle, NV_GPU_CLOCK_FREQUENCIES*);
    QNVAPI_GPU_GETALLCLOCKFREQUENCIES       nvapi_GPU_GetAllClockFrequencies = NULL;

    nvapi_GPU_GetAllClockFrequencies = (QNVAPI_GPU_GETALLCLOCKFREQUENCIES)nvapi_QueryInterface(0x1BD69F49);
    qDebug() << nvapi_GPU_GetAllClockFrequencies;

    //ret = NvAPI_GPU_GetAllClockFrequencies(_gpuHandles[0], &freqs); // wrong base clocks, useless
    ret =  nvapi_GPU_GetAllClockFrequencies(_gpuHandles[0], &freqs);
    if (ret == NVAPI_OK)  {
        delta = (clock * 1000) - freqs.domain[NVAPI_GPU_PUBLIC_CLOCK_MEMORY].frequency;

        qDebug() << delta;
    }

    // seems ok on maxwell and pascal for the mem clocks
    NV_GPU_PERF_PSTATES_INFO deffreqs = { 0 };
    deffreqs.version = NV_GPU_PERF_PSTATES_INFO_VER;
    ret = NvAPI_GPU_GetPstatesInfoEx(_gpuHandles[0], &deffreqs, 0x1); // deprecated but req for def clocks
    if (ret == NVAPI_OK) {
        if (deffreqs.pstates[0].clocks[0].domainId == NVAPI_GPU_PUBLIC_CLOCK_MEMORY)
            delta = (clock * 1000) - deffreqs.pstates[0].clocks[0].freq;
    }

    if (delta == (clock * 1000))
        return ret;

    // todo: bounds check with GetPstates20

    NV_GPU_PERF_PSTATES20_INFO_V1 pset1 = { 0 };
    pset1.version = NV_GPU_PERF_PSTATES20_INFO_VER1;
    pset1.numPstates = 1;
    pset1.numClocks = 1;
    pset1.pstates[0].clocks[0].domainId = NVAPI_GPU_PUBLIC_CLOCK_MEMORY;
    pset1.pstates[0].clocks[0].freqDelta_kHz.value = delta;



    ret = NvAPI_DLL_SetPstates20(_gpuHandles[0], &pset1);
    if (ret == NVAPI_OK) {
        qDebug("GPU #%u: Boost mem clock set to %u (delta %d)", _gpuCount, clock, delta/1000);
    }
    return ret;

}




