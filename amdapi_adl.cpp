#include "amdapi_adl.h"
#include <QDebug>

void* __stdcall ADL_Main_Memory_Alloc ( int iSize )
{
    void* lpBuffer = malloc ( iSize );
    return lpBuffer;
}

// Optional Memory de-allocation function
void __stdcall ADL_Main_Memory_Free ( void** lpBuffer )
{
    if ( NULL != *lpBuffer )
    {
        free ( *lpBuffer );
        *lpBuffer = NULL;
    }
}


amdapi_adl::amdapi_adl() : QLibrary("atiadlxx")
  , _isInitialized(false)
  , _gpuCount(0)
  , _lpAdapterInfo(nullptr)
  , _context(nullptr)
{

    qDebug() << "Entering adl constructor";
    ADL_Main_Control_Create = (ADL_MAIN_CONTROL_CREATE) resolve("ADL_Main_Control_Create");
    ADL_Main_Control_Destroy = (ADL_MAIN_CONTROL_DESTROY) resolve("ADL_Main_Control_Destroy");
    ADL_Adapter_NumberOfAdapters_Get = (ADL_ADAPTER_NUMBEROFADAPTERS_GET) resolve("ADL_Adapter_NumberOfAdapters_Get");
    ADL_Adapter_AdapterInfo_Get = (ADL_ADAPTER_ADAPTERINFO_GET) resolve("ADL_Adapter_AdapterInfo_Get");
    ADL_AdapterX2_Caps = (ADL_ADAPTERX2_CAPS) resolve(  "ADL_AdapterX2_Caps");
    ADL2_Adapter_Active_Get = (ADL2_ADAPTER_ACTIVE_GET)resolve( "ADL2_Adapter_Active_Get");
    ADL2_OverdriveN_Capabilities_Get = (ADL2_OVERDRIVEN_CAPABILITIES_GET) resolve ( "ADL2_OverdriveN_Capabilities_Get");
    ADL2_OverdriveN_SystemClocks_Get = (ADL2_OVERDRIVEN_SYSTEMCLOCKS_GET) resolve ( "ADL2_OverdriveN_SystemClocks_Get");
    ADL2_OverdriveN_SystemClocks_Set = (ADL2_OVERDRIVEN_SYSTEMCLOCKS_SET) resolve ( "ADL2_OverdriveN_SystemClocks_Set");
    ADL2_OverdriveN_MemoryClocks_Get = (ADL2_OVERDRIVEN_MEMORYCLOCKS_GET) resolve ( "ADL2_OverdriveN_MemoryClocks_Get");
    ADL2_OverdriveN_MemoryClocks_Set = (ADL2_OVERDRIVEN_MEMORYCLOCKS_SET) resolve ( "ADL2_OverdriveN_MemoryClocks_Set");
    ADL2_OverdriveN_PerformanceStatus_Get = (ADL2_OVERDRIVEN_PERFORMANCESTATUS_GET) resolve ("ADL2_OverdriveN_PerformanceStatus_Get");
    ADL2_OverdriveN_FanControl_Get = (ADL2_OVERDRIVEN_FANCONTROL_GET) resolve ( "ADL2_OverdriveN_FanControl_Get");
    ADL2_OverdriveN_FanControl_Set = (ADL2_OVERDRIVEN_FANCONTROL_SET) resolve ( "ADL2_OverdriveN_FanControl_Set");
    ADL2_OverdriveN_PowerLimit_Get = (ADL2_OVERDRIVEN_POWERLIMIT_GET) resolve ( "ADL2_OverdriveN_PowerLimit_Get");
    ADL2_OverdriveN_PowerLimit_Set = (ADL2_OVERDRIVEN_POWERLIMIT_SET) resolve ( "ADL2_OverdriveN_PowerLimit_Set");
    ADL2_OverdriveN_Temperature_Get = (ADL2_OVERDRIVEN_TEMPERATURE_GET) resolve ( "ADL2_OverdriveN_Temperature_Get");
    ADL2_Overdrive_Caps = (ADL2_OVERDRIVE_CAPS) resolve ( "ADL2_Overdrive_Caps");
    if ( NULL == ADL_Main_Control_Create ||
         NULL == ADL_Main_Control_Destroy ||
         NULL == ADL_Adapter_NumberOfAdapters_Get||
         NULL == ADL_Adapter_AdapterInfo_Get ||
         NULL == ADL_AdapterX2_Caps ||
         NULL == ADL2_Adapter_Active_Get ||
         NULL == ADL2_OverdriveN_Capabilities_Get ||
         NULL == ADL2_OverdriveN_SystemClocks_Get ||
         NULL == ADL2_OverdriveN_SystemClocks_Set ||
         NULL == ADL2_OverdriveN_MemoryClocks_Get ||
         NULL == ADL2_OverdriveN_MemoryClocks_Set ||
         NULL == ADL2_OverdriveN_PerformanceStatus_Get ||
         NULL == ADL2_OverdriveN_FanControl_Get ||
         NULL == ADL2_OverdriveN_FanControl_Set ||
         NULL == ADL2_Overdrive_Caps
         )
    {
        qDebug("Failed to get ADL function pointers");
        return;
    }

    if (ADL_OK != ADL_Main_Control_Create(ADL_Main_Memory_Alloc, 1))
    {
        qDebug("Failed to initialize nested ADL2 context");
        return ;
    }

    getGPUCount();

    if(_gpuCount)
    {
        _lpAdapterInfo = (LPAdapterInfo)malloc ( sizeof (AdapterInfo) * _gpuCount );
        memset ( _lpAdapterInfo,'\0', sizeof (AdapterInfo) * _gpuCount );

        // Get the AdapterInfo structure for all adapters in the system
        ADL_Adapter_AdapterInfo_Get (_lpAdapterInfo, sizeof (AdapterInfo) * _gpuCount);
        _isInitialized = true;
    }
}


amdapi_adl::~amdapi_adl()
{
    if(_isInitialized)
    {
        free(_lpAdapterInfo);
        ADL_Main_Control_Destroy();
    }
}

int amdapi_adl::getGPUCount()
{
    if ( ADL_OK != ADL_Adapter_NumberOfAdapters_Get ( &_gpuCount ) )
    {
        qDebug("Cannot get the number of adapters!");
        return 0;
    }

    return _gpuCount;
}

int amdapi_adl::getGpuTemperature(int gpu)
{
    int temp;
    int iSupported,iEnabled,iVersion;
    ADL2_Overdrive_Caps(_context, _lpAdapterInfo[gpu].iAdapterIndex, &iSupported, &iEnabled, &iVersion);
    if (iVersion == 7)
    {
        ADL2_OverdriveN_Temperature_Get(_context,_lpAdapterInfo[gpu].iAdapterIndex,1, &temp);
        return temp;
    }
    return 0;
}

int amdapi_adl::getGPUOffset(int gpu)
{
    return 0;
}

int amdapi_adl::getMemOffset(int gpu)
{
    return 0;
}

int amdapi_adl::getGpuClock(int gpu)
{
    return 0;
}

int amdapi_adl::getPowerLimit(int gpu)
{
    return 0;
}

int amdapi_adl::getFanSpeed(int gpu)
{
    return 0;
}

int amdapi_adl::setMemClockOffset(int gpu, int clock)
{
    return 0;
}

int amdapi_adl::setGPUOffset(int gpu, int offset)
{
    return 0;
}

int amdapi_adl::setPowerLimitPercent(int gpu, unsigned int percent)
{
    return 0;
}

int amdapi_adl::setTempLimitOffset(int gpu, unsigned int offset)
{
    return 0;
}

int amdapi_adl::setFanSpeed(int gpu, unsigned int percent)
{
    return 0;
}

int amdapi_adl::getHigherTemp()
{
    unsigned int maxTemp = 0;
    unsigned int gpuCount = getGPUCount();
    for(unsigned int i = 0; i < gpuCount; i++)
    {
        unsigned int temp = getGpuTemperature(i);
        if(temp > maxTemp)
            maxTemp = temp;
    }
    return maxTemp;
}

int amdapi_adl::getLowerTemp()
{
    unsigned int minTemp = 100000000;
    unsigned int gpuCount = getGPUCount();
    for(unsigned int i = 0; i < gpuCount; i++)
    {
        unsigned int temp = getGpuTemperature(i);
        if(temp < minTemp)
            minTemp = temp;
    }
    return minTemp;
}


