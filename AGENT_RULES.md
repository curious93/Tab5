# Agent Instructions: M5Stack Tab5 Launcher

You are operating in the **M5Stack Tab5 (ESP32-P4)** project workspace. 
Your core task is to assist in developing a central "App-Launcher" with various modular Apps that can be started from a central GUI.

Follow these primary rules for every interaction and code change:

## 1. Safety & Git ("Go-Lock" Protocol)
* **Always use Git:** You must commit your changes frequently. Use the configured author email `vonbrueck@gmail.com`.
* **Micro-Commits / Go-Lock:** Before making any large code changes, refactoring, or attempting to fix a build error, ensure the current working state is committed. If your new changes break the build, you MUST revert to the last working commit and reconsider the approach.

## 2. Architecture & Documentation
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
