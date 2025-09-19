# Contributing to NES Emulator

Thank you for your interest in contributing to the NES Emulator project! We welcome contributions from the community. Please read this guide to understand how to get started.

## Table of Contents
- [Code of Conduct](#code-of-conduct)
- [How to Contribute](#how-to-contribute)
- [Development Setup](#development-setup)
- [Building the Project](#building-the-project)
- [Coding Standards](#coding-standards)
- [Testing](#testing)
- [Submitting Changes](#submitting-changes)
- [Reporting Issues](#reporting-issues)

## Code of Conduct
This project adheres to a code of conduct. By participating, you agree to abide by its terms. See [CODE_OF_CONDUCT.md](CODE_OF_CONDUCT.md) for details.

## How to Contribute
1. Fork the repository on GitHub.
2. Create a new branch for your feature or bug fix.
3. Make your changes following the guidelines below.
4. Test your changes thoroughly.
5. Submit a pull request with a clear description of your changes.

## Development Setup
- Ensure you have a C++ compiler (e.g., g++) that supports C++20.
- Install SFML libraries (graphics, window, system, audio).
- Clone the repository and navigate to the project directory.

## Building the Project
Use the provided [Makefile](Makefile) to build the project:

```bash
make all
```

This will compile the sources in [Source/](Source/) and generate the executable `nes`.

## Coding Standards
- Use C++20 standard.
- Follow the existing code style: namespaces like `NES` and `NES::Audio`, consistent indentation, and comments.
- Include header guards in .hpp files.
- Use meaningful variable and function names.
- Reference existing classes like [`NES::CPU`](Source/Core/Processor/CPU.hpp) or [`NES::PPU`](Source/Core/Picture/PPU.hpp) for consistency.
- Avoid global variables; prefer member variables in classes.

## Testing
- Test with sample ROMs like [mario.nes](mario.nes), [tetris.nes](tetris.nes), etc.
- Ensure the emulator runs without crashes and renders graphics correctly.
- Manually verify audio output and controller inputs.

## Submitting Changes
- Ensure your code compiles without warnings.
- Write clear commit messages.
- Provide a detailed pull request description, including what was changed and why.
- Reference any related issues.

## Reporting Issues
If you find a bug or have a feature request, open an issue on GitHub with:
- A clear title and description.
- Steps to reproduce (if applicable).
- Expected vs. actual behavior.
- Screenshots or logs if relevant.

We appreciate your contributions!
