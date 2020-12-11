// To get type on directory entries
#define _DEFAULT_SOURCE

#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <threads.h>

#include <libevdev/libevdev.h>

#include "touchscreen.h"
#include "pitft_agent.h"
#include "framebuffer.h"

mtx_t *touchscreen_mutex;
lv_coord_t touchscreen_x = 0;
lv_coord_t touchscreen_y = 0;
bool touchscreen_pressed = false;


/**
 * @brief Search and return path name of the touchscreen.
 *
 * @return an allocated string if found or NULL otherwise.
 */
char* find_touchscreen_device() {

  const char*      dev_folder = "/dev/input";
  const int        major = 13; // major number for input devices

  DIR*             dir_handler = NULL;
  struct dirent*   dir_entry = NULL;
  struct stat      file_stat;
  char*            device_path = malloc(FILENAME_MAX);
  struct libevdev* dev = NULL;
  int              fd;
  int              rc = 1;
  bool             found = false;

  if ((dir_handler = opendir(dev_folder)) == NULL) {
    fprintf(stderr, "Error when scanning %s for touchscreen device : %s\n", dev_folder, strerror(errno));
  } else {
    while (!found && (dir_entry = readdir(dir_handler)) != NULL) {
      if ( dir_entry->d_type == DT_CHR ) {
        snprintf(device_path, FILENAME_MAX, "%s/%s", dev_folder, dir_entry->d_name);
        if ( (stat( device_path, &file_stat )) != 0 ) {
          fprintf(stderr, "Error when scanning %s for touchscreen device : %s\n", device_path, strerror(errno));
        } else {
          if ( major( file_stat.st_rdev ) == major ) {
            
            fd = open(device_path, O_RDONLY|O_NONBLOCK);
            if ( fd < 0 ) {
              fprintf(stderr, "Failed to open device (%s)\n", strerror(errno));
            } else {
              rc = libevdev_new_from_fd(fd, &dev);
              if (rc < 0) {
                fprintf(stderr, "Failed to init libevdev (%s)\n", strerror(-rc));
              } else {
                printf("device %s\n", device_path);
                printf("  Input device name: \"%s\"\n", libevdev_get_name(dev));
                printf("  Input device ID: bus %#x vendor %#x product %#x\n",
                      libevdev_get_id_bustype(dev),
                      libevdev_get_id_vendor(dev),
                      libevdev_get_id_product(dev));

                if (!libevdev_has_event_type(dev, EV_ABS) ||
                    !libevdev_has_event_code(dev, EV_ABS, ABS_X) ||
                    !libevdev_has_event_code(dev, EV_ABS, ABS_Y) ) {
                  printf("  --> This is not the touchscreen we are looking for.\n");
                } else {
                  printf("  --> This is it : %s\n", device_path);
                  found = true;
                }
                libevdev_free(dev);
              }
              close(fd);
            }
          }
        }
      }
    }
  }
  closedir(dir_handler);
  if ( found ) {
    return device_path;
  } else {
    free(device_path);
    return NULL;
  }
}




/**
 * @brief Open the touchscreen device
 *
 * @return an allocated string if found or NULL otherwise.
 */
int touchscreen_listener( void* device_path ) {
  struct libevdev*     dev = NULL;
  int                  fd;
  int                  rc;
  struct input_event   ev;
  int                  read_state = LIBEVDEV_READ_FLAG_NORMAL;
  bool                 pressed = false;
  int                  x = 0, y = 0;
  int                  maxx = 0;

  touchscreen_mutex = malloc(sizeof(mtx_t));
  mtx_init(touchscreen_mutex, mtx_plain);

  fd = open(device_path, O_RDONLY|O_NONBLOCK);
  if ( fd < 0 ) {
    fprintf(stderr, "Failed to open device (%s)\n", strerror(errno));
  } else {
    rc = libevdev_new_from_fd(fd, &dev);
    if (rc < 0) {
      fprintf(stderr, "Failed to init libevdev (%s)\n", strerror(-rc));
    } else {
      maxx = libevdev_get_abs_maximum(dev, ABS_X);
      do {
        rc = libevdev_next_event(dev, read_state, &ev);
        switch (rc) {
          case LIBEVDEV_READ_STATUS_SUCCESS:
            break;
          case LIBEVDEV_READ_STATUS_SYNC:
            read_state = LIBEVDEV_READ_FLAG_SYNC;
            break;
          default:
            read_state = LIBEVDEV_READ_FLAG_NORMAL;
            break;
        }

        switch (ev.type) {
          case EV_ABS:
          case EV_KEY:
            // We consider single and multi-touch events alike because
            // multitouch is not physically supported by the device

            // Coordinates are reversed x/y 
            switch (ev.code) {
              case ABS_X:
              case ABS_MT_POSITION_X:
                y = maxx - ev.value;
                break;
              case ABS_Y:
              case ABS_MT_POSITION_Y:
                x = ev.value;
                break;
              case BTN_TOUCH:
                if ( ev.value == 1 && !pressed ) {
                  pressed = true;
                }
                if ( ev.value == 0 && pressed ) {
                  pressed = false;
                }
                break;
            }

            if ( touchscreen_x != x || touchscreen_y != y || touchscreen_pressed != pressed ) {
              mtx_lock(touchscreen_mutex);
              if ( pressed ) {
                touchscreen_x = x;
                touchscreen_y = y;
                printf("%dx%d\n", touchscreen_x, touchscreen_y);
              }
              touchscreen_pressed = pressed;
              mtx_unlock(touchscreen_mutex);
            }

            break;
        }
        usleep(5000);
      } while ( ! g_exit_requested );
      libevdev_free(dev);
    }
    close(fd);
  }
  return 0;
}



bool read_touchscreen(lv_indev_drv_t * indev, lv_indev_data_t * data)
{
    mtx_lock(touchscreen_mutex);
    /*Save the state and save the pressed coordinate*/
    data->state = touchscreen_pressed ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;

    /*Set the coordinates (if released use the last pressed coordinates)*/
    data->point.x = touchscreen_x;
    data->point.y = touchscreen_y;
    mtx_unlock(touchscreen_mutex);
    return false; /*Return `false` because we are not buffering and no more data to read*/
}
