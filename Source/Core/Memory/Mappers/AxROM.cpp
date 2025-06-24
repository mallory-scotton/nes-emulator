///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Core/Memory/Mappers/AxROM.hpp"
#include "Core/Cartridge.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace NES::Mappers
///////////////////////////////////////////////////////////////////////////////
namespace NES::Mappers
{

///////////////////////////////////////////////////////////////////////////////
AxROM::AxROM(Cartridge& cartridge, std::function<void(void)> callback)
    : Mapper(cartridge, 7)
    , m_mirroring(MirroringType::LOWER_SINGLE_SCREEN)
    , m_callback(callback)
    , m_pgrBank(0)
{
    if (m_cartridge.GetCHR().size() == 0)
    {
        m_ram.resize(0x2000, 0x00);
    }
}

///////////////////////////////////////////////////////////////////////////////
Byte AxROM::ReadPGR(Address address)
{
    if (address >= 0x8000)
    {
        return (m_cartridge.GetPGR()[m_pgrBank * 0x8000 + (address & 0x7FFF)]);
    }
    return (0x00);
}

///////////////////////////////////////////////////////////////////////////////
void AxROM::WritePGR(Address address, Byte value)
{
    if (address >= 0x8000)
    {
        m_pgrBank = value & 0x07;
        m_mirroring = (value & 0x10) ?
            MirroringType::UPPER_SINGLE_SCREEN :
            MirroringType::LOWER_SINGLE_SCREEN;
        m_callback();
    }
}

///////////////////////////////////////////////////////////////////////////////
Byte AxROM::ReadCHR(Address address)
{
    if (address < 0x2000)
    {
        return (m_ram[address]);
    }
    return (0x00);
}

///////////////////////////////////////////////////////////////////////////////
void AxROM::WriteCHR(Address address, Byte value)
{
    if (address < 0x2000)
    {
        m_ram[address] = value;
    }
}

} // !namespace NES::Mappers
