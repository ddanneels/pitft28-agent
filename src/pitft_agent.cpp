#include <iostream>

#include "framebuffer.h"


int main() {
    std::cout << "Starting piTFT agent" << std::endl;

    if ( Framebuffer::find_framebuffer() != 1 ) {
        std::cerr << "Frambuffer device not found." << std::endl;
        return -1;
    };

    return 0;
}