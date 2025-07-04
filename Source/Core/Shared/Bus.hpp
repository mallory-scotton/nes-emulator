///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Utils.hpp"
#include "Core/Memory/Mapper.hpp"
#include <memory>

///////////////////////////////////////////////////////////////////////////////
// Namespace NES
///////////////////////////////////////////////////////////////////////////////
namespace NES
{

///////////////////////////////////////////////////////////////////////////////
/// \brief Interface for the NES bus
///
///////////////////////////////////////////////////////////////////////////////
class Bus
{
protected:
    ///////////////////////////////////////////////////////////////////////////
    // Protected members
    ///////////////////////////////////////////////////////////////////////////
    std::shared_ptr<Mapper> m_mapper;   //<! Pointer to the current mapper

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Default constructor for the Bus class
    ///
    ///////////////////////////////////////////////////////////////////////////
    Bus(void) : m_mapper(nullptr) {}

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Destructor for the Bus class
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual ~Bus() = default;

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Read a byte from the bus
    ///
    /// \param address Address to read from
    ///
    /// \return The byte read from the bus
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual Byte Read(Address address) = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Write a byte to the bus
    ///
    /// \param address Address to write to
    /// \param value Value to write
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual void Write(Address address, Byte value) = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Set the mapper for the bus
    ///
    /// \param mapper Shared pointer to the mapper to set
    ///
    /// \return True if the mapper was set successfully, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual bool SetMapper(std::shared_ptr<Mapper> mapper) = 0;
};

} // !namespace NES
