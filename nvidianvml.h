#ifndef NVIDIANVML_H
#define NVIDIANVML_H


#include <nvml.h>


class nvidiaNVML
{
public:
    nvidiaNVML();

    bool initNVML();

    unsigned int getGPUCount();

    void shutDownNVML();

    int getGPUTemp(unsigned int index);
    int getFanSpeed(unsigned int index);
    int getMemClock(unsigned int index);
    int getGPUClock(unsigned int index);
    int getPowerDraw(unsigned int index);

    int getMaxSupportedMemClock(unsigned int index);

    int getHigherTemp();
    int getLowerTemp();

    int getHigherFanSpeed();
    int getLowerFanSpeed();

    int getMemMaxClock();
    int getMemLowerClock();

    int getGPUMaxClock();
    int getGPUMinClock();

    int getMaxPowerDraw();
    int getMinPowerDraw();

    int getPowerDrawSum();


    void setClock(unsigned int index);

private:

    unsigned int _gpuCount;

};

#endif // NVIDIANVML_H
