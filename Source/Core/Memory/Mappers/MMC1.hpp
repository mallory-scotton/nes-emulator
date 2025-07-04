///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Core/Memory/Mapper.hpp"
#include "Core/Enums.hpp"
#include <functional>
#include <vector>

///////////////////////////////////////////////////////////////////////////////
// Namespace NES::Mappers
///////////////////////////////////////////////////////////////////////////////
namespace NES::Mappers
{

///////////////////////////////////////////////////////////////////////////////
/// \brief
///
///////////////////////////////////////////////////////////////////////////////
class MMC1 : public Mapper
{
private:
    ///////////////////////////////////////////////////////////////////////////
    //
    ///////////////////////////////////////////////////////////////////////////
    std::function<void(void)> m_callback;   //<! Callback for mirroring
    MirroringType m_mirroring;              //<! Mirroring type
    bool m_usesCHRRAM;                      //<! Indicate if CHR RAM is used
    int m_modeCHR;                          //<! CHR mode
    int m_modePGR;                          //<! PGR mode
    Byte m_register;                        //<! Register for MMC1
    int m_writeCount;                       //<! Count of writes to the register
    Byte m_regPGR;                          //<! Register for PGR
    Byte m_regCHR[2];                       //<! Registers for CHR
    const Byte* m_bankPGR[2];               //<! PGR banks
    int m_bankCHRIndex[2];                  //<! Index for CHR bank
    std::vector<Byte> m_ram;                //<! RAM for MMC1 CHR

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param cartridge
    /// \param callback
    ///
    ///////////////////////////////////////////////////////////////////////////
    MMC1(Cartridge& cartridge, std::function<void(void)> callback);

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param address
    ///
    /// \return
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual Byte ReadPGR(Address address) override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param address
    /// \param value
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual void WritePGR(Address address, Byte value) override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param address
    ///
    /// \return
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual Byte ReadCHR(Address address) override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param address
    /// \param value
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual void WriteCHR(Address address, Byte value) override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \return
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual MirroringType GetMirroringType(void) const override;

private:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    ///////////////////////////////////////////////////////////////////////////
    void CalculatePGRPointers(void);
};

} // !namespace NES::Mappers
