///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Namespace NES
///////////////////////////////////////////////////////////////////////////////
namespace NES
{

///////////////////////////////////////////////////////////////////////////////
/// \brief Enumeration used to define the mirroring type of the NES
///
/// This enumeration is used to specify how the memory mirroring is handled
/// in the NES system. It defines four types of mirroring:
/// - HORIZONTAL:   The memory is mirrored horizontally, meaning the left side
///                 of the screen is a mirror image of the right side.
/// - VERTICAL:     The memory is mirrored vertically, meaning the top half of
///                 the screen is a mirror image of the bottom half.
/// - FOUR_SCREEN:  The memory is mirrored in four different ways, allowing
///                 for more complex graphics.
/// - SINGLE_SCREEN:The memory is mirrored in a single way, meaning there
///                 is no mirroring at all.
///
///////////////////////////////////////////////////////////////////////////////
enum class MirroringType
{
    HORIZONTAL,     //<! Horizontal mirroring
    VERTICAL,       //<! Vertical mirroring
    FOUR_SCREEN,    //<! Four-screen mirroring
    SINGLE_SCREEN   //<! Single-screen mirroring
};

} // !namespace NES
