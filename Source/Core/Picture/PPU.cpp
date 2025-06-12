///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Core/Picture/PPU.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace NES
///////////////////////////////////////////////////////////////////////////////
namespace NES
{

///////////////////////////////////////////////////////////////////////////////
PPU::PPU(PictureBus& bus)
    : m_bus(bus)
    , m_screen(SCANLINE_VISIBLE_DOTS * VISIBLE_SCANLINES * 4, 0xFF)
    , m_spriteMemory(256, 0x00)
{
    for (int x = 0; x < SCANLINE_VISIBLE_DOTS; x++)
    {
        for (int y = 0; y < VISIBLE_SCANLINES; y++)
        {
            m_screen[(y * SCANLINE_VISIBLE_DOTS + x) * 4 + 0] = 0xFF; // R
            m_screen[(y * SCANLINE_VISIBLE_DOTS + x) * 4 + 1] = 0x00; // G
            m_screen[(y * SCANLINE_VISIBLE_DOTS + x) * 4 + 2] = 0xFF; // B
            m_screen[(y * SCANLINE_VISIBLE_DOTS + x) * 4 + 3] = 0xFF; // A
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
const std::vector<Byte>& PPU::GetScreen(void) const
{
    return (m_screen);
}

} // !namespace NES
