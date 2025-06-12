///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Core/Memory/Mappers/NROM.hpp"
#include "Utils/Constants.hpp"
#include "Core/Cartridge.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace NES::Mappers
///////////////////////////////////////////////////////////////////////////////
namespace NES::Mappers
{

///////////////////////////////////////////////////////////////////////////////
NROM::NROM(Cartridge& cartridge)
    : Mapper(cartridge, 0)
{
    m_oneBank = (cartridge.GetPGR().size() == KB32);
    m_chrRAM.clear();
    m_usesCHRRam = (cartridge.GetCHR().size() == 0);
    if (m_usesCHRRam)
    {
        m_chrRAM.resize(KB8, 0x00);
    }
}

///////////////////////////////////////////////////////////////////////////////
Byte NROM::ReadPGR(Uint16 address)
{
    if (!m_oneBank)
    {
        return (m_cartridge.GetPGR()[address - KB32]);
    }
    else
    {
        return (m_cartridge.GetPGR()[(address - KB32) & KB16]);
    }
}

///////////////////////////////////////////////////////////////////////////////
void NROM::WritePGR(Uint16 address, Byte value)
{
    (void)address; (void)value;
    // NROM does not support writing to PGR memory
}

///////////////////////////////////////////////////////////////////////////////
Byte NROM::ReadCHR(Uint16 address)
{
    if (m_usesCHRRam)
    {
        return (m_chrRAM[address]);
    }
    else
    {
        return (m_cartridge.GetCHR()[address]);
    }
}

///////////////////////////////////////////////////////////////////////////////
void NROM::WriteCHR(Uint16 address, Byte value)
{
    // NROM does not support writing to CHR memory if it does not use CHR RAM
    if (m_usesCHRRam)
    {
        m_chrRAM[address] = value;
    }
}

} // !namespace NES::Mappers
