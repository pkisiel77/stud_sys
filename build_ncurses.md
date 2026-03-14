# Building stud_sys with NCURSES Support

## Overview

The stud_sys EKG measurement system has been ported to support cross-platform execution using the ncurses library. This document explains how to enable and build the NCURSES mode.

## Prerequisites

### Install ncurses Development Libraries

#### Ubuntu/Debian:
```bash
sudo apt-get install libncurses5-dev libncursesw5-dev
```

#### Fedora/RHEL:
```bash
sudo dnf install ncurses-devel
```

#### macOS:
```bash
brew install ncurses
```

## Building with NCURSES

### Method 1: Using Makefile (Recommended)

The NCURSES mode is **already enabled by default** in the Makefile:

```bash
make clean
make
```

The Makefile automatically:
- Defines `-D_NCURSES_` compiler flag
- Links against `-lncurses`
- Compiles all source files with NCURSES support

### Method 2: Manual Compilation

If you need to compile manually:

```bash
gcc -D_NCURSES_ -I. -Iblank -Iopcjesys \
    -o stud_sys \
    core.c \
    blank/term_ncurses.c \
    blank/konsola.c \
    blank/wewy.c \
    budz.c \
    admin.c \
    sys_rap.c \
    sprawdz.c \
    mqtt_pub.c \
    sensor_sim.c \
    -lncurses -lm
```

## How NCURSES Mode Works

### Compilation Flag

The `_NCURSES_` preprocessor macro enables ncurses support throughout the codebase:

- **blank/moje.h**: Selects ncurses platform
- **blank/term_ncurses.c/h**: Provides ncurses implementations
- Platform-specific code (Windows, DOS, QNX) is conditionally excluded

### Key Components

1. **Terminal Abstraction Layer**:
   - `term_type()` - Text output
   - `term_printf()` - Formatted output  
   - `term_color()` - Color control
   - `GET_char()` - Keyboard input
   - `setcursor()` - Cursor control

2. **Color Support**:
   - 64 color pairs (8 foreground × 8 background)
   - Supports bold, blink, and reverse attributes

3. **Keyboard Mapping**:
   - Function keys (F1-F12)
   - Arrow keys
   - Special keys (Page Up/Down, Home/End, etc.)

## Platform Detection

The system automatically detects the platform:

```c
/* Platform selection - define one of: _NCURSES_, _DOS_, _QNX_ */
#ifndef _NCURSES_
#ifndef _QNX_
#define _DOS_
#endif
#endif
```

When `-D_NCURSES_` is passed to the compiler, it overrides the default DOS selection.

## Features and Limitations

### Supported Features:
- ✅ Full terminal control via ncurses
- ✅ Color text rendering
- ✅ Keyboard input with function keys
- ✅ Cursor control
- ✅ System beep (audio feedback)
- ✅ Text-based dialogs

### Known Limitations:
- ❌ Graphics mode (text-mode only)
- ❌ Frequency-specific sounds (uses system beep)
- ❌ Advanced mouse support (basic only)
- ❌ Windows-specific file dialogs (disabled)

## Troubleshooting

### Missing ncurses Library

**Error**: `fatal error: ncurses.h: No such file or directory`

**Solution**: Install ncurses development packages (see Prerequisites above)

### Undefined Reference to ncurses Functions

**Error**: `undefined reference to 'initscr'`

**Solution**: Ensure `-lncurses` is in the link flags

### Case-Sensitivity Issues

**Error**: `fatal error: shintdef.h: No such file or directory`

**Solution**: All includes have been fixed to use correct case (shintdef.h). If you encounter new ones, check the actual filename case with `ls -la`.

## Running the Application

After successful compilation:

```bash
./stud_sys
```

The application will run in ncurses terminal mode with full keyboard and display support.

## Development Notes

### Adding New Code

When adding new code, ensure Windows-specific APIs are conditionally compiled:

```c
#ifndef _NCURSES_
    // Windows-specific code
    MessageBox(NULL, "Error", "Title", MB_OK);
#else
    // NCURSES alternative
    fprintf(stderr, "Error: ...\n");
#endif
```

### Testing

Test the build process:

```bash
make clean
make
./stud_sys
```

Verify that:
1. All source files compile without errors
2. Linking succeeds
3. The application starts in ncurses mode
4. Keyboard input works (arrow keys, function keys)
5. Text displays with proper colors

## Additional Resources

- **README.md**: General project documentation
- **NCURSES_PORT.md**: Detailed ncurses port implementation
- **blank/term_ncurses.c**: NCURSES terminal implementation
- **blank/term_ncurses.h**: NCURSES function prototypes

## Support

For issues or questions about the NCURSES port, refer to the main README.md or contact the maintainers.
