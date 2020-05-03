#ifndef SRC_TOUCHSCREEN_H
#define SRC_TOUCHSCREEN_H

#include "lvgl/lvgl.h"

char *find_touchscreen_device();

int touchscreen_listener( void* device_path );

bool my_touchpad_read(lv_indev_drv_t * indev, lv_indev_data_t * data);

#endif // SRC_TOUCHSCREEN_H