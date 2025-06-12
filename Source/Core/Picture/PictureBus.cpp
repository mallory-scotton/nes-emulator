///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Core/Picture/PictureBus.hpp"
#include "Core/Cartridge.hpp"
#include "Core/Enums.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace NES
///////////////////////////////////////////////////////////////////////////////
namespace NES
{

///////////////////////////////////////////////////////////////////////////////
PictureBus::PictureBus(void)
    : Bus()
    , m_palette(0x20)
    , m_ram(0x800)
{
    for (int i = 0; i < 4; i++)
    {
        m_nameTables[i] = 0x000;
    }
}

///////////////////////////////////////////////////////////////////////////////
bool PictureBus::SetMapper(std::shared_ptr<Mapper> mapper)
{
    if (!mapper)
    {
        return (false);
    }
    m_mapper = mapper;
    UpdateMirroring();
    return (true);
}

///////////////////////////////////////////////////////////////////////////////
Byte PictureBus::Read(Address address)
{
    // Picture bus is limited to 0x3FFF
    address = address & 0X3FFF;

    // Check if the mapper is set
    if (!m_mapper)
    {
        return (0x00);
    }

    // Read from CHR memory
    if (address < 0X2000)
    {
        return (m_mapper->ReadCHR(address));
    }
    else if (address < 0x3EFF)
    {
        const int index = (address & 0x3FF);

        if (address >= 0x3000)
        {
            address -= 0x1000;
        }

        if (m_nameTables[0] >= m_ram.size())
        {
            return (m_mapper->ReadCHR(address));
        }
        else if (address < 0x2400)
        {
            return (m_ram[m_nameTables[0] + index]);
        }
        else if (address < 0x2800)
        {
            return (m_ram[m_nameTables[1] + index]);
        }
        else if (address < 0x2C00)
        {
            return (m_ram[m_nameTables[2] + index]);
        }
        else
        {
            return (m_ram[m_nameTables[3] + index]);
        }
    }
    else if (address < 0x3FFF)
    {
        Byte paletteAddress = address & 0x1F;

        return (ReadPalette(paletteAddress));
    }
    return (0x00);
}

///////////////////////////////////////////////////////////////////////////////
void PictureBus::Write(Address address, Byte value)
{
    // Picture bus is limited to 0x3FFF
    address = address & 0x3FFF;

    // Check if the mapper is set
    if (!m_mapper)
    {
        return;
    }

    // Write to CHR memory
    if (address < 0x2000)
    {
        m_mapper->WriteCHR(address, value);
    }
    else if (address < 0x3EFF)
    {
        const int index = address & 0x3FF;

        if (address >= 0X3000)
        {
            address -= 0x1000;
        }

        if (m_nameTables[0] >= m_ram.size())
        {
            m_mapper->WriteCHR(address, value);
        }
        else if (address < 0x2400)
        {
            m_ram[m_nameTables[0] + index] = value;
        }
        else if (address < 0x2800)
        {
            m_ram[m_nameTables[1] + index] = value;
        }
        else if (address < 0x2C00)
        {
            m_ram[m_nameTables[2] + index] = value;
        }
        else
        {
            m_ram[m_nameTables[3] + index] = value;
        }
    }
    else if (address < 0x3FFF)
    {
        Byte paletteAddress = address & 0X1F;

        if (paletteAddress >= 0x10 && address % 4 == 0)
        {
            paletteAddress = paletteAddress & 0xF;
        }
        m_palette[paletteAddress] = value;
    }
}

///////////////////////////////////////////////////////////////////////////////
Byte PictureBus::ReadPalette(Byte address)
{
    if (address >= 0x10 && address % 4 == 0)
    {
        address = address & 0xF;
    }
    return (m_palette[address]);
}

///////////////////////////////////////////////////////////////////////////////
void PictureBus::UpdateMirroring(void)
{
    if (!m_mapper)
    {
        return;
    }

    switch (m_mapper->GetCartridge().GetMirroringType())
    {
    case MirroringType::HORIZONTAL:
        m_nameTables[0] = m_nameTables[1] = 0x000;
        m_nameTables[2] = m_nameTables[3] = 0x400;
        break;
    case MirroringType::VERTICAL:
        m_nameTables[0] = m_nameTables[2] = 0x000;
        m_nameTables[1] = m_nameTables[3] = 0x400;
        break;
    case MirroringType::LOWER_SINGLE_SCREEN:
        for (int i = 0; i < 4; i++)
        {
            m_nameTables[i] = 0x000;
        }
        break;
    case MirroringType::UPPER_SINGLE_SCREEN:
        for (int i = 0; i < 4; i++)
        {
            m_nameTables[i] = 0x400;
        }
        break;
    case MirroringType::FOUR_SCREEN:
        m_nameTables[0] = m_ram.size();
        break;
    }
}

} // !namespace NES
