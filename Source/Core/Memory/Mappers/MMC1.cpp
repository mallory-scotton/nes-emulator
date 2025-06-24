///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Core/Memory/Mappers/MMC1.hpp"
#include "Core/Cartridge.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace NES::Mappers
///////////////////////////////////////////////////////////////////////////////
namespace NES::Mappers
{

///////////////////////////////////////////////////////////////////////////////
MMC1::MMC1(Cartridge& cartridge, std::function<void(void)> callback)
    : Mapper(cartridge, 1)
    , m_callback(callback)
    , m_mirroring(MirroringType::HORIZONTAL)
    , m_modeCHR(0)
    , m_modePGR(0)
    , m_register(0)
    , m_writeCount(0)
    , m_regPGR(0)
    , m_regCHR{0, 0}
    , m_bankPGR{nullptr, nullptr}
    , m_bankCHRIndex{0, 0}
{
    if (m_cartridge.GetCHR().size() == 0)
    {
        m_usesCHRRAM = true;
        m_ram.resize(0x8000, 0x00);
    }
    else
    {
        m_usesCHRRAM = false;
        m_bankCHRIndex[0] = 0;
        m_bankCHRIndex[1] = 0x1000 * m_regCHR[1];
    }
    m_bankPGR[0] = &m_cartridge.GetPGR()[0];
    m_bankPGR[1] = &m_cartridge.GetPGR()[m_cartridge.GetPGR().size() - 0x4000];
}

///////////////////////////////////////////////////////////////////////////////
Byte MMC1::ReadPGR(Address address)
{
    if (address < 0xC000)
    {
        return (*(m_bankPGR[0] + (address & 0x3FFF)));
    }
    return (*(m_bankPGR[1] + (address & 0x3FFF)));
}

///////////////////////////////////////////////////////////////////////////////
void MMC1::WritePGR(Address address, Byte value)
{
    if (!(value & 0x80))
    {
        m_register = (m_register >> 1) | ((value & 1) << 4);
        m_writeCount++;

        if (m_writeCount == 5)
        {
            if (address <= 0x9FFF)
            {
                switch (m_register & 0x3)
                {
                case 0:
                    m_mirroring = MirroringType::LOWER_SINGLE_SCREEN;
                    break;
                case 1:
                    m_mirroring = MirroringType::UPPER_SINGLE_SCREEN;
                    break;
                case 2:
                    m_mirroring = MirroringType::VERTICAL;
                    break;
                case 3:
                    m_mirroring = MirroringType::HORIZONTAL;
                    break;
                }

                m_callback();

                m_modeCHR = (m_register & 0x10) >> 4;
                m_modePGR = (m_register & 0xC) >> 2;

                CalculatePGRPointers();

                if (m_modeCHR == 0)
                {
                    m_bankCHRIndex[0] = 0x1000 * (m_regCHR[0] | 1);
                    m_bankCHRIndex[1] = m_bankCHRIndex[0] + 0x1000;
                }
                else
                {
                    m_bankCHRIndex[0] = 0x1000 * m_regCHR[0];
                    m_bankCHRIndex[1] = 0x1000 * m_regCHR[1];
                }
            }
            else if (address <= 0xBFFF)
            {
                m_regCHR[0] = m_register;
                m_bankCHRIndex[0] = 0x1000 * (m_register | (1 - m_modeCHR));

                if (m_modeCHR == 0)
                {
                    m_bankCHRIndex[1] = m_bankCHRIndex[0] + 0x1000;
                }
            }
            else if (address <= 0xDFFF)
            {
                m_regCHR[1] = m_register;
                if (m_modeCHR == 1)
                {
                    m_bankCHRIndex[1] = 0x1000 * m_register;
                }
            }
            else
            {
                m_register &= 0xF;
                m_regPGR = m_register;
                CalculatePGRPointers();
            }

            m_register = 0;
            m_writeCount = 0;
        }
    }
    else
    {
        m_register = 0;
        m_writeCount = 0;
        m_modePGR = 3;
        CalculatePGRPointers();
    }
}

///////////////////////////////////////////////////////////////////////////////
void MMC1::CalculatePGRPointers(void)
{
    if (m_modePGR <= 1)
    {
        m_bankPGR[0] = &m_cartridge.GetPGR()[0x4000 * (m_regPGR & ~1)];
        m_bankPGR[1] = m_bankPGR[0] + 0x4000;
    }
    else if (m_modePGR == 2)
    {
        m_bankPGR[0] = &m_cartridge.GetPGR()[0];
        m_bankPGR[1] = m_bankPGR[0] + 0x4000 * m_regPGR;
    }
    else
    {
        m_bankPGR[0] = &m_cartridge.GetPGR()[0x4000 * m_regPGR];
        m_bankPGR[1] = &m_cartridge.GetPGR()[m_cartridge.GetPGR().size() - 0x4000];
    }
}

///////////////////////////////////////////////////////////////////////////////
Byte MMC1::ReadCHR(Address address)
{
    if (m_usesCHRRAM)
    {
        if (address < 0x1000)
        {
            return (m_ram[m_bankCHRIndex[0] + address]);
        }
        return (m_ram[m_bankCHRIndex[1] + (address & 0xFFF)]);
    }

    if (address < 0x1000)
    {
        return (m_cartridge.GetCHR()[m_bankCHRIndex[0] + address]);
    }
    return (m_cartridge.GetCHR()[m_bankCHRIndex[1] + (address & 0xFFF)]);
}

///////////////////////////////////////////////////////////////////////////////
void MMC1::WriteCHR(Address address, Byte value)
{
    if (m_usesCHRRAM)
    {
        if (address < 0x1000)
        {
            m_ram[m_bankCHRIndex[0] + address] = value;
        }
        else
        {
            m_ram[m_bankCHRIndex[1] + (address & 0xFFF)] = value;
        }
    }
}

} // !namespace NES::Mappers
