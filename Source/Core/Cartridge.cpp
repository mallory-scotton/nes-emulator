///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Core/Cartridge.hpp"
#include "Utils.hpp"
#include <stdexcept>
#include <fstream>
#include <iostream>

///////////////////////////////////////////////////////////////////////////////
// Namespace NES
///////////////////////////////////////////////////////////////////////////////
namespace NES
{

///////////////////////////////////////////////////////////////////////////////
Cartridge::Cartridge(const Path& filePath)
    : m_mapper(0)
    , m_mirroring(MirroringType::HORIZONTAL)
{
    LoadROM(filePath);
}

///////////////////////////////////////////////////////////////////////////////
void Cartridge::LoadROM(const Path& filePath)
{
    // Open the ROM file
    std::ifstream file(filePath, std::ios::binary);

    // Check if the file was opened successfully
    if (!file.is_open())
    {
        throw std::runtime_error(
            "Failed to open ROM file: " + filePath.string()
        );
    }

    // Read the header
    iNESHeader header = ReadAndValidateHeader(file);

    // Print ROM information
    std::cout << "ROM Info:" << std::endl;
    std::cout << "PRG ROM: " << header.pgrSize * 16 << "KB" << std::endl;
    std::cout << "CHR ROM: " << header.chrSize * 8 << "KB" << std::endl;
    std::cout << "Mapper: " << (int)m_mapper << std::endl;
    std::cout << "Mirroring: " << (m_mirroring == MirroringType::HORIZONTAL
        ? "Horizontal"
        : m_mirroring == MirroringType::VERTICAL
            ? "Vertical"
            : m_mirroring == MirroringType::FOUR_SCREEN
                ? "Four-screen"
                : "Single-screen"
        ) << std::endl;

    // Read program ROM
    Uint32 programSize = header.pgrSize * KB16;
    m_pgr.resize(programSize);
    if (!file.read(reinterpret_cast<char*>(m_pgr.data()), programSize))
    {
        throw std::runtime_error("Failed to read PRG ROM data.");
    }

    // Read character ROM
    Uint32 characterSize = header.chrSize * KB8;
    m_chr.resize(characterSize);
    if (!file.read(reinterpret_cast<char*>(m_chr.data()), characterSize))
    {
        throw std::runtime_error("Failed to read CHR ROM data.");
    }
}

///////////////////////////////////////////////////////////////////////////////
Cartridge::iNESHeader Cartridge::ReadAndValidateHeader(std::ifstream& file)
{
    iNESHeader header;

    // Read the header
    if (!file.read(reinterpret_cast<char*>(&header), sizeof(iNESHeader)))
    {
        throw std::runtime_error("Failed to read iNES header.");
    }

    // Validate iNES magic number
    if (header.magic[0] != 'N' || header.magic[1] != 'E' ||
        header.magic[2] != 'S' || header.magic[3] != 0x1A)
    {
        throw std::runtime_error("Invalid iNES magic number.");
    }

    // Check for PAL ROM
    if (header.flags9 & 0x01)
    {
        throw std::runtime_error("PAL ROM not supported.");
    }

    // Check for valid padding
    for (int i = 0; i < 5; ++i)
    {
        if (header.padding[i] != 0)
        {
            throw std::runtime_error("Invalid padding in iNES header.");
        }
    }

    // Skip trainer if present
    if (header.flags6 & 0x04)
    {
        std::cerr << "Trainer not supported." << std::endl;
        file.seekg(512, std::ios::cur);
    }

    // Set mapper and mirroring type
    m_mapper = (header.flags7 & 0xF0) | ((header.flags6 & 0xF0) >> 4);

    // Determine mirroring type
    if (header.flags6 & 0x08)
    {
        m_mirroring = MirroringType::FOUR_SCREEN;
    }
    else if (header.flags6 & 0x01)
    {
        m_mirroring = MirroringType::VERTICAL;
    }
    else
    {
        m_mirroring = MirroringType::HORIZONTAL;
    }

    return (header);
}

///////////////////////////////////////////////////////////////////////////////
const Rom<Byte>& Cartridge::GetPGR(void) const
{
    return (m_pgr);
}

///////////////////////////////////////////////////////////////////////////////
Rom<Byte>& Cartridge::GetPGR(void)
{
    return (m_pgr);
}

///////////////////////////////////////////////////////////////////////////////
const Rom<Byte>& Cartridge::GetCHR(void) const
{
    return (m_chr);
}

///////////////////////////////////////////////////////////////////////////////
Rom<Byte>& Cartridge::GetCHR(void)
{
    return (m_chr);
}

///////////////////////////////////////////////////////////////////////////////
Uint8 Cartridge::GetMapper(void) const
{
    return (m_mapper);
}

///////////////////////////////////////////////////////////////////////////////
MirroringType Cartridge::GetMirroring(void) const
{
    return (m_mirroring);
}

} // !namespace NES
