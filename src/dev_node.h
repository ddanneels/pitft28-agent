#ifndef PITFT_AGENT_DEV_NODE_H
#define PITFT_AGENT_DEV_NODE_H

#include <filesystem>

class DeviceNodeNumbers {
private:
    unsigned int major;
    unsigned int minor;

public:
    DeviceNodeNumbers( std::filesystem::path device_path );

    unsigned int getMajor();

    unsigned int getMinor();
};

#endif // PITFT_AGENT_DEV_NODE_H