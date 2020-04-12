#include "device_scanner.h"
#include "framebuffer.h"

#include <iostream>
#include <list>
#include <string>

#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>

// to prevent linker error, we need to list here all needed templates
template class DeviceScanner<Framebuffer>;


template <class T>
std::list<T> DeviceScanner<T>::findDevices(const std::string &dev_path, unsigned int major) {
    DIR*           dir_handler;
    struct dirent *dir_entry;
    struct stat    file_stat;

    std::list<T> list;

    if((dir_handler = opendir(dev_path.c_str())) != NULL) {
        while((dir_entry=readdir(dir_handler)) != NULL) {
            if ( dir_entry->d_type == DT_CHR ) {
                std::string device_path = std::string(dev_path).append("/").append(dir_entry->d_name);
                if ( (stat( device_path.c_str(), &file_stat )) == 0 ) {
                    if ( major( file_stat.st_rdev ) == major ) {
                        std::cout << device_path << std::endl;
                    }
                }
            }
        }
    }
    closedir(dir_handler);

    return list;
}
