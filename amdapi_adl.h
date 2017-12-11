#ifndef AMDAPI_ADL_H
#define AMDAPI_ADL_H

#include <QMutex>
#include <QLibrary>
#include <QObject>

#include "adl_sdk.h"
#include "adl_structures.h"

class amdapi_adl : public QLibrary
{

public:
    amdapi_adl();
    ~amdapi_adl();

    int getGPUCount();

    int getGpuTemperature(int gpu);

    int getGPUOffset(int gpu);
    int getMemOffset(int gpu);

    int getGpuClock(int gpu);
    int getPowerLimit(int gpu);
    int getFanSpeed(int gpu);


    int setMemClockOffset(int gpu,  int clock);
    int setGPUOffset(int gpu, int offset);
    int setPowerLimitPercent(int gpu, unsigned int percent);
    int setTempLimitOffset(int gpu, unsigned int offset);
    int setFanSpeed(int gpu, unsigned int percent);

    bool isInitialized(){return _isInitialized;}

    int getHigherTemp();
    int getLowerTemp();


private:

    // Definitions of the used function pointers. Add more if you use other ADL APIs
    typedef int ( *ADL_MAIN_CONTROL_CREATE )(ADL_MAIN_MALLOC_CALLBACK, int );
    typedef int ( *ADL_MAIN_CONTROL_DESTROY )();
    typedef int ( *ADL_FLUSH_DRIVER_DATA)(int);
    typedef int (*ADL2_ADAPTER_ACTIVE_GET ) (ADL_CONTEXT_HANDLE, int, int* );

    typedef int ( *ADL_ADAPTER_NUMBEROFADAPTERS_GET ) ( int* );
    typedef int ( *ADL_ADAPTER_ADAPTERINFO_GET ) ( LPAdapterInfo, int );
    typedef int ( *ADL_ADAPTERX2_CAPS) (int, int*);
    typedef int ( *ADL2_OVERDRIVE_CAPS) (ADL_CONTEXT_HANDLE context, int iAdapterIndex, int * iSupported, int * iEnabled, int * iVersion);
    typedef int ( *ADL2_OVERDRIVEN_CAPABILITIES_GET)        (ADL_CONTEXT_HANDLE, int, ADLODNCapabilities*);
    typedef int ( *ADL2_OVERDRIVEN_SYSTEMCLOCKS_GET)        (ADL_CONTEXT_HANDLE, int, ADLODNPerformanceLevels*);
    typedef int ( *ADL2_OVERDRIVEN_SYSTEMCLOCKS_SET)        (ADL_CONTEXT_HANDLE, int, ADLODNPerformanceLevels*);
    typedef int ( *ADL2_OVERDRIVEN_MEMORYCLOCKS_GET)        (ADL_CONTEXT_HANDLE, int, ADLODNPerformanceLevels*);
    typedef int ( *ADL2_OVERDRIVEN_MEMORYCLOCKS_SET)        (ADL_CONTEXT_HANDLE, int, ADLODNPerformanceLevels*);
    typedef int ( *ADL2_OVERDRIVEN_PERFORMANCESTATUS_GET) (ADL_CONTEXT_HANDLE, int, ADLODNPerformanceStatus*);
    typedef int ( *ADL2_OVERDRIVEN_FANCONTROL_GET) (ADL_CONTEXT_HANDLE, int, ADLODNFanControl*);
    typedef int ( *ADL2_OVERDRIVEN_FANCONTROL_SET) (ADL_CONTEXT_HANDLE, int, ADLODNFanControl*);
    typedef int ( *ADL2_OVERDRIVEN_POWERLIMIT_GET) (ADL_CONTEXT_HANDLE, int, ADLODNPowerLimitSetting*);
    typedef int ( *ADL2_OVERDRIVEN_POWERLIMIT_SET) (ADL_CONTEXT_HANDLE, int, ADLODNPowerLimitSetting*);
    typedef int ( *ADL2_OVERDRIVEN_TEMPERATURE_GET) (ADL_CONTEXT_HANDLE, int, int, int*);

    ADL_MAIN_CONTROL_CREATE          ADL_Main_Control_Create = NULL;
    ADL_MAIN_CONTROL_DESTROY         ADL_Main_Control_Destroy = NULL;
    ADL_ADAPTER_NUMBEROFADAPTERS_GET ADL_Adapter_NumberOfAdapters_Get = NULL;
    ADL_ADAPTER_ADAPTERINFO_GET      ADL_Adapter_AdapterInfo_Get = NULL;
    ADL_ADAPTERX2_CAPS ADL_AdapterX2_Caps = NULL;
    ADL2_ADAPTER_ACTIVE_GET                         ADL2_Adapter_Active_Get=NULL;
    ADL2_OVERDRIVEN_CAPABILITIES_GET ADL2_OverdriveN_Capabilities_Get = NULL;
    ADL2_OVERDRIVEN_SYSTEMCLOCKS_GET ADL2_OverdriveN_SystemClocks_Get = NULL;
    ADL2_OVERDRIVEN_SYSTEMCLOCKS_SET ADL2_OverdriveN_SystemClocks_Set = NULL;
    ADL2_OVERDRIVEN_PERFORMANCESTATUS_GET ADL2_OverdriveN_PerformanceStatus_Get = NULL;
    ADL2_OVERDRIVEN_FANCONTROL_GET ADL2_OverdriveN_FanControl_Get =NULL;
    ADL2_OVERDRIVEN_FANCONTROL_SET ADL2_OverdriveN_FanControl_Set=NULL;
    ADL2_OVERDRIVEN_POWERLIMIT_GET ADL2_OverdriveN_PowerLimit_Get =NULL;
    ADL2_OVERDRIVEN_POWERLIMIT_SET ADL2_OverdriveN_PowerLimit_Set=NULL;
    ADL2_OVERDRIVEN_MEMORYCLOCKS_GET ADL2_OverdriveN_MemoryClocks_Get = NULL;
    ADL2_OVERDRIVEN_MEMORYCLOCKS_GET ADL2_OverdriveN_MemoryClocks_Set = NULL;
    ADL2_OVERDRIVE_CAPS ADL2_Overdrive_Caps = NULL;
    ADL2_OVERDRIVEN_TEMPERATURE_GET ADL2_OverdriveN_Temperature_Get = NULL;

 private:

    bool _isInitialized;
    int _gpuCount;
    ADL_CONTEXT_HANDLE _context;
    LPAdapterInfo   _lpAdapterInfo;

};

#endif // AMDAPI_ADL_H
