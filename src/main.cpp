#include <Arduino.h>
#include "config.h"

// ---------------------------------------------------------
// Tab5 App Launcher Boot Sequence
// ---------------------------------------------------------

void setup() {
    // 1. Initialize System Logging
    Serial.begin(Config::SERIAL_BAUD_RATE);
    delay(500); 

    Serial.println("\n--- M5Stack Tab5 App Launcher Booting ---");
    Serial.println("[INFO] Loading Hardware Configuration...");

    // 2. Initialize Hardware (BSP / Display)
    // TODO: M5.begin() or specific Tab5 Board Support Package init here

    // 3. Initialize LVGL & Start UI Task
    // TODO: Start FreeRTOS task for lv_task_handler()
    
    Serial.println("[INFO] Boot sequence completed.");
}

void loop() {
    // The main Arduino loop (Core 1 or 0 depending on framework)
    // Best Practice: Keep this non-blocking. 
    // Business logic runs here or in dedicated FreeRTOS tasks.
    
    delay(10); 
}
