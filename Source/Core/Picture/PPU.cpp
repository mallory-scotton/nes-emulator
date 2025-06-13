///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Core/Picture/PPU.hpp"
#include "Core/Colors.hpp"

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
    if (m_cycle > 0 && m_cycle <= SCANLINE_VISIBLE_DOTS)
    {
        Byte bgColor = 0;
        Byte sprColor = 0;
        bool bgOpaque = false;
        bool sprOpaque = true;
        bool spriteForeground = false;

        int x = m_cycle - 1;
        int y = m_scanline;

        if (m_showBackground)
        {
            int x_fine = (m_fineXScroll + x) % 8;

            if (!m_hideEdgeBackground || x >= 8)
            {
                int addr = 0x2000 | (m_dataAddress & 0x0FFF);
                Byte tile = m_bus.Read(addr);

                addr = (tile * 16) + ((m_dataAddress >> 12) & 0x7);
                addr |= static_cast<int>(m_bgPage) << 12;
                bgColor = (m_bus.Read(addr) >> (7 ^ x_fine)) & 1;
                bgColor |= ((m_bus.Read(addr + 8) >> (7 ^ x_fine)) & 1) << 1;

                bgOpaque = bgColor;

                addr = 0x23C0 |
                    (m_dataAddress & 0x0C00) |
                    ((m_dataAddress >> 4) & 0x38) |
                    ((m_dataAddress >> 2) & 0x07);
                Byte attribute = m_bus.Read(addr);
                int shift = ((m_dataAddress >> 4) & 4) | (m_dataAddress & 2);
                bgColor |= ((attribute >> shift) & 0x3) << 2;
            }

            if (x_fine == 7)
            {
                if ((m_dataAddress & 0x001F) == 31)
                {
                    m_dataAddress &= ~0x001F;
                    m_dataAddress ^= 0x0400;
                }
                else
                {
                    m_dataAddress += 1;
                }
            }
        }

        if (m_showSprites && (!m_hideEdgeSprites || x >= 8))
        {
            for (Byte i : m_scanlineSprites)
            {
                Byte spr_x = m_spriteMemory[i * 4 + 3];

                if (0 > x - spr_x || x - spr_x >= 8)
                {
                    continue;
                }

                Byte spr_y = m_spriteMemory[i * 4 + 0] + 1;
                Byte tile = m_spriteMemory[i * 4 + 1];
                Byte attribute = m_spriteMemory[i * 4 + 2];

                int length = (m_longSprites) ? 16 : 8;

                int x_shift = (x - spr_x) % 8;
                int y_offset = (y - spr_y) % length;

                if ((attribute & 0x40) == 0)
                {
                    x_shift ^= 7;
                }
                if ((attribute & 0x80) != 0)
                {
                    y_offset ^= (length - 1);
                }

                Address addr = 0;

                if (!m_longSprites)
                {
                    addr = tile * 16 + y_offset;
                    if (m_sprPage == CharacterPage::HIGH)
                    {
                        addr += 0x1000;
                    }
                }
                else
                {
                    y_offset = (y_offset & 7) | ((y_offset & 8) << 1);
                    addr = (tile >> 1) * 32 + y_offset;
                    addr |= (tile & 1) << 12;
                }

                sprColor |= (m_bus.Read(addr) >> (x_shift)) & 1;
                sprColor |= ((m_bus.Read(addr + 8) >> (x_shift)) & 1) << 1;

                if (!(sprOpaque = sprColor))
                {
                    sprColor = 0;
                    continue;
                }

                sprColor |= 0x10;
                sprColor |= (attribute & 0x3) << 2;

                spriteForeground = !(attribute & 0x20);

                if (
                    !m_sprZeroHit && m_showBackground &&
                    i == 0 && sprOpaque && bgOpaque
                )
                {
                    m_sprZeroHit = true;
                }

                break;
            }
        }

        Byte paletteAddr = bgColor;

        if (
            (!bgOpaque && sprOpaque) ||
            (bgOpaque && sprOpaque && spriteForeground)
        )
        {
            paletteAddr = sprColor;
        }
        else if (!bgOpaque && !sprOpaque)
        {
            paletteAddr = 0;
        }

        int index = y * SCANLINE_VISIBLE_DOTS * 4 + x * 4;
        Uint32 color = NES_COLORS[m_bus.ReadPalette(paletteAddr)];

        m_buffer[index + 0] = (color >> 16) & 0xFF; // R
        m_buffer[index + 1] = (color >> 8) & 0xFF;  // G
        m_buffer[index + 2] = (color >> 0) & 0xFF;  // B
        m_buffer[index + 3] = 0xFF;                 // A
    }
    else if (m_cycle == SCANLINE_VISIBLE_DOTS + 1 && m_showBackground)
    {
        if ((m_dataAddress & 0x7000) != 0x7000)
        {
            m_dataAddress += 0x1000;
        }
        else
        {
            m_dataAddress &= ~0x7000;
            int y = (m_dataAddress & 0x03E0) >> 5;

            if (y == 29)
            {
                y = 0;
                m_dataAddress ^= 0x0800;
            }
            else if (y == 31)
            {
                y = 0;
            }
            else
            {
                y++;
            }

            m_dataAddress = (m_dataAddress & ~0x03E0) | (y << 5);
        }
    }
    else if (
        m_cycle == SCANLINE_VISIBLE_DOTS + 2 &&
        m_showBackground && m_showSprites
    )
    {
        m_dataAddress &= ~0x41F;
        m_dataAddress |= m_tempAddress & 0x41F;
    }

    if (m_cycle == 260 && m_showBackground && m_showSprites)
    {
        m_bus.ScanlineIRQ();
    }

    if (m_cycle >= SCANLINE_END_CYCLE)
    {
        m_scanlineSprites.resize(0);

        int range = 8;
        if (m_longSprites)
        {
            range = 16;
        }

        size_t j = 0;
        for (size_t i = m_spriteDataAddress / 4; i < 64; i++)
        {
            int diff = (m_scanline - m_spriteMemory[i * 4]);
            if (0 <= diff && diff < range)
            {
                if (j >= 8)
                {
                    m_spriteOverflow = true;
                    break;
                }
                m_scanlineSprites.push_back(i);
                j++;
            }
        }

        m_scanline++;
        m_cycle = 0;
    }

    if (m_scanline >= VISIBLE_SCANLINES)
    {
        m_pipelineState = State::POST_RENDER;
    }
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

///////////////////////////////////////////////////////////////////////////////
void PPU::SetVBlankCallback(std::function<void(void)> callback)
{
    m_vblankCallback = std::move(callback);
}

///////////////////////////////////////////////////////////////////////////////
void PPU::Control(Byte control)
{
    m_generateInterrupt = control & 0x80;
    m_longSprites = control & 0x20;
    m_bgPage = static_cast<CharacterPage>(!!(control & 0x10));
    m_sprPage = static_cast<CharacterPage>(!!(control & 0x8));

    if (control & 0x4)
    {
        m_dataAddrIncrement = 0x20;
    }
    else
    {
        m_dataAddrIncrement = 1;
    }

    m_tempAddress &= ~0xC00;
    m_tempAddress |= (control & 0x3) << 10;
}

///////////////////////////////////////////////////////////////////////////////
void PPU::SetMask(Byte mask)
{
    m_greyscaleMode = mask & 0x1;
    m_hideEdgeBackground = !(mask & 0x2);
    m_hideEdgeSprites = !(mask & 0x4);
    m_showBackground = mask & 0x8;
    m_showSprites = mask & 0x10;
}

///////////////////////////////////////////////////////////////////////////////
void PPU::SetOAMAdress(Byte address)
{
    m_spriteDataAddress = address;
}

///////////////////////////////////////////////////////////////////////////////
void PPU::SetDataAddress(Byte address)
{
    if (m_firstWrite)
    {
        m_tempAddress &= ~0xFF00;
        m_tempAddress |= (address & 0x3F) << 8;
        m_firstWrite = false;
    }
    else
    {
        m_tempAddress &= ~0xFF;
        m_tempAddress |= address;
        m_dataAddress = m_tempAddress;
        m_firstWrite = true;
    }
}

///////////////////////////////////////////////////////////////////////////////
void PPU::SetScroll(Byte scroll)
{
    if (m_firstWrite)
    {
        m_tempAddress &= ~0x1F;
        m_tempAddress |= (scroll >> 3) & 0x1F;
        m_fineXScroll  = scroll & 0x7;
        m_firstWrite = false;
    }
    else
    {
        m_tempAddress &= ~0x73E0;
        m_tempAddress |= ((scroll & 0x7) << 12) | ((scroll & 0xF8) << 2);
        m_firstWrite = true;
    }
}

///////////////////////////////////////////////////////////////////////////////
void PPU::SetData(Byte data)
{
    m_bus.Write(m_dataAddress, data);
    m_dataAddress += m_dataAddrIncrement;
}

///////////////////////////////////////////////////////////////////////////////
void PPU::SetOAMData(Byte data)
{
    m_spriteMemory[(m_spriteDataAddress++)] = data;
}

///////////////////////////////////////////////////////////////////////////////
Byte PPU::GetStatus(void)
{
    Byte status = m_spriteOverflow << 5 | m_sprZeroHit << 6 | m_vblank << 7;

    m_vblank = false;
    m_firstWrite = true;

    return (status);
}

///////////////////////////////////////////////////////////////////////////////
Byte PPU::GetData(void)
{
    Byte data = m_bus.Read(m_dataAddress);

    m_dataAddress += m_dataAddrIncrement;
    if (m_dataAddress < 0x3F00)
    {
        std::swap(data, m_dataBuffer);
    }

    return (data);
}

///////////////////////////////////////////////////////////////////////////////
Byte PPU::GetOAMData(void)
{
    return (m_spriteMemory[m_spriteDataAddress]);
}

} // !namespace NES
