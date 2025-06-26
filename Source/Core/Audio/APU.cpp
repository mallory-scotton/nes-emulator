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
float APU::Mix(Byte pulse1, Byte pulse2, Byte triangle, Byte noise, Byte dmc)
{
    float pulse1out = static_cast<float>(pulse1);
    float pulse2out = static_cast<float>(pulse2);

    float pulse_out = 0;
    if (pulse1out + pulse2out != 0)
    {
        pulse_out = 95.88 / ((8128.0 / (pulse1out + pulse2out)) + 100.0);
    }

    float tnd_out = 0;
    float triangleout = static_cast<float>(triangle);
    float noiseout = static_cast<float>(noise);
    float dmcout = static_cast<float>(dmc);

    if (triangleout + noiseout + dmcout != 0)
    {
        float tnd_sum =
            (triangleout / 8227.0) +
            (noiseout / 12241.0) +
            (dmcout / 22638.0);
        tnd_out = 159.79 / (1.0 / tnd_sum + 100.0);
    }

    return (pulse_out + tnd_out);
}

///////////////////////////////////////////////////////////////////////////////
void APU::Step(void)
{
    m_noise.Clock();
    m_dmc.Clock();
    m_triangle.Clock();

    if (m_dividedByTwo)
    {
        m_counter.Clock();
        m_pulse1.Clock();
        m_pulse2.Clock();
        m_queue.Push(Mix(
            m_pulse1.Sample(),
            m_pulse2.Sample(),
            m_triangle.Sample(),
            m_noise.Sample(),
            m_dmc.Sample()
        ));
    }
    m_dividedByTwo = !m_dividedByTwo;
}

///////////////////////////////////////////////////////////////////////////////
Byte APU::ReadStatus(void)
{
    bool frameInterrupt = m_counter.frameInterrupt;
    m_counter.ClearFrameInterrupt();

    bool dmcInterrupt = m_dmc.interrupt;
    m_dmc.ClearInterrupt();

    return (
        (!m_pulse1.counter.IsMuted()) << 0 |
        (!m_pulse2.counter.IsMuted()) << 1 |
        (!m_triangle.lengthCounter.IsMuted()) << 2 |
        (!m_noise.counter.IsMuted()) << 3 |
        (!m_dmc.HasMoreSamples()) << 4 |
        frameInterrupt << 6 | dmcInterrupt << 7
    );
}

///////////////////////////////////////////////////////////////////////////////
void APU::WriteRegister(Address address, Byte value)
{
    switch (address)
    {
    case APU_SQ1_VOL:
    {
        m_pulse1.volume.fixedVolume = value & 0xf;
        m_pulse1.volume.constantVolume = value & (1 << 4);
        m_pulse1.volume.isLooping = m_pulse1.counter.halt = value & (1 << 5);
        m_pulse1.seqType = static_cast<Audio::PulseDuty::Type>(value >> 6);
        break;
    }

    case APU_SQ1_SWEEP:
    {
        m_pulse1.sweep.enabled = value & (1 << 7);
        m_pulse1.sweep.period = (value >> 4) & 0x7;
        m_pulse1.sweep.negate = value & (1 << 3);
        m_pulse1.sweep.shift = value & 0x7;
        m_pulse1.sweep.reload = true;
        break;
    }

    case APU_SQ1_LO:
    {
        int period = (m_pulse1.period & 0xff00) | value;
        m_pulse1.SetPeriod(period);
        break;
    }

    case APU_SQ1_HI:
    {
        int period = (m_pulse1.period & 0x00ff) | ((value & 0x7) << 8);
        m_pulse1.counter.SetFromTable(value >> 3);
        m_pulse1.seqIndex = 0;
        m_pulse1.volume.shouldStart = true;
        m_pulse1.SetPeriod(period);
        break;
    }

    case APU_SQ2_VOL:
    {
        m_pulse2.volume.fixedVolume = value & 0xf;
        m_pulse2.volume.constantVolume = value & (1 << 4);
        m_pulse2.volume.isLooping = m_pulse2.counter.halt = value & (1 << 5);
        m_pulse2.seqType = static_cast<Audio::PulseDuty::Type>(value >> 6);
        break;
    }

    case APU_SQ2_SWEEP:
    {
        m_pulse2.sweep.enabled = value & (1 << 7);
        m_pulse2.sweep.period = (value >> 4) & 0x7;
        m_pulse2.sweep.negate = value & (1 << 3);
        m_pulse2.sweep.shift = value & 0x7;
        m_pulse2.sweep.reload = true;
        break;
    }

    case APU_SQ2_LO:
    {
        int period = (m_pulse2.period & 0xff00) | value;
        m_pulse2.SetPeriod(period);
        break;
    }

    case APU_SQ2_HI:
    {
        int period = (m_pulse2.period & 0x00ff) | ((value & 0x7) << 8);
        m_pulse2.counter.SetFromTable(value >> 3);
        m_pulse2.seqIndex = 0;
        m_pulse2.SetPeriod(period);
        m_pulse2.volume.shouldStart = true;
        break;
    }

    case APU_TRI_LINEAR:
    {
        m_triangle.linearCounter.SetLinear(value & 0x7f);
        m_triangle.linearCounter.reload  = true;
        m_triangle.linearCounter.control = m_triangle.lengthCounter.halt = 1 >> 7;
        break;
    }

    case APU_TRI_LO:
    {
        int period = (m_triangle.period & 0xff00) | value;
        m_triangle.SetPeriod(period);
        break;
    }

    case APU_TRI_HI:
    {
        int period = (m_triangle.period & 0x00ff) | ((value & 0x7) << 8);
        m_triangle.lengthCounter.SetFromTable(value >> 3);
        m_triangle.SetPeriod(period);
        m_triangle.linearCounter.reload = true;
        break;
    }

    case APU_NOISE_VOL:
    {
        m_noise.volume.fixedVolume = value & 0xf;
        m_noise.volume.constantVolume = value & (1 << 4);
        m_noise.volume.isLooping = m_noise.counter.halt = value & (1 << 5);
        break;
    }

    case APU_NOISE_LO:
    {
        m_noise.mode = static_cast<Audio::Noise::Mode>(value & (1 << 7));
        m_noise.SetPeriodFromTable(value & 0xf);
        break;
    }

    case APU_NOISE_HI:
    {
        m_noise.counter.SetFromTable(value >> 3);
        m_noise.volume.divider.Reset();
        break;
    }

    case APU_DMC_FREQ:
    {
        m_dmc.irqEnable = value >> 7;
        m_dmc.loop = value >> 6;
        m_dmc.SetRate(value & 0xf);
        break;
    }

    case APU_DMC_RAW:
    {
        m_dmc.volume = value & 0x7f;
        break;
    }

    case APU_DMC_START:
    {
        m_dmc.sampleBegin = 0xc000 | (value << 6);
        break;
    }

    case APU_DMC_LEN:
    {
        m_dmc.sampleLength = (value << 4) | 1;
        break;
    }

    case APU_CONTROL:
    {
        m_pulse1.counter.SetEnable(value & 0x1);
        m_pulse2.counter.SetEnable(value & 0x2);
        m_triangle.lengthCounter.SetEnable(value & 0x4);
        m_noise.counter.SetEnable(value & 0x8);
        m_dmc.Control(value & 0x10);
        break;
    }

    case APU_FRAME_CONTROL:
    {
        m_counter.Reset(
            static_cast<Audio::FrameCounter::Mode>(value >> 7),
            value >> 6
        );
        break;
    }
    }
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
