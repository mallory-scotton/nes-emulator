///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Utils.hpp"
#include <SFML/Graphics.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace NES
///////////////////////////////////////////////////////////////////////////////
namespace NES
{

///////////////////////////////////////////////////////////////////////////////
/// \brief
///
///////////////////////////////////////////////////////////////////////////////
class Controller
{
public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Enumeration of controller buttons
    ///
    ///////////////////////////////////////////////////////////////////////////
    enum class Button
    {
        A,          //< A button
        B,          //< B button
        SELECT,     //< Select button
        START,      //< Start button
        UP,         //< Up direction
        DOWN,       //< Down direction
        LEFT,       //< Left direction
        RIGHT,      //< Right direction
        TOTAL       //< Total number of buttons
    };

private:
    ///////////////////////////////////////////////////////////////////////////
    // Private member variables
    ///////////////////////////////////////////////////////////////////////////
    bool m_strobe;                          //<! Strobe flag
    unsigned int m_keyStates;               //<! Bitmask for key states
    std::vector<sf::Keyboard::Key> m_keys;  //<! Key mapping for buttons

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Default constructor
    ///
    ///////////////////////////////////////////////////////////////////////////
    Controller(void);

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Sets the state of a button
    ///
    /// \param value The button value
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Strobe(Byte value);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Reads the state of a button
    ///
    /// \return The current state of the button
    ///
    ///////////////////////////////////////////////////////////////////////////
    Byte Read(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Sets the key mapping for the controller
    ///
    /// \param keybinds The key bindings for the controller
    ///
    ///////////////////////////////////////////////////////////////////////////
    void SetKeyMapping(const std::vector<sf::Keyboard::Key>& keybinds);
};

} // !namespace NES
