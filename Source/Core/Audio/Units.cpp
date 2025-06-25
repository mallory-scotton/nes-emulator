///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Core/Audio/Units.hpp"
#include "Core/Audio/Pulse.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace NES::Audio
///////////////////////////////////////////////////////////////////////////////
namespace NES::Audio
{

///////////////////////////////////////////////////////////////////////////////
LengthCounter::LengthCounter(void)
    : halt(false)
    , enabled(false)
    , counter(0)
{}

///////////////////////////////////////////////////////////////////////////////
void LengthCounter::SetEnable(bool value)
{
    enabled = value;

    if (!enabled)
    {
        counter = 0;
    }
}

///////////////////////////////////////////////////////////////////////////////
bool LengthCounter::IsEnabled(void) const
{
    return (enabled);
}

///////////////////////////////////////////////////////////////////////////////
void LengthCounter::SetFromTable(size_t index)
{
    const static int table[] = {
        10, 254, 20, 2,  40, 4,  80, 6,  160, 8,  60, 10, 14, 12, 26, 14,
        12, 16,  24, 18, 48, 20, 96, 22, 192, 24, 72, 26, 16, 28, 32, 30
    };

    if (!enabled)
    {
        return;
    }

    counter = table[index];
}

///////////////////////////////////////////////////////////////////////////////
void LengthCounter::HalfFrameClock(void)
{
    if (halt || counter == 0)
    {
        return;
    }

    counter--;
}

///////////////////////////////////////////////////////////////////////////////
bool LengthCounter::IsMuted(void) const
{
    return (!enabled || counter == 0);
}

///////////////////////////////////////////////////////////////////////////////
LinearCounter::LinearCounter(void)
    : reload(false)
    , reloadValue(0)
    , control(true)
    , counter(0)
{}

///////////////////////////////////////////////////////////////////////////////
void LinearCounter::SetLinear(int value)
{
    reloadValue = value;
}

///////////////////////////////////////////////////////////////////////////////
void LinearCounter::QuarterFrameClock(void)
{
    if (reload)
    {
        counter = reloadValue;
        if (!control)
        {
            reload = false;
        }
    }

    if (counter == 0)
    {
        return;
    }

    counter--;
}

///////////////////////////////////////////////////////////////////////////////
Volume::Volume(void)
    : divider(0)
    , fixedVolume(0xF)
    , decayVolume(0xF)
    , constantVolume(true)
    , isLooping(false)
    , shouldStart(false)
{}

///////////////////////////////////////////////////////////////////////////////
void Volume::QuarterFrameClock(void)
{
    if (shouldStart)
    {
        shouldStart = false;
        decayVolume = 0xF;
        divider.SetPeriod(fixedVolume);
        return;
    }

    if (!divider.Clock())
    {
        return;
    }

    if (decayVolume > 0)
    {
        decayVolume--;
    }
    else if (isLooping)
    {
        decayVolume = 0xF;
    }
}

///////////////////////////////////////////////////////////////////////////////
int Volume::Get(void) const
{
    if  (constantVolume)
    {
        return (fixedVolume);
    }
    return (decayVolume);
}

///////////////////////////////////////////////////////////////////////////////
Sweep::Sweep(Pulse& pulse, bool onesComplement)
    : pulse(pulse)
    , period(0)
    , enabled(false)
    , reload(false)
    , negate(false)
    , shift(0)
    , onesComplement(onesComplement)
    , divider(0)
{}

///////////////////////////////////////////////////////////////////////////////
void Sweep::HalfFrameClock(void)
{
    if (reload)
    {
        divider.SetPeriod(period);
        reload = false;
        return;
    }

    if (!enabled || !divider.Clock())
    {
        return;
    }

    if (shift > 0)
    {
        int current = pulse.period;
        int target = CalculateTarget(current);

        if (!IsMuted(pulse.period, target))
        {
            pulse.SetPeriod(target);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
bool Sweep::IsMuted(int current, int target)
{
    return (current < 8 || target > 0x7FF);
}

///////////////////////////////////////////////////////////////////////////////
int Sweep::CalculateTarget(int current) const
{
    const int amount = current >> shift;

    if (!negate)
    {
        return (current + amount);
    }

    if (onesComplement)
    {
        return (std::max(0, current - amount - 1));
    }
    return (std::max(0, current - amount));
}

///////////////////////////////////////////////////////////////////////////////
bool PulseDuty::Active(Type cycle, int index)
{
    const bool sequences[] = {
        0, 0, 0, 0, 0, 0, 0, 1, // 12.5%
        0, 0, 0, 0, 0, 0, 1, 1, // 25.0%
        0, 0, 0, 0, 1, 1, 1, 1, // 50.0%
        1, 1, 1, 1, 1, 1, 0, 0, // 25.0% negated
    };
    return (sequences[static_cast<int>(cycle) * length + index]);
}

} // !namespace NES::Audio
