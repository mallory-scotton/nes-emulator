///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Core/Memory/Mapper.hpp"
#include "Core/Cartridge.hpp"
#include "Core/Memory/Mappers/AxROM.hpp"
#include "Core/Memory/Mappers/CNROM.hpp"
#include "Core/Memory/Mappers/GxROM.hpp"
#include "Core/Memory/Mappers/MMC1.hpp"
#include "Core/Memory/Mappers/MMC3.hpp"
#include "Core/Memory/Mappers/NROM.hpp"
#include "Core/Memory/Mappers/UxROM.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace NES
///////////////////////////////////////////////////////////////////////////////
namespace NES
{

///////////////////////////////////////////////////////////////////////////////
Mapper::Mapper(Cartridge& cartridge, Uint8 id)
    : id(id)
    , m_cartridge(cartridge)
{}

///////////////////////////////////////////////////////////////////////////////
const Cartridge& Mapper::GetCartridge(void) const
{
    return (m_cartridge);
}

///////////////////////////////////////////////////////////////////////////////
void Mapper::ScanlineIRQ(void)
{}

///////////////////////////////////////////////////////////////////////////////
std::unique_ptr<Mapper> Mapper::CreateMapper(
    Uint8 type,
    Cartridge& cartridge,
    IRQHandler& irq,
    std::function<void(void)> callback
)
{
    switch (type)
    {
    case 0:
        return (std::make_unique<Mappers::NROM>(cartridge));
    case 1:
        return (std::make_unique<Mappers::MMC1>(cartridge, callback));
    case 2:
        return (std::make_unique<Mappers::UxROM>(cartridge));
    case 3:
        return (std::make_unique<Mappers::CNROM>(cartridge));
    case 4:
        return (std::make_unique<Mappers::MMC3>(cartridge));
    case 7:
        return (std::make_unique<Mappers::AxROM>(cartridge, callback));
    case 66:
        return (std::make_unique<Mappers::GxROM>(cartridge, callback));
    default:
        return (nullptr);
    };
}

} // !namespace NES
