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
    , m_buffer(SCANLINE_VISIBLE_DOTS * VISIBLE_SCANLINES * 4, 0xFF)
    , m_spriteMemory(256, 0x00)
{
    for (int x = 0; x < SCANLINE_VISIBLE_DOTS; x++)
    {
        for (int y = 0; y < VISIBLE_SCANLINES; y++)
        {
            m_screen[(y * SCANLINE_VISIBLE_DOTS + x) * 4 + 0] = 0xFF; // R
            m_buffer[(y * SCANLINE_VISIBLE_DOTS + x) * 4 + 0] = 0xFF; // R
            m_screen[(y * SCANLINE_VISIBLE_DOTS + x) * 4 + 1] = 0x00; // G
            m_buffer[(y * SCANLINE_VISIBLE_DOTS + x) * 4 + 1] = 0x00; // G
            m_screen[(y * SCANLINE_VISIBLE_DOTS + x) * 4 + 2] = 0xFF; // B
            m_buffer[(y * SCANLINE_VISIBLE_DOTS + x) * 4 + 2] = 0xFF; // B
            m_screen[(y * SCANLINE_VISIBLE_DOTS + x) * 4 + 3] = 0xFF; // A
            m_buffer[(y * SCANLINE_VISIBLE_DOTS + x) * 4 + 3] = 0xFF; // A
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
const std::vector<Byte>& PPU::GetScreen(void) const
{
    return (m_screen);
}

///////////////////////////////////////////////////////////////////////////////
void PPU::Step(void)
{
    // Switch based on the current pipeline state
    switch (m_pipelineState)
    {
    case State::PRE_RENDER:
        PreRenderStep();
        break;
    case State::RENDER:
        RenderStep();
        break;
    case State::POST_RENDER:
        PostRenderStep();
        break;
    case State::VERTICAL_BLANK:
        VerticalBlankStep();
        break;
    }

    // Increment cycle and scanline
    m_cycle++;
}

///////////////////////////////////////////////////////////////////////////////
void PPU::Reset(void)
{
    m_longSprites = false;
    m_generateInterrupt = false;
    m_greyscaleMode = false;
    m_vblank = false;
    m_spriteOverflow = false;
    m_showBackground = true;
    m_showSprites = true;
    m_evenFrame = true;
    m_firstWrite = true;
    m_dataAddress = 0x00;
    m_cycle = 0;
    m_scanline = 0;
    m_spriteDataAddress = 0x00;
    m_fineXScroll = 0;
    m_tempAddress = 0x00;
    m_dataAddrIncrement = 1;
    m_pipelineState = State::PRE_RENDER;
    m_scanlineSprites.reserve(8);
    m_scanlineSprites.resize(0);
}

///////////////////////////////////////////////////////////////////////////////
void PPU::PreRenderStep(void)
{
    bool showActors = m_showBackground && m_showSprites;

    if (m_cycle == 1)
    {
        m_vblank = false;
        m_sprZeroHit = false;
    }
    else if (m_cycle == SCANLINE_VISIBLE_DOTS + 2 && showActors)
    {
        // Set bits related to horizontal position
        m_dataAddress &= ~0x41F;
        m_dataAddress |= m_tempAddress & 0x41F;
    }
    else if (m_cycle > 280 && m_cycle <= 304 && showActors)
    {
        // Set bits related to vertical position
        m_dataAddress &= ~0x7BE0;
        m_dataAddress |= m_tempAddress & 0x7BE0;
    }

    // If we are at the end of the pre-render scanline
    if (m_cycle >= SCANLINE_END_CYCLE - (!m_evenFrame && showActors))
    {
        m_pipelineState = State::RENDER;
        m_cycle = 0;
        m_scanline = 0;
    }

    // Handle IRQ for MMC3 mapper
    if (m_cycle == 260 && showActors)
    {
        m_bus.ScanlineIRQ();
    }
}

///////////////////////////////////////////////////////////////////////////////
void PPU::RenderStep(void)
{
    // TODO: Implement rendering logic
}

///////////////////////////////////////////////////////////////////////////////
void PPU::PostRenderStep(void)
{
    // Check if we are at the end of the scanline
    if (m_cycle >= SCANLINE_END_CYCLE)
    {
        // If we are at the last visible scanline,
        // switch to vertical blank state
        m_scanline++;
        m_cycle = 0;
        m_pipelineState = State::VERTICAL_BLANK;

        // If we are at the last scanline, copy the buffer to the screen
        for (size_t i = 0; i < m_buffer.size(); i++)
        {
            m_screen[i] = m_buffer[i];
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void PPU::VerticalBlankStep(void)
{
    // If we are in the vertical blank state, we need to handle the scanline
    if (m_cycle == 1 && m_scanline == VISIBLE_SCANLINES + 1)
    {
        // Set the vertical blank flag
        m_vblank = true;
        if (m_generateInterrupt)
        {
            m_vblankCallback();
        }
    }

    // If we are at the end of the vertical blank scanline
    if (m_cycle >= SCANLINE_END_CYCLE)
    {
        m_scanline++;
        m_cycle = 0;
    }

    // If we have reached the end of the frame
    if (m_scanline >= FRAME_END_SCANLINE)
    {
        // Reset the scanline and cycle to start a new frame
        m_pipelineState = State::PRE_RENDER;
        m_scanline = 0;
        m_evenFrame = !m_evenFrame;
    }
}

} // !namespace NES
