///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Core/Processor/MainBus.hpp"
#include <iostream>

///////////////////////////////////////////////////////////////////////////////
// Namespace NES
///////////////////////////////////////////////////////////////////////////////
namespace NES
{

///////////////////////////////////////////////////////////////////////////////
MainBus::MainBus(
    PPU& ppu,
    APU& apu,
    Controller& controller1,
    Controller& controller2,
    std::function<void(Byte)> callback
)
    : m_ram(0x800, 0x00)
    , m_extRam(0x2000, 0x00)
    , m_dmaCallback(callback)
    , m_ppu(ppu)
    , m_apu(apu)
    , m_controller1(controller1)
    , m_controller2(controller2)
{}

///////////////////////////////////////////////////////////////////////////////
Address MainBus::NormalizeMirror(Address address)
{
    // Normalize the address to handle mirroring for PPU registers
    if (address >= PPU_CTRL && address < APU_REGISTER_START)
    {
        return (address & 0x2007);
    }
    return (address);
}

///////////////////////////////////////////////////////////////////////////////
Byte MainBus::Read(Address address)
{
    if (address < 0x2000)
    {
        return (m_ram[address & 0x7FF]);
    }
    else if (address < 0x4020)
    {
        address = NormalizeMirror(address);
        switch (static_cast<Register>(address))
        {
        case PPU_STATUS:
            return (m_ppu.GetStatus());
        case PPU_DATA:
            return (m_ppu.GetData());
        case JOY1:
            return (m_controller1.Read());
        case JOY2_AND_FRAME_CONTROL:
            return (m_controller2.Read());
        case OAM_DATA:
            return (m_ppu.GetOAMData());
        case APU_CONTROL_AND_STATUS:
            return (m_apu.ReadStatus());
        default:
            return (0x00); // Unhandled register, return 0
        }
    }
    else if (address < 0x6000)
    {
        // Expansion ROM or other memory-mapped devices
        return (0x00);
    }
    else if (address < 0x8000)
    {
        return (m_extRam[address - 0x6000]);
    }
    else if (m_mapper)
    {
        return (m_mapper->ReadPGR(address));
    }
    return (0x00);
}

///////////////////////////////////////////////////////////////////////////////
void MainBus::Write(Address address, Byte value)
{
    if (address < 0x2000)
    {
        m_ram[address & 0x7FF] = value;
    }
    else if (address < 0x4020)
    {
        address = NormalizeMirror(address);

        switch (static_cast<Register>(address))
        {
        case PPU_CTRL:
            m_ppu.Control(value);
            break;
        case PPU_MASK:
            m_ppu.SetMask(value);
            break;
        case OAM_ADDR:
            m_ppu.SetOAMAddress(value);
            break;
        case OAM_DATA:
            m_ppu.SetOAMData(value);
            break;
        case PPU_ADDR:
            m_ppu.SetDataAddress(value);
            break;
        case PPU_SCROLL:
            m_ppu.SetScroll(value);
            break;
        case PPU_DATA:
            m_ppu.SetData(value);
            break;
        case OAM_DMA:
            m_dmaCallback(value);
            break;
        case JOY1:
            m_controller1.Strobe(value);
            m_controller2.Strobe(value);
            break;
        case APU_CONTROL_AND_STATUS:
        case JOY2_AND_FRAME_CONTROL:
            m_apu.WriteRegister(address, value);
            break;
        default:
            if (address >= APU_REGISTER_START && address <= APU_REGISTER_END)
            {
                m_apu.WriteRegister(address, value);
            }
            break;
        }
    }
    else if (address < 0x6000)
    {
        // Expansion ROM or other memory-mapped devices
        // No write operation defined, do nothing
    }
    else if (address < 0x8000)
    {
        m_extRam[address - 0x6000] = value;
    }
    else if (m_mapper)
    {
        m_mapper->WritePGR(address, value);
    }
}

///////////////////////////////////////////////////////////////////////////////
bool MainBus::SetMapper(std::shared_ptr<Mapper> mapper)
{
    if (!mapper)
    {
        return (false);
    }
    m_mapper = mapper;
    return (true);
}

///////////////////////////////////////////////////////////////////////////////
const Byte* MainBus::GetPagePtr(Byte page)
{
    Address address = page << 8;

    if (address < 0x2000)
    {
        return (&m_ram[address & 0x7FF]);
    }
    else if (address < 0x4020)
    {
        // Register address memory pointer access attempt
    }
    else if (address < 0x6000)
    {
        // Expansion ROM access attempted, which is unsupported
    }
    else if (address < 0x8000)
    {
        return (&m_extRam[address - 0x6000]);
    }
    else
    {
        // Unexpected DMA request
    }
    return (nullptr);
}

} // !namespace NES
