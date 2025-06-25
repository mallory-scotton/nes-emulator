///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Core/Audio/DMC.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace NES::Audio
///////////////////////////////////////////////////////////////////////////////
namespace NES::Audio
{

///////////////////////////////////////////////////////////////////////////////
DMC::DMC(IRQHandler& irq, std::function<Byte(Address)> dma)
    : irqEnable(false)
    , loop(false)
    , volume(0)
    , changeEnabled(false)
    , changeRate(0)
    , sampleBegin(0)
    , sampleLength(0)
    , remainingBytes(0)
    , currentAddress(0)
    , sampleBuffer(0)
    , shifter(0)
    , remainingBits(0)
    , silenced(false)
    , interrupt(false)
    , m_irq(irq)
    , m_dma(dma)
{}

///////////////////////////////////////////////////////////////////////////////
void DMC::SetIRQEnable(bool enable)
{
    irqEnable = enable;
    if (!irqEnable)
    {
        interrupt = false;
        m_irq.Release();
    }
}

///////////////////////////////////////////////////////////////////////////////
void DMC::SetRate(int index)
{
    const static int rates[] = {
        428, 380, 340, 320, 286, 254, 226, 214,
        190, 160, 142, 128, 106, 84, 72, 54
    };

    changeRate.SetPeriod(rates[index]);
    changeRate.Reset();
}

///////////////////////////////////////////////////////////////////////////////
void DMC::Control(bool enable)
{
    changeEnabled = enable;

    if (!enable)
    {
        remainingBytes = 0;
    }
    else if (remainingBytes == 0)
    {
        currentAddress = sampleBegin;
        remainingBytes = sampleLength;
    }
}

///////////////////////////////////////////////////////////////////////////////
void DMC::ClearInterrupt(void)
{
    m_irq.Release();
    interrupt = false;
}

///////////////////////////////////////////////////////////////////////////////
void DMC::Clock(void)
{
    if (!changeEnabled || !changeRate.Clock())
    {
        return;
    }

    int delta = PopDelta();

    if (silenced)
    {
        return;
    }

    if (delta == 1 && volume <= 125)
    {
        volume += 2;
    }
    else if (delta == 0 && volume >= 2)
    {
        volume -= 2;
    }
}

///////////////////////////////////////////////////////////////////////////////
Byte DMC::Sample(void) const
{
    return (volume);
}

///////////////////////////////////////////////////////////////////////////////
bool DMC::HasMoreSamples(void) const
{
    return (remainingBytes > 0);
}

///////////////////////////////////////////////////////////////////////////////
bool DMC::LoadSample(void)
{
    if (remainingBytes == 0)
    {
        if (!loop)
        {
            if (irqEnable)
            {
                interrupt = true;
                m_irq.Pull();
            }

            return (false);
        }

        currentAddress = sampleBegin;
        remainingBytes = sampleLength;
    }
    else
    {
        remainingBytes--;
    }

    sampleBuffer = m_dma(currentAddress);

    if (currentAddress == 0xFFFF)
    {
        currentAddress = 0x8000;
    }
    else
    {
        currentAddress++;
    }
    return (true);
}

///////////////////////////////////////////////////////////////////////////////
int DMC::PopDelta(void)
{
    if (remainingBits == 0)
    {
        remainingBits = 8;

        if (LoadSample())
        {
            shifter = sampleBuffer;
            silenced = false;
        }
        else
        {
            silenced = true;
        }
    }
    else
    {
        remainingBits--;
    }

    int rv = shifter & 0x1;
    shifter >>= 1;
    return (rv);
}

} // !namespace NES::Audio
