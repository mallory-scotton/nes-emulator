///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Utils.hpp"
#include "Core/Audio/Divider.hpp"
#include "Core/Audio/Units.hpp"
#include "Core/Processor/IRQHandler.hpp"
#include <functional>

///////////////////////////////////////////////////////////////////////////////
// Namespace NES::Audio
///////////////////////////////////////////////////////////////////////////////
namespace NES::Audio
{

///////////////////////////////////////////////////////////////////////////////
/// \brief
///
///////////////////////////////////////////////////////////////////////////////
class DMC
{
public:
    ///////////////////////////////////////////////////////////////////////////
    // Public members
    ///////////////////////////////////////////////////////////////////////////
    bool irqEnable;                     //<!
    bool loop;                          //<!
    int volume;                         //<!
    bool changeEnabled;                 //<!
    Divider changeRate;                 //<!
    Address sampleBegin;                //<!
    int sampleLength;                   //<!
    int remainingBytes;                 //<!
    Address currentAddress;             //<!
    Byte sampleBuffer;                  //<!
    int shifter;                        //<!
    int remainingBits;                  //<!
    bool silenced;                      //<!
    bool interrupt;                     //<!

private:
    ///////////////////////////////////////////////////////////////////////////
    // Private members
    ///////////////////////////////////////////////////////////////////////////
    IRQHandler& m_irq;                  //<! Reference to the IRQ handler
    std::function<Byte(Address)> m_dma; //<! Function to read from memory

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Constructor for the DMC class
    ///
    /// \param irq Reference to the IRQ handler
    /// \param dma Function to read from memory
    ///
    ///////////////////////////////////////////////////////////////////////////
    DMC(IRQHandler& irq, std::function<Byte(Address)> dma);

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param enable
    ///
    ///////////////////////////////////////////////////////////////////////////
    void SetIRQEnable(bool enable);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param index
    ///
    ///////////////////////////////////////////////////////////////////////////
    void SetRate(int index);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param enable
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Control(bool enable);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    ///////////////////////////////////////////////////////////////////////////
    void ClearInterrupt(void);

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
    bool HasMoreSamples(void) const;

private:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \return
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool LoadSample(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \return
    ///
    ///////////////////////////////////////////////////////////////////////////
    int PopDelta(void);
};

} // !namespace NES::Audio
