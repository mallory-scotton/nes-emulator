///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Utils.hpp"
#include "Core/Audio/Noise.hpp"
#include "Core/Audio/Triangle.hpp"
#include "Core/Audio/Pulse.hpp"
#include "Core/Audio/Timer.hpp"
#include "Core/Audio/DMC.hpp"
#include "Core/Audio/FrameCounter.hpp"
#include "Core/Audio/Player.hpp"
#include "Core/Processor/IRQHandler.hpp"
#include <functional>

///////////////////////////////////////////////////////////////////////////////
// Namespace NES
///////////////////////////////////////////////////////////////////////////////
namespace NES
{

///////////////////////////////////////////////////////////////////////////////
/// \brief
///
///////////////////////////////////////////////////////////////////////////////
class APU
{
private:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    ///////////////////////////////////////////////////////////////////////////
    enum Register
    {
        APU_SQ1_VOL       = 0x4000,
        APU_SQ1_SWEEP     = 0x4001,
        APU_SQ1_LO        = 0x4002,
        APU_SQ1_HI        = 0x4003,

        APU_SQ2_VOL       = 0x4004,
        APU_SQ2_SWEEP     = 0x4005,
        APU_SQ2_LO        = 0x4006,
        APU_SQ2_HI        = 0x4007,

        APU_TRI_LINEAR    = 0x4008,
        // unused - 0x4009
        APU_TRI_LO        = 0x400a,
        APU_TRI_HI        = 0x400b,

        APU_NOISE_VOL     = 0x400c,
        // unused - 0x400d
        APU_NOISE_LO      = 0x400e,
        APU_NOISE_HI      = 0x400f,

        APU_DMC_FREQ      = 0x4010,
        APU_DMC_RAW       = 0x4011,
        APU_DMC_START     = 0x4012,
        APU_DMC_LEN       = 0x4013,

        APU_CONTROL       = 0x4015,

        APU_FRAME_CONTROL = 0x4017,
    };

private:
    ///////////////////////////////////////////////////////////////////////////
    // Private members
    ///////////////////////////////////////////////////////////////////////////
    Audio::Pulse m_pulse1;              //<! Square wave channel 1
    Audio::Pulse m_pulse2;              //<! Square wave channel 2
    Audio::Triangle m_triangle;         //<! Triangle wave channel
    Audio::Noise m_noise;               //<! Noise channel
    Audio::DMC m_dmc;                   //<! Delta Modulation Channel
    Audio::FrameCounter m_counter;      //<! Frame counter for APU timing
    bool m_dividedByTwo;                //<! Indicate if it is divided by two
    Audio::RingBuffer<float>& m_queue;  //<! Audio queue for the player
    Audio::Timer m_timer;               //<! Timer for APU operations

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param player
    /// \param irq
    /// \param callback
    ///
    ///////////////////////////////////////////////////////////////////////////
    APU(
        Audio::Player& player,
        IRQHandler& irq,
        std::function<Byte(Address)> callback
    );

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Step(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \return
    ///
    ///////////////////////////////////////////////////////////////////////////
    Byte ReadStatus(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param address
    /// \param value
    ///
    ///////////////////////////////////////////////////////////////////////////
    void WriteRegister(Address address, Byte value);

private:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param irq
    ///
    /// \return
    ///
    ///////////////////////////////////////////////////////////////////////////
    Audio::FrameCounter SetupFrameCounter(IRQHandler& irq);

private:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param pulse1
    /// \param pulse2
    /// \param triangle
    /// \param noise
    /// \param dmc
    ///
    /// \return
    ///
    ///////////////////////////////////////////////////////////////////////////
    static float Mix(
        Byte pulse1,
        Byte pulse2,
        Byte triangle,
        Byte noise,
        Byte dmc
    );
};

} // !namespace NES
