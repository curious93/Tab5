# Agent Instructions: M5Stack Tab5 Launcher

You are operating in the **M5Stack Tab5 (ESP32-P4)** project workspace. 
Your core task is to assist in developing a central "App-Launcher" with various modular Apps that can be started from a central GUI.

Follow these primary rules for every interaction and code change:

## 1. Safety & Git ("Go-Lock" Protocol)
* **Always use Git:** You must commit your changes frequently. Use the configured author email `vonbrueck@gmail.com`.
* **Micro-Commits / Go-Lock:** Before making any large code changes, refactoring, or attempting to fix a build error, ensure the current working state is committed. If your new changes break the build, you MUST revert to the last working commit and reconsider the approach.

## 2. Code Quality & Best Practices
* **Modern C++ / Clean Code:** Always write idiomatic, modern C++. Adhere strictly to embedded best practices to prevent memory leaks and undefined behavior:
  * Use **RAII** and smart pointers (`std::unique_ptr`, `std::shared_ptr`) whenever possible instead of raw `new`/`delete`.
  * Enforce **Const Correctness** (`const` values, `const` methods).
  * Avoid global variables; use Dependency Injection or Singletons sparingly and intentionally.

## 3. Architecture & Documentation
Knowledge regarding specific domains has been extracted to dedicated documents. 
**You MUST read the relevant documentation located in the `docs/` folder BEFORE modifying those domains.**

* **Graphical User Interface (LVGL, SquareLine, Memory):** -> Read `docs/UI_ARCHITECTURE.md`

## 3. General Hardware Rules
* **Central Configuration:** No magic numbers in code. All Pins, I2C addresses, colors, and timer intervals must be defined in a central `config.h` file.
* **Separation of Concerns:** Hardware and Network logic must be extracted into separate classes or modules and never placed directly inside UI tasks.

## 4. Operational Workflow
1. Analyze user request and identify required domains.
2. Read the relevant `/docs/...` file if applicable.
3. Ensure working state is committed before coding.
4. Execute changes, compile/test (`pio run`), and commit.

## 5. Communication & Action Protocol (Read-Only on Questions)
* **Question == No Action:** If the user asks a question, asks for an explanation of a bug, or inquires about an architecture detail, you MUST act in **Read-Only Mode**. 
* You are strictly FORBIDDEN to modify any files, run build commands, or commit code when answering a question. 
* Only modify code if the user explicitly instructs you to execute a change (e.g., "Fix it", "Implement this", "Proceed").

## 6. Human-AI Collaboration Protocol
* **Language Split:** All code, function names, and inline comments MUST be written in **English**. All discussions, architectural planning, and chat interactions MUST be conducted in **German**.
* **Planning Checkpoint:** Before implementing any large feature or taking actions that span multiple files, the Agent MUST propose an implementation plan (via markdown) for the user to review.
* **Error Verification:** If a build or task fails, never guess the solution. Rely heavily on the exact terminal logs provided by the user or extracted from `pio run`.

## 7. Hardware Implementation Rule (Pre-Flight Checklist)
* **Zero-Trust Build:** Before presenting ANY implementation plan that involves UI, Sensors, or System Logic on an embedded device, the Agent MUST systematically verify and document the following 4 pillars hardware-abstraction:
  1. **Init:** Is the hardware core explicitly initialized (e.g., `M5.begin()`)?
  2. **Output:** Is the display buffer mapped correctly? Is the PSRAM compiler flag (`-D BOARD_HAS_PSRAM` or equivalent) explicitly defined in `platformio.ini` to prevent out-of-memory crashes?
  3. **Input:** Is a physical input driver (e.g., Touch/Buttons via `lv_indev_drv_t`) hooked up to the UI? The display cannot be a dead canvas.
  4. **Lifecycle:** Are the framework timers (`lv_tick_inc()`, `lv_timer_handler()`) properly synchronized with the hardware tick/main loop (`M5.update()`) without congesting the FreeRTOS heap?
* **Exception:** Do not assume these elements "can be added later". A plan without them is structurally defective and forbidden.
