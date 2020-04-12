#include <iostream>
#include <list>

#include "framebuffer.h"
#include "device_scanner.h"

int main() {
    std::cout << "Starting piTFT agent" << std::endl;

    std::list<Framebuffer> list_fb = DeviceScanner<Framebuffer>::findDevices(
        Framebuffer::getDevSearchPath(),
        Framebuffer::getAssociatedMajorNumber()
    );

    if ( list_fb.size() != 1 ) {
        std::cerr << "Frambuffer device not found." << std::endl;
        return -1;
    };

    return 0;
}