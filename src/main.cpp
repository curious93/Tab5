#include <Arduino.h>
// NOTE: Using M5GFX directly (M5Unified has open I2S bug on ESP32-P4 as of 2025)
#include <M5GFX.h>
#include <lvgl.h>
#include <memory>
#include "apps/AvatarApp.h"
#include "config.h"

// Global display driver (M5GFX handles Panel + Touch for Tab5)
static M5GFX gDisplay;

// ---------------------------------------------------------
// Pre-Flight Checklist State
// Pillar 1 (Init)     : gDisplay.begin() + setRotation   -> setup()
// Pillar 2 (Output)   : Double PSRAM draw buffers         -> lvgl_display_init()
// Pillar 3 (Input)    : Touch driver callback             -> lvgl_touch_init()
// Pillar 4 (Lifecycle): lv_tick_inc + lv_timer_handler    -> TickTask + loop()
// ---------------------------------------------------------

static std::unique_ptr<AvatarApp> gAvatar;
static SemaphoreHandle_t           gGuiMutex;
static TaskHandle_t                gSensorTask;
// NOTE: No gTickTask needed — LV_TICK_CUSTOM=1 uses millis() automatically

// Double buffering: two halves of the screen in PSRAM to eliminate tearing
static lv_disp_draw_buf_t gDrawBuf;
static lv_color_t*        gBuf1 = nullptr;
static lv_color_t*        gBuf2 = nullptr;

// ---------------------------------------------------------
// Pillar 2: Output — Display flush callback
// LVGL calls this when a region of the screen needs to be painted.
// ---------------------------------------------------------
static void disp_flush_cb(lv_disp_drv_t* drv, const lv_area_t* area, lv_color_t* color_p) {
    const int32_t w = area->x2 - area->x1 + 1;
    const int32_t h = area->y2 - area->y1 + 1;
    gDisplay.startWrite();
    gDisplay.setAddrWindow(area->x1, area->y1, w, h);
    gDisplay.writePixels((lgfx::rgb565_t*)color_p, w * h);
    gDisplay.endWrite();
    lv_disp_flush_ready(drv);
}

// ---------------------------------------------------------
// Pillar 3: Input — Touch read callback
// LVGL calls this each tick to ask: "Is anyone touching the screen?"
// ---------------------------------------------------------
static void touch_read_cb(lv_indev_drv_t* drv, lv_indev_data_t* data) {
    uint16_t tx = 0, ty = 0;
    if (gDisplay.getTouch(&tx, &ty)) {
        data->point.x = static_cast<lv_coord_t>(tx);
        data->point.y = static_cast<lv_coord_t>(ty);
        data->state   = LV_INDEV_STATE_PR;
    } else {
        data->state = LV_INDEV_STATE_REL;
    }
}

// ---------------------------------------------------------
// Pillar 2: LVGL display driver registration
// FIX: Uses runtime gDisplay.width()/height() AFTER setRotation()
//      to guarantee LVGL and hardware agree on the screen dimensions.
// FIX: Double buffering (gBuf1 + gBuf2) eliminates screen tearing.
// ---------------------------------------------------------
static void lvgl_display_init() {
    const int32_t dispW = gDisplay.width();
    const int32_t dispH = gDisplay.height();
    const size_t  BUF_PIXELS = static_cast<size_t>(dispW) * (static_cast<size_t>(dispH) / 10);

    gBuf1 = (lv_color_t*)heap_caps_malloc(BUF_PIXELS * sizeof(lv_color_t), MALLOC_CAP_SPIRAM);
    gBuf2 = (lv_color_t*)heap_caps_malloc(BUF_PIXELS * sizeof(lv_color_t), MALLOC_CAP_SPIRAM);

    if (!gBuf1 || !gBuf2) {
        Serial.println("[FATAL] Could not allocate LVGL draw buffers in PSRAM!");
        return;
    }
    // Double buffer: LVGL alternates between buf1 and buf2 to prevent tearing
    lv_disp_draw_buf_init(&gDrawBuf, gBuf1, gBuf2, BUF_PIXELS);

    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    // FIX: Read resolution directly from M5GFX after rotation is applied
    disp_drv.hor_res  = static_cast<lv_coord_t>(dispW);
    disp_drv.ver_res  = static_cast<lv_coord_t>(dispH);
    disp_drv.flush_cb = disp_flush_cb;
    disp_drv.draw_buf = &gDrawBuf;
    lv_disp_drv_register(&disp_drv);
    Serial.printf("[INFO] LVGL Display: %dx%d\n", dispW, dispH);
}

// ---------------------------------------------------------
// Pillar 3: LVGL touch driver registration
// ---------------------------------------------------------
static void lvgl_touch_init() {
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type    = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = touch_read_cb;
    lv_indev_drv_register(&indev_drv);
}

// ---------------------------------------------------------
// Pillar 4: Lifecycle — Sensor / Gaze Simulation Task (Core 0)
// Simulates face-tracking coordinates until real camera is active.
// FIX: Reduced speed (angle += 0.01f) for smooth, natural movement.
// NOTE: No separate Tick task needed — LV_TICK_CUSTOM=1 pulls time from millis().
// ---------------------------------------------------------
static void SensorTaskCode(void* pvParameters) {
    float angle = 0.0f;
    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(33)); // ~30 fps update rate
        angle += 0.01f;  // FIX: was 0.05f — slowed down significantly
        const float sim_x = sinf(angle * 0.7f);
        const float sim_y = cosf(angle * 1.1f);

        if (xSemaphoreTake(gGuiMutex, pdMS_TO_TICKS(10)) == pdTRUE) {
            if (gAvatar) gAvatar->updateGaze(sim_x, sim_y);
            xSemaphoreGive(gGuiMutex);
        }
    }
}

// ---------------------------------------------------------
// Pillar 1: Init — Setup (runs once on Core 1)
// ---------------------------------------------------------
void setup() {
    Serial.begin(Config::SERIAL_BAUD_RATE);
    delay(200);
    Serial.println("\n--- M5Stack Tab5 App Launcher Booting ---");

    // 1. Hardware Init (Pillar 1)
    gDisplay.begin();

    // FIX: Explicitly set landscape orientation BEFORE LVGL reads the resolution.
    // Rotation 1 = 90° = 1280x800 landscape on the Tab5 panel.
    gDisplay.setRotation(1);
    gDisplay.setBrightness(200);
    gDisplay.fillScreen(TFT_BLACK); // Clear any boot artefacts

    // 2. LVGL core + hardware drivers (Pillars 2 & 3)
    lv_init();
    lvgl_display_init();
    lvgl_touch_init();

    // 3. GUI Mutex
    gGuiMutex = xSemaphoreCreateMutex();

    // 4. Launch Avatar App
    if (xSemaphoreTake(gGuiMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
        gAvatar = std::make_unique<AvatarApp>();
        gAvatar->show(lv_scr_act());
        xSemaphoreGive(gGuiMutex);
    }

    // 5. Start sensor simulation on Core 0 (no separate tick task needed)
    xTaskCreatePinnedToCore(SensorTaskCode, "SensorTask", 4096, nullptr, 1, &gSensorTask, 0);

    Serial.println("[INFO] Boot complete. Avatar is live.");
}

// ---------------------------------------------------------
// Pillar 4: Lifecycle — Main loop (Core 1)
// ---------------------------------------------------------
void loop() {
    if (xSemaphoreTake(gGuiMutex, pdMS_TO_TICKS(10)) == pdTRUE) {
        lv_timer_handler(); // Render dirty UI regions to screen
        xSemaphoreGive(gGuiMutex);
    }
    delay(5);
}
