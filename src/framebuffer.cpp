#include "framebuffer.h"
#include "dev_node.h"

#include <filesystem>
#include <iostream>


int Framebuffer::find_framebuffer()
{
    const std::string DEV_DIR_NAME("/dev");

    int counter = 0;

    for ( const std::filesystem::directory_entry entry : std::filesystem::directory_iterator(DEV_DIR_NAME) ) {
        
        if ( entry.is_character_file() ) {
            DeviceNodeNumbers n = DeviceNodeNumbers(entry.path());
            if ( n.getMajor() == 29 ) {
                std::cout << entry.path() << std::endl;
                counter++;
            }
        }
    }

    return counter;
}

Framebuffer::Framebuffer( char* device )
{
    //noop
}

Framebuffer::~Framebuffer()
{
    //noop
}

