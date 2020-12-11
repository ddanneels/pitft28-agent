#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <threads.h>
#include <unistd.h>


#include <uchar.h>
#include <wchar.h>

#include "lvgl/lvgl.h"

#include "touchscreen.h"
#include "framebuffer.h"
#include "pitft_agent.h"
#include "interface.h"


static lv_disp_buf_t disp_buf;
static lv_color_t *buf = NULL;


bool g_exit_requested = false;

void sigterm_handler(int signal)
{
  g_exit_requested = true;
}



int tick_thread (void *args)
{
  while(!g_exit_requested) {
    usleep(5*1000);
    lv_tick_inc(5);
  }
  return 0;
}

int main(int argc, char* argv[]) {

  char*                touchscreen_path;
  char*                framebuffer_path;
  lv_disp_drv_t        disp_drv;
  lv_indev_drv_t       touchscreen_drv;

  // Several threads
  thrd_t thr_touchscreen;
  thrd_t thr_tick;

  // LittlevGL initialization
  lv_init();
  lv_disp_drv_init(&disp_drv);
  lv_indev_drv_init(&touchscreen_drv);

  // Probing devices
  // Input devices may change depending on hardware connected at boot
  if (( touchscreen_path = find_touchscreen_device() ) == NULL ) {
    fprintf(stderr, "No touchscreen device found\n");
  };

  if (( framebuffer_path = find_framebuffer_device() ) == NULL ) {
    fprintf(stderr, "No framebuffer device found\n");
  };

  if ( touchscreen_path == NULL || framebuffer_path == NULL ) {
    if (touchscreen_path != NULL ) 
      free(touchscreen_path);

    if (framebuffer_path != NULL ) 
      free(framebuffer_path);

    return 1;
  }

  // Listen for Ctrl-C
  signal(SIGINT, sigterm_handler );

  if ( open_framebuffer( framebuffer_path ) != 0 ) {
    return 1;
  };

  Fill( 0, 0, fb->xres-1, fb->yres-1, rgb(0, 0, 0xc0));

  // Continue with LittlevGL configuration,
  // for display and touch input
  buf = malloc( fb->xres * fb->yres * sizeof(lv_color_t));
  lv_disp_buf_init(&disp_buf, buf, NULL, fb->xres * fb->yres); 

  disp_drv.hor_res  = fb->xres;
  disp_drv.ver_res  = fb->yres;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.buffer   = &disp_buf;
  lv_disp_drv_register(&disp_drv);    

  touchscreen_drv.type    = LV_INDEV_TYPE_POINTER;
  touchscreen_drv.read_cb = read_touchscreen;
  lv_indev_drv_register(&touchscreen_drv);

  setup_interface();

  // Launch auxiliary threads 
  thrd_create( &thr_touchscreen, touchscreen_listener, (void*) touchscreen_path );
  thrd_create( &thr_tick, tick_thread, NULL);

  main_loop();

  // Wait for threads termination
  thrd_join( thr_touchscreen, NULL );
  thrd_join( thr_tick, NULL);

  // Clean-up
  Fill( 0, 0, fb->xres-1, fb->yres-1, rgb(0, 0, 0));

  printf("Thank you. Bye.\n");

  close_framebuffer();

  // Free globally allocated memory
  if (touchscreen_path != NULL ) 
    free(touchscreen_path);

  if (framebuffer_path != NULL ) 
    free(framebuffer_path);
  
  return 0;
}



