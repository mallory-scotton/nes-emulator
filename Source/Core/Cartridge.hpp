///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Utils/Types.hpp"
#include "Core/Enums.hpp"
#include <vector>
#include <fstream>

///////////////////////////////////////////////////////////////////////////////
// Namespace NES
///////////////////////////////////////////////////////////////////////////////
namespace NES
{

///////////////////////////////////////////////////////////////////////////////
/// \brief Cartridge class for handling NES ROMs
///
/// This class is responsible for loading and managing NES ROMs, including
/// reading the iNES header, loading program and character ROM data, and
/// determining the mapper and mirroring type. It provides functionality to
/// read the ROM file, validate the iNES header, and store the ROM data in
/// appropriate data structures.
///
///////////////////////////////////////////////////////////////////////////////
class Cartridge
{
private:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief iNES header structure
    ///
    /// This structure represents the iNES header used in NES ROM files.
    /// It contains fields for the magic number, program ROM size, character
    /// ROM size, flags for mapper and mirroring, and other metadata.
    ///
    ///////////////////////////////////////////////////////////////////////////
    struct iNESHeader
    {
        char magic[4];          //<! NES magic code
        Uint8 pgrSize;          //<! PRG ROM size in 16KB units
        Uint8 chrSize;          //<! CHR ROM size in 8KB units
        Uint8 flags6;           //<! Mapper, mirroring, battery, trainer
        Uint8 flags7;           //<! Mapper, VS/PlayChoice, NES 2.0
        Uint8 flags8;           //<! PRG-RAM size
        Uint8 flags9;           //<! TV system
        Uint8 flags10;          //<! TV system, PRG-RAM presence
        char padding[5];        //<! Unused padding
    };

private:
    ///////////////////////////////////////////////////////////////////////////
    // Private properties
    ///////////////////////////////////////////////////////////////////////////
    Rom<Byte> m_pgr;            //<! Program ROM data
    Rom<Byte> m_chr;            //<! Character ROM data
    Uint8 m_mapper;             //<! Mapper number
    MirroringType m_mirroring;  //<! Mirroring type

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Constructor for Cartridge class
    ///
    /// \param filePath Path to the ROM file
    ///
    ///////////////////////////////////////////////////////////////////////////
    Cartridge(const Path& filePath);

private:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Load the ROM file and read its contents
    ///
    /// \param filePath Path to the ROM file
    ///
    ///////////////////////////////////////////////////////////////////////////
    void LoadROM(const Path& filePath);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Read and validate the iNES header from the ROM file
    ///
    /// \param file The input file stream to read the header from
    ///
    /// \return The validated iNES header
    ///
    ///////////////////////////////////////////////////////////////////////////
    iNESHeader ReadAndValidateHeader(std::ifstream& file);
};

} // !namespace NES
