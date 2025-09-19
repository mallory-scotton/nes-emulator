# 🎮 NES Emulator - Nintendo Entertainment System Emulator

A high-performance, feature-complete Nintendo Entertainment System (NES) emulator built with modern C++20 and SFML. This project implements cycle-accurate emulation of the NES console, including CPU, PPU, APU, and comprehensive memory mapper support for authentic retro gaming experiences.

## 🎯 Project Overview

This NES emulator is a sophisticated system-level emulation project that recreates the complete Nintendo Entertainment System hardware stack. Built with performance and accuracy in mind, it features cycle-accurate timing, authentic audio synthesis, and pixel-perfect graphics rendering with optional CRT shader effects.

**Language**: C++20  
**Graphics Library**: SFML (Simple and Fast Multimedia Library)  
**Build System**: GNU Make  
**Architecture**: Modular component-based design with hardware abstraction

## ✨ Features

### Core Emulation Components
- **6502 CPU Emulation**: Full cycle-accurate 6502 processor implementation with all documented opcodes
- **Picture Processing Unit (PPU)**: Complete PPU emulation with sprite rendering, background tiles, and scrolling
- **Audio Processing Unit (APU)**: Authentic audio synthesis with all 5 channels:
  - 2x Pulse Wave Channels (Square waves)
  - 1x Triangle Wave Channel
  - 1x Noise Channel
  - 1x Delta Modulation Channel (DMC)
- **Memory Management**: Sophisticated memory mapping system with multiple mapper implementations
- **Controller Support**: Authentic NES controller input handling with customizable key bindings

### Supported Memory Mappers
- **NROM (Mapper 0)**: Basic mapper for simple games like Super Mario Bros.
- **MMC1 (Mapper 1)**: Bank switching mapper supporting larger games
- **UxROM (Mapper 2)**: UNROM and UOROM cartridge support
- **CNROM (Mapper 3)**: Character ROM switching for graphics-heavy games
- **MMC3 (Mapper 4)**: Advanced mapper with IRQ support for complex games
- **AxROM (Mapper 7)**: 32KB program ROM switching with single-screen mirroring
- **GxROM (Mapper 66)**: Dual program and character ROM switching

### Visual & Audio Features
- **Authentic Resolution**: Native 256x240 pixel rendering scaled to 768x720
- **CRT Shader Effects**: Optional retro CRT monitor simulation with:
  - Scanline simulation
  - Phosphor glow effects
  - Screen curvature distortion
  - RGB shadow mask
- **Real-time Audio**: High-quality audio output with proper channel mixing
- **Frame-perfect Timing**: 60 FPS gameplay with authentic NES timing

### Advanced Features
- **ROM Compatibility**: Support for .nes ROM files with iNES header format
- **Pause/Resume**: Real-time emulation control (F2 to toggle)
- **Single-step Debugging**: Frame-by-frame execution for development (F3)
- **Shader Toggle**: Enable/disable CRT effects on-the-fly (F1)
- **Cartridge RAM**: Support for games with battery-backed SRAM

## 🖼️ Gallery

### Main Emulation Window
<video src="https://github.com/user-attachments/assets/2668c0b8-fb32-4f3a-8c4f-e11c72f53b4a"></video>
*Real-time NES game emulation with authentic pixel-perfect rendering*

### CRT Shader Effects
<video src="https://github.com/user-attachments/assets/1fc22425-9565-4eff-a97c-cffd965f89b6"></video>
*Optional CRT monitor simulation with scanlines and phosphor effects*

### ROM Compatibility
<video src="https://github.com/user-attachments/assets/25849609-998a-4846-a6a3-06e563c13f4b"></video>
*Wide compatibility with popular NES titles*

## 📦 Installation

### Prerequisites
- **C++ Compiler**: GCC or Clang with C++20 support
- **SFML Libraries**: Complete SFML development libraries
  - libsfml-system
  - libsfml-window  
  - libsfml-graphics
  - libsfml-audio
- **Build Tools**: GNU Make
- **System Libraries**: pthread, math library (libm)

### Platform-Specific Installation

#### Ubuntu/Debian
```bash
sudo apt update
sudo apt install build-essential libsfml-dev
```

#### Fedora/RHEL
```bash
sudo dnf install gcc-c++ make SFML-devel
```

#### Arch Linux
```bash
sudo pacman -S gcc make sfml
```

### Quick Start
```bash
# Clone the repository
git clone https://github.com/mallory-scotton/nes-emulator.git
cd nes-emulator

# Build the emulator
make

# Run with a ROM file
./nes path/to/your/rom.nes
```

## 🚀 Usage

### Command Line Interface
```bash
./nes <ROM_FILE>
```

### Interactive Controls

#### System Controls
| Key | Action |
|-----|--------|
| `ESC` | Exit emulator |
| `F1` | Toggle CRT shader effects |
| `F2` | Pause/Resume emulation |
| `F3` | Step one frame (when paused) |

#### NES Controller Mapping
| NES Button | Keyboard Key |
|------------|--------------|
| **A** | `A` |
| **B** | `Z` |
| **SELECT** | `Right Shift` |
| **START** | `Enter` |
| **UP** | `↑` |
| **DOWN** | `↓` |
| **LEFT** | `←` |
| **RIGHT** | `→` |

### ROM Compatibility
The emulator supports standard iNES format ROM files (.nes extension) and is compatible with:
- Super Mario Bros. series
- The Legend of Zelda
- Mega Man series
- Tetris
- And hundreds of other commercial and homebrew titles

### Audio Configuration
- **Sample Rate**: 44.1 kHz stereo output
- **Latency**: Low-latency real-time audio synthesis
- **Channels**: Full 5-channel NES APU emulation with authentic sound

## 🔧 Build Instructions

### Standard Build
```bash
make          # Build the emulator
make re       # Clean rebuild
make clean    # Remove object files
make fclean   # Complete cleanup
```

### Build Configuration
The Makefile supports several build configurations:

```bash
# Debug build with additional symbols
make DFLAGS="-g -DDEBUG"

# Optimized release build
make DFLAGS="-O3 -DNDEBUG"

# Build with warnings as errors
make WARNING_FLAGS="-Wall -Wextra -Werror"
```

### Development Build
```bash
# Enable all warnings and debug information
make CXXFLAGS="-std=gnu++20 -g3 -Wall -Wextra -Wpedantic"
```

## 📋 Dependencies

### Required Libraries
- **SFML**: Complete multimedia framework
  - Graphics rendering and display management
  - Window management and event handling
  - Audio output and real-time synthesis
  - System utilities and timing
- **Standard Libraries**: pthread (threading), libm (mathematics)

### Development Dependencies
- **GCC/Clang**: Modern C++ compiler with C++20 support
- **GNU Make**: Build system automation
- **Git**: Version control (for cloning)

### Library Versions
- **Minimum SFML Version**: 2.5.0 or higher
- **C++ Standard**: C++20 (GNU extensions supported)
- **Compiler Support**: GCC 10+, Clang 12+

### Installation Commands by Platform

#### macOS (Homebrew)
```bash
brew install sfml make
```

#### Windows (MSYS2)
```bash
pacman -S mingw-w64-x86_64-sfml mingw-w64-x86_64-gcc make
```

## 📄 License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for complete details.

```
MIT License

Copyright (c) 2025 Mallory SCOTTON

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
```

## 🤝 Contributing

We welcome contributions to the NES Emulator project! Please read our [Contributing Guidelines](CONTRIBUTING.md) and [Code of Conduct](CODE_OF_CONDUCT.md) for details on our development process and community standards.

### Development Workflow
1. **Fork** the repository on GitHub
2. **Create** a feature branch (`git checkout -b feature/amazing-feature`)
3. **Implement** your changes following the coding standards
4. **Test** thoroughly with various ROM files
5. **Commit** your changes (`git commit -m 'Add amazing feature'`)
6. **Push** to your branch (`git push origin feature/amazing-feature`)
7. **Submit** a Pull Request with detailed description

### Code Standards
- **C++20 Standard**: Use modern C++ features and best practices
- **Naming Conventions**: Follow existing patterns (PascalCase for classes, camelCase for functions)
- **Documentation**: Include comprehensive code comments and documentation
- **Modular Design**: Maintain separation between emulation components
- **Testing**: Verify compatibility with multiple ROM files
- **Performance**: Maintain 60 FPS performance on target hardware

### Areas for Contribution
- **Additional Mappers**: Implement support for more cartridge types
- **Performance Optimization**: CPU and memory optimizations
- **Audio Enhancements**: Advanced audio filtering and effects
- **Debugging Tools**: Memory viewers, CPU state inspection
- **Platform Support**: macOS, Linux distribution improvements
- **Documentation**: User guides, technical documentation

## 📞 Contact

**Project Lead**: Mallory Scotton  
**Email**: [mscotton.pro@gmail.com](mailto:mscotton.pro@gmail.com)  
**GitHub**: [@mallory-scotton](https://github.com/mallory-scotton)  
**Project Repository**: [nes-emulator](https://github.com/mallory-scotton/nes-emulator)

For technical questions, bug reports, or feature requests, please:
1. **Check existing issues** on GitHub first
2. **Open a new issue** with detailed information
3. **Contact directly** for urgent matters or collaboration

### Community & Support
- **Bug Reports**: Use GitHub Issues with detailed reproduction steps
- **Feature Requests**: Discuss proposed features in GitHub Discussions
- **Technical Questions**: Email with specific technical details
- **Collaboration**: Open to academic and research partnerships

---

**NES Emulator** - Bringing classic Nintendo gaming to modern systems through accurate hardware emulation  
*Coded with ❤️ by passionate developers for the retro gaming community*
