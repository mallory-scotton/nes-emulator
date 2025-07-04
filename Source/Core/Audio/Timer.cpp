///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Core/Audio/Timer.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace NES::Audio
///////////////////////////////////////////////////////////////////////////////
namespace NES::Audio
{

///////////////////////////////////////////////////////////////////////////////
Timer::Timer(std::chrono::nanoseconds period)
    : period(period)
    , m_leftover(0)
{}

///////////////////////////////////////////////////////////////////////////////
int Timer::Clock(std::chrono::nanoseconds elapsed)
{
    m_leftover += elapsed;

    if (m_leftover < elapsed)
    {
        return (0);
    }
    int64_t cycles = m_leftover / period;
    m_leftover = m_leftover % period;
    return (static_cast<int>(cycles));
}

} // !namespace NES::Audio
