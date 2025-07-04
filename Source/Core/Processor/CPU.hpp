///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Core/Processor/MainBus.hpp"
#include "Core/Processor/IRQHandler.hpp"
#include "Core/Processor/OpCodes.hpp"
#include "Utils.hpp"
#include <list>

///////////////////////////////////////////////////////////////////////////////
// Namespace NES
///////////////////////////////////////////////////////////////////////////////
namespace NES
{

///////////////////////////////////////////////////////////////////////////////
/// \brief
///
///////////////////////////////////////////////////////////////////////////////
class CPU
{
private:
    ///////////////////////////////////////////////////////////////////////////
    // Private members
    ///////////////////////////////////////////////////////////////////////////
    int m_skipCycles;                        //<!
    int m_cycles;                           //<!
    Address r_pc;                           //<!
    Byte r_sp;                              //<!
    Byte r_a;                               //<!
    Byte r_x;                               //<!
    Byte r_y;                               //<!
    bool f_c;                               //<!
    bool f_z;                               //<!
    bool f_i;                               //<!
    bool f_d;                               //<!
    bool f_v;                               //<!
    bool f_n;                               //<!
    bool m_pendingNMI;                      //<!
    MainBus& m_bus;                         //<!
    int m_irqPulldowns;                     //<!
    std::list<IRQHandler> m_irqHandlers;    //<!

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Constructor for the CPU class
    ///
    /// \param bus Reference to the main bus
    ///
    ///////////////////////////////////////////////////////////////////////////
    CPU(MainBus& bus);

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Step through the CPU's instruction cycle
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Step(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Reset the CPU
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Reset(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Reset the CPU to a specific address
    ///
    /// \param address Address to reset the CPU to
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Reset(Address address);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the current program counter (PC)
    ///
    /// \return The current program counter (PC)
    ///
    ///////////////////////////////////////////////////////////////////////////
    Address GetPC(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Skip cycles for OAM DMA
    ///
    ///////////////////////////////////////////////////////////////////////////
    void SkipOAMDMACycles(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Skip cycles for DMC DMA
    ///
    ///////////////////////////////////////////////////////////////////////////
    void SkipDMCDMACycles(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Handle a Non-Maskable Interrupt (NMI)
    ///
    ///////////////////////////////////////////////////////////////////////////
    void NMIInterrupt(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Create an IRQHandler
    ///
    /// \return A reference to the IRQHandler
    ///
    ///////////////////////////////////////////////////////////////////////////
    IRQHandler& CreateIRQHandler(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Set the pulldown state for an IRQ
    ///
    /// \param bit Bit to set the pulldown for
    /// \param state State to set the pulldown to
    ///
    ///////////////////////////////////////////////////////////////////////////
    void SetIRQPulldown(int bit, bool state);

private:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Execute an interrupt sequence
    ///
    /// \param type Interrupt type
    ///
    ///////////////////////////////////////////////////////////////////////////
    void InterruptSequence(InterruptType type);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param opcode Opcode to execute
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool ExecuteImplied(Byte opcode);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Execute a branch instruction
    ///
    /// \param opcode Opcode to execute
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool ExecuteBranch(Byte opcode);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Execute a type 0 instruction
    ///
    /// \param opcode Opcode to execute
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool ExecuteType0(Byte opcode);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Execute a type 1 instruction
    ///
    /// \param opcode Opcode to execute
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool ExecuteType1(Byte opcode);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Execute a type 2 instruction
    ///
    /// \param opcode Opcode to execute
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool ExecuteType2(Byte opcode);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Read an address from the bus
    ///
    /// \param address Address to read
    ///
    /// \return The value read from the address
    ///
    ///////////////////////////////////////////////////////////////////////////
    Address ReadAddress(Address address);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Push a value onto the stack
    ///
    /// \param value Value to push onto the stack
    ///
    ///////////////////////////////////////////////////////////////////////////
    void PushStack(Byte value);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Pull a value from the stack
    ///
    /// \return The value popped from the stack
    ///
    ///////////////////////////////////////////////////////////////////////////
    Byte PullStack(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Skip cycles when crossing a page boundary
    ///
    /// \param a Address to skip from
    /// \param b Address to skip to
    ///
    ///////////////////////////////////////////////////////////////////////////
    void SkipPageCrossCycle(Address a, Address b);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Set the Zero and Negative flags based on a value
    ///
    /// \param value Value to set the carry flag to
    ///
    ///////////////////////////////////////////////////////////////////////////
    void SetZN(Byte value);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Check if an IRQ is pending
    ///
    /// \return True if an IRQ is pending, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool IsPendingIRQ(void) const;
};

} // !namespace NES
