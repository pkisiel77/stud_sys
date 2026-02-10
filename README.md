# stud_sys - Real-Time EKG Measurement System

## Overview
This is a real-time EKG (ECG) measurement and data management system originally developed for Windows NT as a master's thesis. The system has been ported to support cross-platform execution using ncurses.

## Platform Support

### Windows Console (Original)
The original implementation uses Windows-specific console APIs:
- `windows.h` for console operations
- `MessageBox` for dialogs
- Windows GDI for text rendering

### ncurses (New - Linux/Unix)
The system now supports ncurses for cross-platform terminal control:
- Uses standard ncurses library
- Compatible with Linux, macOS, and other Unix-like systems
- Maintains the same user interface and functionality

## Building

### Prerequisites

#### For ncurses (Linux/Unix):
```bash
# Ubuntu/Debian
sudo apt-get install libncurses5-dev libncursesw5-dev

# Fedora/RHEL
sudo dnf install ncurses-devel

# macOS
brew install ncurses
```

#### For Windows:
Use Visual Studio or compatible Windows compiler with the project files (.vcxproj, .sln)

### Compilation

#### ncurses version (Linux/Unix):
```bash
make
```

Or manually:
```bash
gcc -D_NCURSES_ -o stud_sys core.c blank/term_ncurses.c blank/konsola.c \
    blank/wewy.c KONFIG.C RANDF.C RAP_BL.C -lncurses -lm
```

#### Windows version:
Open `stud_sys.sln` in Visual Studio and build.

### Running

#### ncurses version:
```bash
./stud_sys
```

#### Windows version:
Run the compiled executable from Visual Studio or command prompt.

## Architecture

### Terminal Abstraction Layer
The system uses a terminal abstraction layer that allows switching between platforms:

- **blank/moje.h** - Platform selection (define `_NCURSES_`, `_DOS_`, or `_QNX_`)
- **blank/term_ncurses.c/h** - ncurses implementation
- **blank/term_wint.c** - Windows implementation
- **blank/konsola.c** - Console initialization wrapper

### Key Functions Abstracted
- `term_type()` - Output text at position
- `term_printf()` - Formatted output
- `term_color()` - Set colors
- `GET_char()` - Get keyboard input
- `MessageBox()` - Dialog boxes (ncurses uses text-based dialogs)
- `sound()` / `nosound()` - Audio feedback

### Color Support
The system maps Windows color attributes to ncurses color pairs:
- 8 foreground colors × 8 background colors = 64 color pairs
- Supports attributes: bold (hilight), blink, reverse

### Keyboard Mapping
Function keys and special keys are mapped consistently:
- F1-F12 function keys
- Arrow keys (up, down, left, right)
- Page Up/Down, Home/End
- Insert, Delete
- Escape, Enter, Backspace, Tab

## Files Modified for ncurses Support

1. **blank/term_ncurses.h** - New: ncurses function declarations
2. **blank/term_ncurses.c** - New: ncurses implementation
3. **blank/moje.h** - Updated: Added `_NCURSES_` platform support
4. **blank/konsola.c** - Updated: Conditional compilation for ncurses
5. **Makefile** - New: Build system for ncurses

## Known Limitations

1. **Graphics Mode**: The original system has a graphics mode that is not fully supported in ncurses (limited to text-mode rendering)
2. **Sound**: ncurses uses the system beep instead of frequency-specific sounds
3. **Mouse**: Mouse support is basic in the ncurses version
4. **MessageBox**: Replaced with simple text-based dialog boxes

## Features

- Real-time task scheduler (agenda system)
- Service queue management  
- Interactive forms and data entry
- Report generation
- User authentication
- EKG signal measurement interface
- Database management

## Original System Information

- **Title**: System czasu rzeczywistego w srodowisku Windows NT (Real-time system in Windows NT environment)
- **Purpose**: Pomiar sygnału EKG (EKG signal measurement)
- **Author**: Paweł Kisielewicz
- **Supervisor**: Jan T. Duda
- **Institution**: Master's thesis project
- **Version**: 1.0
- **Date**: 29.10.2001

## License

MIT License - See LICENSE.txt for details

## Contact

Original: gonzo77@poczta.fm
