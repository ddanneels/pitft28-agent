// To get usleep
#define _DEFAULT_SOURCE

#include <stdio.h>
#include <unistd.h>

#include "lvgl/lvgl.h"

#include "pitft_agent.h"


static const char * pincode_kb_map[] = {
                     "1", "2", "3", "4", "5", LV_SYMBOL_BACKSPACE, "\n",
                     "6", "7", "8", "9", "0", LV_SYMBOL_OK, ""};

lv_obj_t *lock_screen;
lv_obj_t *main_screen;
lv_obj_t *pincode_label;
lv_obj_t *pincode_ta;
lv_obj_t *pincode_kb;

void btn_event_cb(lv_obj_t * btn, lv_event_t event)
{
  if(event == LV_EVENT_CLICKED) {
    g_exit_requested = true;
  }
}

void pincode_event_cb( lv_obj_t *kb, lv_event_t event) 
{
  static int tries = 3;

  lv_kb_def_event_cb(kb, event);

  if (event == LV_EVENT_APPLY) {
    printf(lv_ta_get_text(pincode_ta));
    printf("\n");
    if ( strncmp( lv_ta_get_text(pincode_ta), "1397", 10) == 0 ) {
      lv_label_set_text( pincode_label, "Welcome home !");
      lv_scr_load(main_screen);
    } else {
      char n_left[] = "x tries left";
      switch ( --tries ) {
      case 0:
        lv_label_set_text( pincode_label, "Failed");
        g_exit_requested = true;
        break;
      case 1:
        lv_label_set_text( pincode_label, "This is your last try");
        break;
      default:        
        n_left[0] = '0' + tries;
        lv_label_set_text( pincode_label, n_left);
        break;
      }
    }
    lv_ta_set_text(pincode_ta,"");
  }
}

void setup_interface()
{

  lock_screen = lv_obj_create(NULL, NULL);
  lv_scr_load(lock_screen);

  pincode_label = lv_label_create(lock_screen, NULL);
  lv_label_set_text(pincode_label, "Enter pin code to unlock");
  lv_obj_set_width(pincode_label, LV_HOR_RES);
  lv_obj_align(pincode_label, NULL, LV_ALIGN_IN_TOP_MID, 0, 10);

  pincode_ta = lv_ta_create(lock_screen, NULL);
  lv_ta_set_one_line(pincode_ta, true);
  lv_ta_set_pwd_mode(pincode_ta, true);
  lv_ta_set_text_align(pincode_ta, LV_LABEL_ALIGN_CENTER);
  lv_ta_set_accepted_chars(pincode_ta, "0123456789");
  lv_ta_set_max_length(pincode_ta, 6);
  lv_obj_set_width(pincode_ta, LV_HOR_RES * 0.5);
  lv_ta_set_text(pincode_ta, "");
  lv_ta_set_text_sel(pincode_ta, false);
  lv_obj_align(pincode_ta, pincode_label, LV_ALIGN_CENTER | LV_ALIGN_OUT_BOTTOM_MID, 0, 0);

  pincode_kb = lv_kb_create(lock_screen, NULL);
  lv_kb_set_map(pincode_kb, pincode_kb_map );
  lv_kb_set_ta(pincode_kb, pincode_ta);
  lv_obj_set_height(pincode_kb, LV_VER_RES * 0.5 );
  lv_obj_set_width(pincode_kb, LV_HOR_RES - 20 );
  lv_obj_align(pincode_kb, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -10);
  lv_obj_set_event_cb( pincode_kb, pincode_event_cb);

  main_screen = lv_obj_create(NULL, NULL);
  lv_obj_t * btn = lv_btn_create(main_screen, NULL);
  lv_obj_set_pos(btn, 10, 10);
  lv_obj_set_size(btn, 100, 50);
  lv_obj_set_event_cb(btn, btn_event_cb);

  lv_obj_t * label = lv_label_create(btn, NULL);
  lv_label_set_text(label, "Exit");

}


void main_loop() {
  while ( ! g_exit_requested ) {
    usleep(5000);
    if ( lv_disp_get_inactive_time( NULL ) > 5000 ) {
      // Lock screen
      lv_ta_set_text(pincode_ta,"");
      lv_scr_load(lock_screen);
    }
    lv_task_handler();
  };
}