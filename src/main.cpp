#include <Arduino.h>
#include "config.h"
#include <lvgl.h>
#include <memory>
#include "apps/AvatarApp.h"

// ---------------------------------------------------------
// Multithreaded UI Environment
// ---------------------------------------------------------
static std::unique_ptr<AvatarApp> myAvatar;
static SemaphoreHandle_t xGuiSemaphore;

TaskHandle_t TaskSensor;

// ---------------------------------------------------------
// Background Sensor Task (Core 0)
// ---------------------------------------------------------
void TaskSensorCode(void* pvParameters) {
    float angle = 0.0f;
    for(;;) {
        vTaskDelay(pdMS_TO_TICKS(40)); // 25 FPS update rate

        // Simulate wandering coordinates (Simulated "Face Detection")
        angle += 0.05f;
        float sim_x = sin(angle * 0.8f);
        float sim_y = cos(angle * 1.5f);

        // UI-UPDATE RULE: Try to take Mutex lock before touching LVGL!
        if (xSemaphoreTake(xGuiSemaphore, (TickType_t)10) == pdTRUE) {
            if (myAvatar) {
                myAvatar->updateGaze(sim_x, sim_y);
            }
            xSemaphoreGive(xGuiSemaphore);
        }
    }
}

// ---------------------------------------------------------
// Core 1 Main Setup
// ---------------------------------------------------------
void setup() {
    Serial.begin(Config::SERIAL_BAUD_RATE);
    delay(500); 

    Serial.println("\n--- M5Stack Tab5 App Launcher Booting ---");

    // 1. Setup UI System & Mutex
    xGuiSemaphore = xSemaphoreCreateMutex();
    
    // Hardware LVGL Initialization (Mock target for compile checks)
    lv_init(); 
    // TODO: Connect logical M5.Display flush callbacks to LVGL here

    // 2. Initialize the App
    if (xSemaphoreTake(xGuiSemaphore, (TickType_t)10) == pdTRUE) {
        myAvatar = std::make_unique<AvatarApp>();
        myAvatar->show(lv_scr_act()); // Render on main screen
        xSemaphoreGive(xGuiSemaphore);
    }

    // 3. Start Core 0 Sensor Task
    xTaskCreatePinnedToCore(
        TaskSensorCode,   // Task function
        "TaskSensor",     // Name
        4000,             // Stack size
        NULL,             // Parameter
        1,                // Priority
        &TaskSensor,      // Handle
        0                 // Run on Core 0
    );

    Serial.println("[INFO] System setup complete. Eyes are running autonomously.");
}

void loop() {
    // 4. FreeRTOS UI Drawing Loop
    if (xSemaphoreTake(xGuiSemaphore, (TickType_t)10) == pdTRUE) {
        lv_timer_handler(); // Engine paints the screen if needed
        xSemaphoreGive(xGuiSemaphore);
    }
    delay(5); // Yield logic
}
