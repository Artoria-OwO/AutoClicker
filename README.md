# Auto Clicker

A Windows auto-clicker built with C++ and Win32 API. Features a modern dark-themed GUI with Chinese/English bilingual support.

## Features

- **F5 global switch** — highest-priority global toggle; trigger key only responds after F5 is enabled
- **Separate trigger key and click key** — both support any keyboard or mouse button
- **Two trigger modes**
  - Toggle: press once to start, press again to stop
  - Hold: hold trigger key to click continuously, release to stop
- **Configurable interval** — set click interval in milliseconds with optional random jitter
- **Bilingual UI** — switch between Chinese and English at runtime

## Build

Requires Visual Studio 2022. Run:

```bat
build_gui.bat
```

Produces `autoclicker_gui.exe`.

## Usage

1. Run `autoclicker_gui.exe`
2. Click **Set** to assign a trigger key and a click key (cannot be F5)
3. Choose trigger mode (Toggle or Hold)
4. Set interval and jitter, then click **Apply**
5. Press **F5** to enable the global switch
6. Use the trigger key to start/stop auto-clicking

## Notes

- Mouse click keys click at the current cursor position
- Keyboard click keys send input to the current focused window
- If ineffective in protected applications, try running as administrator
