#ifndef PITFT_AGENT_DEVICE_H
#define PITFT_AGENT_DEVICE_H

#include <string>

class Device {
protected:
    std::string device_path;
public:
    Device( const std::string &device_path ) {
        this->device_path = device_path;
    };
};


#endif // PITFT_AGENT_DEVICE_H
