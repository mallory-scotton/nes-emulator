///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Core/Audio/APU.hpp"
#include <chrono>

///////////////////////////////////////////////////////////////////////////////
// Namespace NES
///////////////////////////////////////////////////////////////////////////////
namespace NES
{

///////////////////////////////////////////////////////////////////////////////
APU::APU(
    Audio::Player& player,
    IRQHandler& irq,
    std::function<Byte(Address)> callback
)
    : m_pulse1(1)
    , m_pulse2(2)
    , m_dmc(irq, callback)
    , m_counter(SetupFrameCounter(irq))
    , m_dividedByTwo(false)
    , m_queue(player.queue)
    , m_timer(std::chrono::nanoseconds(
        int64_t(1e9) / int64_t(player.outputSampleRate)
    ))
{}

///////////////////////////////////////////////////////////////////////////////
void APU::Step(void)
{}

///////////////////////////////////////////////////////////////////////////////
Byte APU::ReadStatus(void)
{
    // TODO: Implement APU status reading logic
    return (0x00);
}

///////////////////////////////////////////////////////////////////////////////
void APU::WriteRegister(Byte address, Byte value)
{
    (void)address; (void)value;
    // TODO: Implement APU register writing logic
}

///////////////////////////////////////////////////////////////////////////////
Audio::FrameCounter APU::SetupFrameCounter(IRQHandler& irq)
{
    return (Audio::FrameCounter(
        {
            std::ref(m_pulse1.volume),
            std::ref(m_pulse1.sweep),
            std::ref(m_pulse1.counter),

            std::ref(m_pulse2.volume),
            std::ref(m_pulse2.sweep),
            std::ref(m_pulse2.counter),

            std::ref(m_triangle.lengthCounter),
            std::ref(m_triangle.linearCounter),

            std::ref(m_noise.volume),
            std::ref(m_noise.counter)
        },
        irq
    ));
}

} // !namespace NES
