///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Core/Memory/Mappers/UxROM.hpp"
#include "Core/Cartridge.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace NES::Mappers
///////////////////////////////////////////////////////////////////////////////
namespace NES::Mappers
{

///////////////////////////////////////////////////////////////////////////////
UxROM::UxROM(Cartridge& cartridge)
    : Mapper(cartridge, 2)
    , m_usesCHRRAM(false)
    , m_selectPGR(0x00)
{
    if (m_cartridge.GetCHR().size() == 0)
    {
        m_usesCHRRAM = true;
        m_ram.resize(0x2000, 0x00);
    }
    m_bankPtr = &m_cartridge.GetPGR()[m_cartridge.GetPGR().size() - 0x4000];
}

///////////////////////////////////////////////////////////////////////////////
Byte UxROM::ReadPGR(Address address)
{
    if (address < 0xC000)
    {
        return (m_cartridge.GetPGR()
            [((address - 0x8000) & 0x3FFF) | (m_selectPGR << 14)]
        );
    }
    return (*(m_bankPtr + (address & 0x3FFF)));
}

///////////////////////////////////////////////////////////////////////////////
void UxROM::WritePGR(Address address, Byte value)
{
    NES_UNUSED(address);
    m_selectPGR = value;
}

///////////////////////////////////////////////////////////////////////////////
Byte UxROM::ReadCHR(Address address)
{
    if (m_usesCHRRAM)
    {
        return (m_ram[address]);
    }
    return (m_cartridge.GetCHR()[address]);
}

///////////////////////////////////////////////////////////////////////////////
void UxROM::WriteCHR(Address address, Byte value)
{
    if (m_usesCHRRAM)
    {
        m_ram[address] = value;
    }
}

} // !namespace NES::Mappers
