#ifndef SRC_FRAMEBUFFER_H
#define SRC_FRAMEBUFFER_H

#include <uchar.h>

#include "lvgl/lvgl.h"

struct framebuffer {
  char16_t*    mem;
  unsigned int memsize;
  unsigned int line_length;
  int          fd;
  unsigned int xres, yres;
};
typedef struct framebuffer framebuffer_t;

extern framebuffer_t* fb;

/*
struct rgba {
  unsigned int r, g, b, a;
};
typedef struct rgba rgba_t;
*/

char* find_framebuffer_device();

int open_framebuffer( char* device_path );

void close_framebuffer( );



char16_t rgb( unsigned char r, unsigned char v, unsigned char b );


void SetPixel( unsigned int x, unsigned int y, char16_t color );

void Fill( unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, char16_t color );

void my_disp_flush(lv_disp_drv_t * disp, const lv_area_t * area, lv_color_t * color_p);


#endif // SRC_FRAMEBUFFER_H