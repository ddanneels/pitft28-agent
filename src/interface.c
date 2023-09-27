// To get usleep
#define _DEFAULT_SOURCE

#include <stdio.h>
#include <unistd.h>

#include "lvgl/lvgl.h"
#include "interface.h"
#include "pitft_agent.h"

lv_obj_t *main_screen;
lv_obj_t *exit_btn;
lv_obj_t *exit_btn_label;


void exit_btn_event_cb(lv_event_t *event);

void exit_btn_event_cb(lv_event_t *event)
{
  if( lv_event_get_target(event) == exit_btn ) {
    printf("Hey, I've received an event !\n");
  }
  if( lv_event_get_code(event) == LV_EVENT_CLICKED ) {
    g_exit_requested = true;
  }
}

void setup_interface()
{
  main_screen = lv_obj_create(NULL);
  exit_btn = lv_btn_create(main_screen);
  lv_obj_set_pos(exit_btn, 10, 10);
  lv_obj_set_size(exit_btn, 100, 50);
  lv_obj_add_event_cb(exit_btn, exit_btn_event_cb, LV_EVENT_CLICKED, NULL);

  lv_obj_t * exit_btn_label = lv_label_create(exit_btn);
  lv_label_set_text(exit_btn_label, "Exit");
}


void main_loop() {
  while ( ! g_exit_requested ) {
    usleep(5000);
    lv_task_handler();
  };
}