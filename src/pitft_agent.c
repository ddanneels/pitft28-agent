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

/*
#include <ft2build.h>
#include FT_FREETYPE_H

FT_Library  library;
*/


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
        usleep(3*1000);
        lv_tick_inc(3);
    }
}


void btn_event_cb(lv_obj_t * btn, lv_event_t event)
{
    if(event == LV_EVENT_CLICKED) {
        printf("Clicked\n");
        g_exit_requested = true;
    }
}



int main(int argc, char* argv[]) {

  char*                touchscreen_path;
  char*                framebuffer_path;

thrd_t thr_touchscreen;
thrd_t thr_tick;

// struct framebuffer*  fb;
/*  FT_Error             e;
  FT_Face              face;
  FT_UInt              glyph_index;
char32_t ch[1];
mbstate_t ps;*/

lv_init();


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

  // allright so far

/*
e = FT_Init_FreeType( &library );
if ( e ) {
  fprintf(stderr, "Unable to initialize Freetype library : %d\n", e);
}

e = FT_New_Face( library,
                     "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf",
                     0,
                     &face );
if ( e ) {
  fprintf(stderr, "Unable to load font : %d\n", e);
}

e = FT_Set_Char_Size(
          face,
          0,
          24*64,
          72,
          72 ); 
if ( e ) {
  fprintf(stderr, "Unable to set char size : %d\n", e);
}

mbsinit( &ps );
mbrtoc32( ch, "Aqc-%", 1, &ps );
printf("Printing code %d\n", ch[0]);
glyph_index = FT_Get_Char_Index( face, ch[0] );
e = FT_Load_Glyph( face, glyph_index, FT_LOAD_DEFAULT );
if ( e ) {
  fprintf(stderr, "Unable to load glyph : %d\n", e);
}
e = FT_Render_Glyph( face->glyph, FT_RENDER_MODE_NORMAL );

*/



  // Listen for Ctrl-C
  signal(SIGINT, sigterm_handler );

  if ( open_framebuffer( framebuffer_path ) != 0 ) {
    return 1;
  };

  Fill( 0, 0, fb->xres-1, fb->yres-1, rgb(0, 0, 0xc0));


printf("sizeof lv_color_t : %d\n", sizeof(lv_color_t));

buf = malloc( fb->xres * fb->yres * sizeof(lv_color_t));
lv_disp_buf_init(&disp_buf, buf, NULL, fb->xres * fb->yres); 

lv_disp_drv_t disp_drv;               /*Descriptor of a display driver*/
lv_disp_drv_init(&disp_drv);          /*Basic initialization*/
disp_drv.flush_cb = my_disp_flush;    /*Set your driver function*/
disp_drv.buffer = &disp_buf;          /*Assign the buffer to the display*/
    /*Set the resolution of the display*/
    disp_drv.hor_res = 320;
    disp_drv.ver_res = 240;

lv_disp_drv_register(&disp_drv);    

lv_indev_drv_t indev_drv;                  /*Descriptor of a input device driver*/
lv_indev_drv_init(&indev_drv);             /*Basic initialization*/
indev_drv.type = LV_INDEV_TYPE_POINTER;    /*Touch pad is a pointer-like device*/
indev_drv.read_cb = my_touchpad_read;      /*Set your driver function*/
lv_indev_drv_register(&indev_drv);         /*Finally register the driver*/



/*
printf(" Glyph: %dx%d\n", face->glyph->bitmap.width, face->glyph->bitmap.rows );

  for ( unsigned int x = 0 ; x < face->glyph->bitmap.width ; x++ ) {
    for ( unsigned int y = 0 ; y < face->glyph->bitmap.rows ; y++ ) {
      unsigned char c = face->glyph->bitmap.buffer[ y*face->glyph->bitmap.width + x ];

      SetPixel( fb, 100 + x, 100 + y, rgb( c, c, c));
    }
  }
*/


lv_obj_t * btn = lv_btn_create(lv_scr_act(), NULL);     /*Add a button the current screen*/
lv_obj_set_pos(btn, 10, 10);                            /*Set its position*/
lv_obj_set_size(btn, 100, 50);                          /*Set its size*/
lv_obj_set_event_cb(btn, btn_event_cb);                 /*Assign a callback to the button*/

lv_obj_t * label = lv_label_create(btn, NULL);          /*Add a label to the button*/
lv_label_set_text(label, "Button");                     /*Set the labels text*/




thrd_create( &thr_touchscreen, touchscreen_listener, (void*) touchscreen_path );
thrd_create( &thr_tick, tick_thread, NULL);

      while ( ! g_exit_requested ) {
        usleep(3000);
        lv_task_handler();
      };

thrd_join( thr_touchscreen, NULL );
thrd_join( thr_tick, NULL);

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



