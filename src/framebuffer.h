#ifndef PITFT_AGENT_FRAMEBUFFER_H
#define PITFT_AGENT_FRAMEBUFFER_H

#include "device.h"

#include <string>

class Framebuffer : public Device {
public:
    static unsigned int getAssociatedMajorNumber() { return 29; }
    static std::string getDevSearchPath() { return "/dev"; }

    Framebuffer( const std::string &device_path );
    ~Framebuffer();
};


#endif // PITFT_AGENT_FRAMEBUFFER_H