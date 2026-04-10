/**
 * LVGL Configuration for M5Stack Tab5 (ESP32-P4)
 *
 * Best Practice: Booleans via #define 1/0, not via macros.
 * This file is required by the LVGL build system.
 * Place it in the /include/ folder and add -D LV_CONF_INCLUDE_SIMPLE to platformio.ini.
 */

#if 1 /* Guard against accidental inclusion */

#ifndef LV_CONF_H
#define LV_CONF_H

#include <stdint.h>

/* === Memory (Pillar 2: Output / PSRAM) === */
/* Use external malloc() so ESP-IDF can draw from the PSRAM */
#define LV_MEM_CUSTOM 1
#if LV_MEM_CUSTOM == 0
    #define LV_MEM_SIZE (256 * 1024U) /* Fallback internal RAM if PSRAM not in use */
#else
    #include <stdlib.h>
    #define LV_MEM_CUSTOM_INCLUDE <stdlib.h>
    #define LV_MEM_CUSTOM_ALLOC   malloc
    #define LV_MEM_CUSTOM_FREE    free
    #define LV_MEM_CUSTOM_REALLOC realloc
#endif

/* === Tick (Pillar 4: Lifecycle) === */
/* Bind LVGL animation clock to Arduino millis() */
#define LV_TICK_CUSTOM 1
#if LV_TICK_CUSTOM
    #define LV_TICK_CUSTOM_INCLUDE "Arduino.h"
    #define LV_TICK_CUSTOM_SYS_TIME_EXPR (millis())
#endif

/* === Display Resolution === */
#define LV_HOR_RES_MAX 1280
#define LV_VER_RES_MAX 800

/* === Color Depth === */
#define LV_COLOR_DEPTH 16

/* === Performance Monitor (useful during dev) === */
#define LV_USE_PERF_MONITOR 1
#define LV_USE_MEM_MONITOR  1

/* === Font Support === */
#define LV_FONT_MONTSERRAT_14 1
#define LV_FONT_MONTSERRAT_20 1
#define LV_FONT_DEFAULT &lv_font_montserrat_14

/* === Widgets === */
#define LV_USE_ARC      1
#define LV_USE_BTN      1
#define LV_USE_LABEL    1
#define LV_USE_IMG      1
#define LV_USE_LINE     1
#define LV_USE_ROLLER   1
#define LV_USE_SLIDER   1
#define LV_USE_SWITCH   1

/* === Logging (for Serial debug output) === */
#define LV_USE_LOG      1
#define LV_LOG_LEVEL    LV_LOG_LEVEL_WARN
#define LV_LOG_PRINTF   1

#endif /* LV_CONF_H */
#endif /* End of "Content enable" */
