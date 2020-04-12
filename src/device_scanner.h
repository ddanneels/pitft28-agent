#ifndef PITFT_AGENT_DEVICE_SCANNER_H
#define PITFT_AGENT_DEVICE_SCANNER_H

#include "device.h"

#include <list>
#include <string>

template <class T>
class DeviceScanner {
public:
    static std::list<T> findDevices(const std::string &dev_path, unsigned int major);


};

#endif // PITFT_AGENT_DEVICE_SCANNER_H