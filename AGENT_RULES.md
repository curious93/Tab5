# Agent Instructions: M5Stack Tab5 Launcher

You are operating in the **M5Stack Tab5 (ESP32-P4)** project workspace. 
Your core task is to assist in developing a central "App-Launcher" with various modular Apps (e.g., settings, weather, sensors, mini-games) that can be started from a central GUI.

Follow these strict rules for every interaction and code change:

## 1. Safety & Git ("Go-Lock" Protocol)
* **Always use Git:** You must commit your changes frequently. Use the configured author email `vonbrueck@gmail.com`.
* **Micro-Commits / Go-Lock:** Before making any large code changes, refactoring, or attempting to fix a build error, ensure the current working state is committed. If your new changes break the build and you cannot trivially fix it in one attempt, you MUST revert to the last working commit and reconsider the approach rather than wildly guessing and breaking more things.

## 2. Framework & Tech Stack
* **Environment:** PlatformIO + Arduino Core (leveraging the `m5stack-tab5` BSP for basic hardware access).
* **Graphics:** LVGL (Light and Versatile Graphics Library).

## 3. Architecture Rules (Anti-Spaghetti)
* **Separation of Concerns:** Never mix UI rendering logic (LVGL widgets, screens) with Business Logic (sensor reading, Wi-Fi requests, I2C logic) in the same files or functions. Hardware and Network logic must be extracted into separate classes or modules.
* **Central Configuration:** No magic numbers in code. All Pins, I2C addresses, colors, and timer intervals must be defined in a central `config.h` file.
* **App Modularity:** Every App must inherit from a central base class (e.g., `Tab5App`). Apps must implement isolated lifecycle methods (`onStart()`, `onLoop()`, `onStop()`).
* **Memory Management:** Memory leaks on ESP32 UI projects cause immediate crashes. When the Launcher switches to a new App or an App is closed, you MUST guarantee that all LVGL objects and allocated memory belonging to the closing context are destroyed and freed properly.

## 4. Operational Workflow
1. Analyze the user request and identify required changes.
2. Ensure the working state is committed (`git status` & `git commit`).
3. Make targeted changes following the architectural rules above.
4. Compile/Test using `pio run` (if applicable and PlatformIO is set up).
5. Commit successful changes immediately before pursuing the next task.
