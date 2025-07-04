///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Core/Audio/Noise.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace NES::Audio
///////////////////////////////////////////////////////////////////////////////
namespace NES::Audio
{

///////////////////////////////////////////////////////////////////////////////
Noise::Noise(void)
    : divider(0)
    , mode(Mode::Bit1)
    , period(0)
    , shiftRegister(1)
{}

///////////////////////////////////////////////////////////////////////////////
void Noise::SetPeriodFromTable(int index)
{
    static const int periods[] = {
        4, 8, 16, 32, 64, 96, 128, 160, 202,
        254, 380, 508, 762, 1016, 2034, 4068
    };

    divider.SetPeriod(periods[index]);
}

///////////////////////////////////////////////////////////////////////////////
void Noise::Clock(void)
{
    if (!divider.Clock())
    {
        return;
    }

    bool fi1 = (shiftRegister & 0x2) ?
        mode == Mode::Bit1 : (shiftRegister & 0x40);
    bool fi2 = (shiftRegister & 0x1);
    bool feedback = fi1 != fi2;

    shiftRegister = shiftRegister >> 1 | (feedback << 14);
}

///////////////////////////////////////////////////////////////////////////////
Byte Noise::Sample(void) const
{
    return (volume.Get());
}

} // !namespace NES::Audio
