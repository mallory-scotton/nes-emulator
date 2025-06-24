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
/// - HORIZONTAL: The memory is mirrored horizontally, meaning the left side
///               of the screen is a mirror image of the right side.
/// - VERTICAL: The memory is mirrored vertically, meaning the top half of
///             the screen is a mirror image of the bottom half.
/// - FOUR_SCREEN: The memory is mirrored in four different ways, allowing
///                for more complex graphics.
/// - LOWER_SINGLE_SCREEN: The memory is mirrored to only the lower half of
///                        the screen, allowing for a single-screen display.
/// - UPPER_SINGLE_SCREEN: The memory is mirrored to only the upper half of
///                        the screen, allowing for a single-screen display.
///
///////////////////////////////////////////////////////////////////////////////
enum class MirroringType
{
    HORIZONTAL = 0,         //<! Horizontal mirroring
    VERTICAL = 1,           //<! Vertical mirroring
    FOUR_SCREEN = 8,        //<! Four-screen mirroring
    LOWER_SINGLE_SCREEN,    //<! Lower Single-screen mirroring
    UPPER_SINGLE_SCREEN     //<! Upper Single-screen mirroring
};

} // !namespace NES
