///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Core/Controller.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace NES
///////////////////////////////////////////////////////////////////////////////
namespace NES
{

///////////////////////////////////////////////////////////////////////////////
Controller::Controller(void)
    : m_strobe(false)
    , m_keyStates(0)
    , m_keys(static_cast<size_t>(Button::TOTAL), sf::Keyboard::Unknown)
{
    // Initialize key mappings for the controller buttons
    m_keys[static_cast<int>(Button::A)] = sf::Keyboard::A;
    m_keys[static_cast<int>(Button::B)] = sf::Keyboard::Z;
    m_keys[static_cast<int>(Button::SELECT)] = sf::Keyboard::RShift;
    m_keys[static_cast<int>(Button::START)] = sf::Keyboard::Return;
    m_keys[static_cast<int>(Button::UP)] = sf::Keyboard::Up;
    m_keys[static_cast<int>(Button::DOWN)] = sf::Keyboard::Down;
    m_keys[static_cast<int>(Button::LEFT)] = sf::Keyboard::Left;
    m_keys[static_cast<int>(Button::RIGHT)] = sf::Keyboard::Right;
}

///////////////////////////////////////////////////////////////////////////////
void Controller::Strobe(Byte value)
{
    m_strobe = (value & 1);

    if (!m_strobe)
    {
        m_keyStates = 0;
        int shift = 0;

        for (int btn = 0; btn < static_cast<int>(Button::TOTAL); btn++)
        {
            m_keyStates |= (sf::Keyboard::isKeyPressed(m_keys[btn]) ? 1 : 0)
                << shift;
            shift++;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
Byte Controller::Read(void)
{
    Byte result = 0;

    // If strobe is active, return the first key state without shifting
    if (m_strobe)
    {
        result = (m_keyStates & 1);
    }
    else
    {
        result = (m_keyStates & 1);
        m_keyStates >>= 1;
    }

    return (result | 0x40);
}

///////////////////////////////////////////////////////////////////////////////
void Controller::SetKeyMapping(const std::vector<sf::Keyboard::Key>& keybinds)
{
    // Ensure the keybinds vector has the correct size
    if (keybinds.size() != static_cast<size_t>(Button::TOTAL))
    {
        return; // Invalid keybinds size
    }

    m_keys = keybinds;
}

} // !namespace NES
