///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Core/Shared/Bus.hpp"
#include "Core/Picture/PPU.hpp"
#include "Core/Audio/APU.hpp"
#include "Core/Controller.hpp"
#include <vector>
#include <functional>

///////////////////////////////////////////////////////////////////////////////
// Namespace NES
///////////////////////////////////////////////////////////////////////////////
namespace NES
{

///////////////////////////////////////////////////////////////////////////////
/// \brief Main bus for the NES
///
/// This class represents the main bus of the NES, which handles communication
/// between the CPU, PPU, APU, and other components. It defines the
/// memory-mapped registers used by the NES system.
///
///////////////////////////////////////////////////////////////////////////////
class MainBus : public Bus
{
public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Enumeration of memory-mapped registers
    ///
    ///////////////////////////////////////////////////////////////////////////
    enum Register
    {
        PPU_CTRL = 0x2000,
        PPU_MASK = 0x2001,
        PPU_STATUS = 0x2002,
        OAM_ADDR = 0x2003,
        OAM_DATA = 0x2004,
        PPU_SCROLL = 0x2005,
        PPU_ADDR = 0x2006,
        PPU_DATA = 0x2007,
        APU_REGISTER_START = 0x4000,
        APU_REGISTER_END = 0x4013,
        OAM_DMA = 0x4014,
        APU_CONTROL_AND_STATUS = 0x4015,
        JOY1 = 0x4016,
        JOY2_AND_FRAME_CONTROL = 0x4017
    };

private:
    ///////////////////////////////////////////////////////////////////////////
    // Private members
    ///////////////////////////////////////////////////////////////////////////
    std::vector<Byte> m_ram;                    //<! Internal RAM
    std::vector<Byte> m_extRam;                 //<! External RAM
    std::function<void(Byte)> m_dmaCallback;    //<! DMA callback function
    PPU& m_ppu;                                 //<! Reference to the PPU
    APU& m_apu;                                 //<! Reference to the APU
    Controller& m_controller1;                  //<! First controller
    Controller& m_controller2;                  //<! Second controller

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param ppu
    /// \param apu
    /// \param controller1
    /// \param controller2
    /// \param callback
    ///
    ///////////////////////////////////////////////////////////////////////////
    MainBus(
        PPU& ppu,
        APU& apu,
        Controller& controller1,
        Controller& controller2,
        std::function<void(Byte)> callback
    );

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Read a byte from the bus
    ///
    /// \param address Address to read from
    ///
    /// \return The byte read from the bus
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual Byte Read(Address address) override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Write a byte to the bus
    ///
    /// \param address Address to write to
    /// \param value Value to write
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual void Write(Address address, Byte value) override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Set the mapper for the bus
    ///
    /// \param mapper Shared pointer to the mapper to set
    ///
    /// \return True if the mapper was set successfully, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual bool SetMapper(std::shared_ptr<Mapper> mapper) override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the pointer to a specific page in the RAM
    ///
    /// \param page Page number to get the pointer for
    ///
    /// \return Pointer to the start of the specified page in RAM
    ///
    ///////////////////////////////////////////////////////////////////////////
    const Byte* GetPagePtr(Byte page);

private:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Normalize a mirror address
    ///
    /// \param address Address to normalize
    ///
    /// \return The normalized address
    ///
    ///////////////////////////////////////////////////////////////////////////
    static Address NormalizeMirror(Address address);
};

} // !namespace NES
