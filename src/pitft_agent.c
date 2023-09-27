
// pitft28-agent, (GPLv3 License) copyright (C) 2021  Damien DANNEELS
//
// Credits :
// LVGL library and drivers (MIT License) : LVGL LLC, https://lvgl.io
// PiTFT : Adafruit Industries, LLC, https://adafruit.com
// Raspberry Pi : Raspberry Pi Foundation, https://www.raspberrypi.org/
//



#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <threads.h>
#include <unistd.h>

#include <uchar.h>
#include <wchar.h>

#include "lvgl/lvgl.h"

#include "lv_drivers/display/fbdev.h"
#include "lv_drivers/indev/evdev.h"

#include "pitft_agent.h"
#include "interface.h"


bool g_exit_requested = false;

void sigterm_handler(int signal);

void sigterm_handler(int signal)
{
  g_exit_requested = true;
}

int tick_thread (void *args);

int tick_thread (void *args)
{
  (void)args;

  while(!g_exit_requested) {
    usleep(5*1000);
    lv_tick_inc(5);
  }
  return 0;
}

int main(int argc, char* argv[]) {


  // Several threads
  thrd_t thr_tick;

  // LittlevGL initialization
  lv_init();
  fbdev_init();

  /*Create a display buffer*/
  lv_disp_drv_t        disp_drv;
  static lv_disp_draw_buf_t draw_buf;
  static lv_color_t buf1[64000];                        /*Declare a buffer for 1/10 screen size*/
  lv_disp_draw_buf_init(&draw_buf, buf1, NULL, 64000);  /*Initialize the display buffer.*/

  /*Create a display*/
  lv_disp_drv_init(&disp_drv); /*Basic initialization*/
  disp_drv.draw_buf = &draw_buf;
  disp_drv.flush_cb = fbdev_flush;
  lv_disp_drv_register(&disp_drv);

  evdev_init();
  lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv); /*Basic initialization*/
  indev_drv.type = LV_INDEV_TYPE_POINTER;

  /*This function will be called periodically (by the library) to get the mouse position and state*/
  indev_drv.read_cb = evdev_read;
  lv_indev_t *mouse_indev = lv_indev_drv_register(&indev_drv);

  /*Set a cursor for the mouse*/
  LV_IMG_DECLARE(mouse_cursor_icon); /*Declare the image file.*/
  lv_obj_t * cursor_obj = lv_img_create(lv_scr_act()); /*Create an image object for the cursor */
  lv_img_set_src(cursor_obj, &mouse_cursor_icon);           /*Set the image source*/
  lv_indev_set_cursor(mouse_indev, cursor_obj);             /*Connect the image  object to the driver*/

  // Listen for Ctrl-C
  signal(SIGINT, sigterm_handler );

  setup_interface();

  // Launch auxiliary threads 
  //thrd_create( &thr_touchscreen, touchscreen_listener, (void*) touchscreen_path );
  thrd_create( &thr_tick, tick_thread, NULL);

  main_loop();

  // Wait for threads termination
  //thrd_join( thr_touchscreen, NULL );
  thrd_join( thr_tick, NULL);

  printf("\nThank you. Bye.\n");

  
  return 0;
}



