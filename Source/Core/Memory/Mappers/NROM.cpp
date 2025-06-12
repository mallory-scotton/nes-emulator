///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Core/Memory/Mappers/NROM.hpp"
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
    m_oneBank = (cartridge.GetPGR().size() == 0x4000);
    m_chrRAM.clear();
    m_usesCHRRam = (cartridge.GetCHR().size() == 0);
    if (m_usesCHRRam)
    {
        m_chrRAM.resize(0x2000, 0x00);
    }
}

///////////////////////////////////////////////////////////////////////////////
Byte NROM::ReadPGR(Address address)
{
    if (!m_oneBank)
    {
        return (m_cartridge.GetPGR()[address - 0x8000]);
    }
    else
    {
        return (m_cartridge.GetPGR()[(address - 0x8000) & 0X3FFF]);
    }
}

///////////////////////////////////////////////////////////////////////////////
void NROM::WritePGR(Address address, Byte value)
{
    NES_UNUSED(address);
    NES_UNUSED(value);
    // NROM does not support writing to PGR memory
}

///////////////////////////////////////////////////////////////////////////////
Byte NROM::ReadCHR(Address address)
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
void NROM::WriteCHR(Address address, Byte value)
{
    // NROM does not support writing to CHR memory if it does not use CHR RAM
    if (m_usesCHRRam)
    {
        m_chrRAM[address] = value;
    }
}

} // !namespace NES::Mappers
