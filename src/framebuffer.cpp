#include "framebuffer.h"
#include "device.h"


#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <string>


Framebuffer::Framebuffer( const std::string &device_path )
: Device( device_path )
{
    //noop
}

Framebuffer::~Framebuffer()
{
    //noop
}

