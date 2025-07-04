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

///////////////////////////////////////////////////////////////////////////////
// Namespace NES::Audio
///////////////////////////////////////////////////////////////////////////////
namespace NES::Audio
{

///////////////////////////////////////////////////////////////////////////////
/// \brief
///
///////////////////////////////////////////////////////////////////////////////
class Triangle
{
public:
    ///////////////////////////////////////////////////////////////////////////
    // Public members
    ///////////////////////////////////////////////////////////////////////////
    LengthCounter lengthCounter;    //<!
    LinearCounter linearCounter;    //<!
    Uint32 seqIndex;                //<!
    Divider sequencer;              //<!
    int period;                     //<!

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    ///////////////////////////////////////////////////////////////////////////
    Triangle(void);

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

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \return
    ///
    ///////////////////////////////////////////////////////////////////////////
    int Volume(void) const;
};

} // !namespace NES::Audio
