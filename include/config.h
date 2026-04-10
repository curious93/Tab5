#pragma once

/*
 * M5Stack Tab5 (ESP32-P4) Central Configuration
 * All Magic Numbers, Pins, and Constants belong here.
 * This ensures "Const Correctness" and single-source-of-truth.
 */
namespace Config {
    
    // ----------------------
    // Serial & Communication
    // ----------------------
    constexpr int SERIAL_BAUD_RATE = 115200;

    // ----------------------
    // Display / UI Settings
    // ----------------------
    // Note: Tab5 exact display resolution (approx 7" 1280x800, adjust if needed)
    constexpr int DISPLAY_WIDTH = 1280;
    constexpr int DISPLAY_HEIGHT = 800;
    
    // ----------------------
    // Hardware Pins
    // ----------------------
    // TODO: Define I/O pins for I2C, SPI, Buttons as they are added

} // namespace Config
