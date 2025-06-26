///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Core/Emulator.hpp"
#include <functional>
#include <algorithm>
#include <iostream>

///////////////////////////////////////////////////////////////////////////////
// Namespace NES
///////////////////////////////////////////////////////////////////////////////
namespace NES
{

///////////////////////////////////////////////////////////////////////////////
Emulator::Emulator(const Path& romPath)
    : m_cartridge(romPath)
    , m_cpu(m_mbus)
    , m_player(static_cast<int>(
        1.0 / std::chrono::duration_cast<
            std::chrono::duration<double>
        >(std::chrono::nanoseconds(1118)).count()
    ))
    , m_ppu(m_pbus)
    , m_apu(
        m_player,
        m_cpu.CreateIRQHandler(),
        std::bind(&Emulator::DMCDMACallback, this, std::placeholders::_1)
    )
    , m_mbus(
        m_ppu, m_apu, m_ctrl1, m_ctrl2,
        std::bind(&Emulator::OAMDMACallback, this, std::placeholders::_1)
    )
    , m_lastWakeUp(std::chrono::high_resolution_clock::now())
    , m_elapsedTime(m_lastWakeUp - m_lastWakeUp)
    , m_paused(false)
{
    m_mapper = NES::Mapper::CreateMapper(
        m_cartridge.GetMapper(), m_cartridge, m_cpu.CreateIRQHandler(),
        [&](void)
        {
            m_pbus.UpdateMirroring();
        }
    );

    if (!m_mapper)
    {
        throw std::runtime_error("Failed to create mapper!");
    }

    if (!m_pbus.SetMapper(m_mapper) || !m_mbus.SetMapper(m_mapper))
    {
        throw std::runtime_error("Failed to set mapper!");
    }

    m_ppu.SetVBlankCallback([&]()
    {
        m_cpu.NMIInterrupt();
    });

    m_cpu.Reset();
    m_ppu.Reset();

    m_player.Start();
}

///////////////////////////////////////////////////////////////////////////////
void Emulator::TogglePause(void)
{
    m_paused = !m_paused;
    if (!m_paused)
    {
        m_lastWakeUp = std::chrono::high_resolution_clock::now();
    }
}

///////////////////////////////////////////////////////////////////////////////
bool Emulator::IsPaused(void) const
{
    return (m_paused);
}

///////////////////////////////////////////////////////////////////////////////
void Emulator::SkipOneCycle(void)
{
    for (int i = 0; i < 29781; i++)
    {
        m_ppu.Step();
        m_ppu.Step();
        m_ppu.Step();

        m_cpu.Step();

        m_apu.Step();
    }
}

///////////////////////////////////////////////////////////////////////////////
void Emulator::Update(void)
{
    if (m_paused)
    {
        return;
    }

    const auto now = std::chrono::high_resolution_clock::now();
    m_elapsedTime += now - m_lastWakeUp;
    m_lastWakeUp = now;

    while (m_elapsedTime > std::chrono::nanoseconds(559))
    {
        m_ppu.Step();
        m_ppu.Step();
        m_ppu.Step();

        m_cpu.Step();

        m_apu.Step();

        m_elapsedTime -= std::chrono::nanoseconds(559);
    }
}

///////////////////////////////////////////////////////////////////////////////
const NES::Byte* Emulator::GetScreenData(void) const
{
    return (m_ppu.GetScreen().data());
}

///////////////////////////////////////////////////////////////////////////////
Byte Emulator::DMCDMACallback(Address address)
{
    m_cpu.SkipDMCDMACycles();
    return (m_pbus.Read(address));
}

///////////////////////////////////////////////////////////////////////////////
void Emulator::OAMDMACallback(Byte page)
{
    m_cpu.SkipOAMDMACycles();
    auto pagePtr = m_mbus.GetPagePtr(page);
    if (pagePtr != nullptr)
    {
        m_ppu.DoDMA(pagePtr);
    }
}

} // !namespace NES
