#include "lvgl/lvgl.h"
#include "lv_drivers/display/sunxifb.h"
#include "lv_drivers/indev/evdev.h"
#include "lv_examples/lv_examples.h"
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

#define DISP_BUF_SIZE (LV_HOR_RES_MAX * LV_VER_RES_MAX)

int main(void)
{
    /*LittlevGL init*/
    lv_init();
	
	sunxifb_init(0);
	
    /*Linux frame buffer device init*/
   // fbdev_init();
	
	static uint32_t width, height;
    sunxifb_get_sizes(&width, &height);

    /*A small buffer for LittlevGL to draw the screen's content*/
    static lv_color_t buf[DISP_BUF_SIZE];
    static lv_color_t buf1x[DISP_BUF_SIZE];
    /*Initialize a descriptor for the buffer*/
    static lv_disp_buf_t disp_buf;
	
    lv_disp_buf_init(&disp_buf, buf, buf1x, DISP_BUF_SIZE);



    /*Initialize and register a display driver*/
    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.buffer   = &disp_buf;
    disp_drv.flush_cb = sunxifb_flush;
    lv_disp_drv_register(&disp_drv);

    evdev_init();

    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);     
    indev_drv.type =LV_INDEV_TYPE_POINTER;               
    indev_drv.read_cb =evdev_read;             
    lv_indev_drv_register(&indev_drv);

    /*Create a Demo*/
    //lv_demo_widgets();
    lv_my_widget(1);

    /*Handle LitlevGL tasks (tickless mode)*/
    while(1) {
        lv_task_handler();
        usleep(5000);
    }

    return 0;
}

/*Set in lv_conf.h as `LV_TICK_CUSTOM_SYS_TIME_EXPR`*/
uint32_t custom_tick_get(void)
{
    static uint64_t start_ms = 0;
    if(start_ms == 0) {
        struct timeval tv_start;
        gettimeofday(&tv_start, NULL);
        start_ms = (tv_start.tv_sec * 1000000 + tv_start.tv_usec) / 1000;
    }

    struct timeval tv_now;
    gettimeofday(&tv_now, NULL);
    uint64_t now_ms;
    now_ms = (tv_now.tv_sec * 1000000 + tv_now.tv_usec) / 1000;

    uint32_t time_ms = now_ms - start_ms;
    return time_ms;
}
