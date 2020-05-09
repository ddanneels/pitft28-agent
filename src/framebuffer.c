// To get type on directory entries
#define _DEFAULT_SOURCE

#include <dirent.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <uchar.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <sys/mman.h>
#include <wiringPi.h>

#include <linux/fb.h>

#include "framebuffer.h"

framebuffer_t* fb = NULL;


/**
 * @brief Search and return path name of the framebuffer.
 *
 * @return an allocated string if found or NULL otherwise.
 */
char* find_framebuffer_device() {

  const char*      dev_folder = "/dev";
  const int        major = 29; // major number for framebuffer devices

  DIR*                      dir_handler = NULL;
  struct dirent*            dir_entry = NULL;
  struct stat               file_stat;
  struct fb_fix_screeninfo  fix_screeninfo;
  struct fb_var_screeninfo  var_screeninfo;
  char*                     device_path = malloc(FILENAME_MAX);
  int                       fd;
  bool                      found = false;

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
            
            fd = open(device_path, O_RDONLY);
            if ( fd < 0 ) {
              fprintf(stderr, "Failed to open device (%s)\n", strerror(errno));
            } else {
            
              if ( ioctl(fd, FBIOGET_FSCREENINFO, &fix_screeninfo) == -1 
                || ioctl(fd, FBIOGET_VSCREENINFO, &var_screeninfo) == -1 ) {
                  fprintf(stderr, "Failed to ioctl device (%s)\n", strerror(errno));
              } else {
                printf("device %s\n", device_path);
                printf("  %s\n", fix_screeninfo.id);
                printf("  Resolution (%dx%d) %d bpp\n", var_screeninfo.xres, var_screeninfo.yres, var_screeninfo.bits_per_pixel);
                if ( fix_screeninfo.type == FB_TYPE_PACKED_PIXELS 
                  && fix_screeninfo.visual == FB_VISUAL_TRUECOLOR 
                  && var_screeninfo.xres <= 640
                  && var_screeninfo.yres <= 480
                  && var_screeninfo.bits_per_pixel == 16 
                ) {
                  printf("  --> This is it : %s\n", device_path);
                  found = true;
                } else {
                  printf("  --> This is not the touchscreen we are looking for.\n");
                }
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


int open_framebuffer( char* device_path ) {

  struct fb_fix_screeninfo  fix_screeninfo;
  struct fb_var_screeninfo  var_screeninfo;
  
  fb = malloc( sizeof( struct framebuffer ) );
  
  fb->fd = open(device_path, O_RDWR);
  if ( fb->fd < 0 ) {
    fprintf(stderr, "Failed to open device (%s)\n", strerror(errno));
  } else {
  
    if ( ioctl(fb->fd, FBIOGET_FSCREENINFO, &fix_screeninfo) == -1 
      || ioctl(fb->fd, FBIOGET_VSCREENINFO, &var_screeninfo) == -1 ) {
        fprintf(stderr, "Failed to ioctl device (%s)\n", strerror(errno));
    } else {

      printf("  smem_len: %d\n", fix_screeninfo.smem_len);
      printf("  smem_start: %p\n", (void*) fix_screeninfo.smem_start);
      printf("  line_length: %d\n", fix_screeninfo.line_length);
      printf("  mmio_start: %p\n", (void*) fix_screeninfo.mmio_start);
      printf("  mmio_len: %d\n", fix_screeninfo.mmio_len);
      printf("  size: %d x %d mm\n", var_screeninfo.width, var_screeninfo.height );
      printf("  resolution: %dx%d %d bpp\n", var_screeninfo.xres, var_screeninfo.yres, var_screeninfo.bits_per_pixel);

      fb->memsize = var_screeninfo.xres * var_screeninfo.yres * var_screeninfo.bits_per_pixel / 8;
      fb->line_length = fix_screeninfo.line_length;
      fb->xres = var_screeninfo.xres;
      fb->yres = var_screeninfo.yres;

      fb->mem = mmap( NULL, fb->memsize, PROT_READ | PROT_WRITE, MAP_SHARED, fb->fd, 0);
      if ( fb->mem == (char16_t*) -1 ) {
        fprintf(stderr, "Failed to map framebuffer (%s)\n", strerror(errno));
      } else {
        printf("  mmap: %p\n", fb->mem);
        printf("  memsize: %d\n", fb->memsize);
        if ( wiringPiSetup() != 0 ) {
          fprintf(stderr, "Failed to map framebuffer (%s)\n", strerror(errno));
        } else {
          // Turn the backlight ON
          digitalWrite(1, 1);
        }
        return 0;
      }
    }    
    close( fb->fd );
  }

  free( fb );
  return -1;
}



char16_t rgb( unsigned char r, unsigned char g, unsigned char b ) {
  return (char16_t) ( ( ( r & 0xF8 ) << 8 ) 
                    | ( ( g & 0xFC ) << 3 ) 
                    | ( ( b & 0xF8 ) >> 3 ) ); 
}

void SetPixel( unsigned int x, unsigned int y, char16_t color ) {
  int addr = ( y * fb->xres ) + x;
  assert( addr < ( fb->xres * fb->yres ));

  fb->mem[addr] = color;
}

void Fill( unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, char16_t color ) {
  for ( int y = y1 ; y <= y2 ; y++) {
    for ( int x = x1 ; x <= x2 ; x++ ) {
      SetPixel( x, y, color );
    }
  }
}


void close_framebuffer() {
  if ( munmap( fb->mem, fb->memsize) != 0 ) {
    fprintf(stderr, "Failed to unmap framebuffer (%s)\n", strerror(errno));
  }
  if ( close( fb->fd) != 0 ) {
    fprintf(stderr, "Failed to close framebuffer (%s)\n", strerror(errno));
  }
  // Turn the backlight OFF
  digitalWrite(1, 0);

  free( fb);
  fb = NULL;
}


void my_disp_flush(lv_disp_drv_t * disp, const lv_area_t * area, lv_color_t * color_p)
{
    unsigned int x, y;
    for(y = area->y1; y <= area->y2; y++) {
        for(x = area->x1; x <= area->x2; x++) {
            SetPixel( x, y, lv_color_to16(*color_p) );  /* Put a pixel to the display.*/
            color_p++;
        }
    }

    lv_disp_flush_ready(disp);         /* Indicate you are ready with the flushing*/
}