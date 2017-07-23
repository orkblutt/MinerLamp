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

    int getHigherTemp();
    int getLowerTemp();
    int getHigherFanSpeed();
    int getLowerFanSpeed();
    int getMaxClock();
    int getLowerClock();

private:

    unsigned int _gpuCount;

};

#endif // NVIDIANVML_H
