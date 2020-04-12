#include "dev_node.h"

#include <iostream>

extern "C" {
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>

}


DeviceNodeNumbers::DeviceNodeNumbers( std::filesystem::path device_path ) {

    struct stat t;

    if ( stat( device_path.c_str(), &t ) == -1 ) {
        std::cerr << "unexpected error on stat(" << device_path.c_str() << ")" << std::endl;
        this->major = this->minor = 0;
    } else {
        this->major = major( t.st_rdev );
        this->minor = minor( t.st_rdev );
        //std::cerr << "stat(" << device_path.c_str() << ") " << t.st_dev << " " << t.st_rdev << " (" << this->major << "," << this->minor << ")" << std::endl;
    }

}


unsigned int DeviceNodeNumbers::getMajor() {
    return this->major;
}

unsigned int DeviceNodeNumbers::getMinor() {
    return this->minor;
}