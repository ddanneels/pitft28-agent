
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

#include "lv_drivers/display/monitor.h"
#include "lv_drivers/indev/mouse.h"

#include "pitft_agent.h"
#include "interface.h"


bool g_exit_requested = false;

void sigterm_handler(int signal)
{
  g_exit_requested = true;
}



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
  /* Use the 'monitor' driver which creates window on PC's monitor to simulate a display*/
  monitor_init();

  /*Create a display buffer*/
  lv_disp_drv_t        disp_drv;
  static lv_disp_buf_t disp_buf1;
  static lv_color_t buf1_1[LV_HOR_RES_MAX * 120];
  lv_disp_buf_init(&disp_buf1, buf1_1, NULL, LV_HOR_RES_MAX * 120);

  /*Create a display*/
  lv_disp_drv_init(&disp_drv); /*Basic initialization*/
  disp_drv.buffer = &disp_buf1;
  disp_drv.flush_cb = monitor_flush;
  lv_disp_drv_register(&disp_drv);

  /* Add the mouse as input device
   * Use the 'mouse' driver which reads the PC's mouse*/
  mouse_init();
  lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv); /*Basic initialization*/
  indev_drv.type = LV_INDEV_TYPE_POINTER;

  /*This function will be called periodically (by the library) to get the mouse position and state*/
  indev_drv.read_cb = mouse_read;
  lv_indev_t *mouse_indev = lv_indev_drv_register(&indev_drv);

  /*Set a cursor for the mouse*/
  LV_IMG_DECLARE(mouse_cursor_icon); /*Declare the image file.*/
  lv_obj_t * cursor_obj = lv_img_create(lv_scr_act(), NULL); /*Create an image object for the cursor */
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

  printf("Thank you. Bye.\n");

  
  return 0;
}



