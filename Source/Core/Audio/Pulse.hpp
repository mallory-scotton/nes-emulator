///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Utils.hpp"
#include "Core/Audio/Units.hpp"
#include "Core/Audio/Divider.hpp"
#include <vector>
#include <string>

///////////////////////////////////////////////////////////////////////////////
// Namespace NES::Audio
///////////////////////////////////////////////////////////////////////////////
namespace NES::Audio
{

///////////////////////////////////////////////////////////////////////////////
/// \brief
///
///////////////////////////////////////////////////////////////////////////////
class Pulse
{
public:
    ///////////////////////////////////////////////////////////////////////////
    // Public members
    ///////////////////////////////////////////////////////////////////////////
    Volume volume;              //<!
    LengthCounter counter;      //<!
    size_t seqIndex;            //<!
    PulseDuty::Type seqType;    //<!
    Divider sequencer;          //<!
    int type;                   //<!
    int period;                 //<!
    Sweep sweep;                //<!

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param type
    ///
    ///////////////////////////////////////////////////////////////////////////
    Pulse(int type);

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param period
    ///
    ///////////////////////////////////////////////////////////////////////////
    void SetPeriod(int period);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Clock(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \return
    ///
    ///////////////////////////////////////////////////////////////////////////
    Byte Sample(void) const;
};

} // !namespace NES::Audio
