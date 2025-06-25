///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "NES.hpp"
#include "Utils/Constants.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <chrono>

void sfml(const std::string& romPath)
{
    sf::RenderWindow window(
        sf::VideoMode(NES::NES_WIDTH * 3, NES::NES_HEIGHT * 3),
        "NES Emulator",
        sf::Style::Titlebar | sf::Style::Close
    );
    sf::Texture texture;
    sf::Sprite sprite;
    sf::Shader crtShader;
    sf::RenderTexture renderTexture;
    bool useShader = true;

    NES::Emulator emulator(romPath);

    texture.create(NES::NES_WIDTH, NES::NES_HEIGHT);
    sprite.setTexture(texture);

    if (!crtShader.loadFromFile("Shaders/CRT.frag", sf::Shader::Fragment))
    {
        std::cerr << "Failed to load CRT shader!" << std::endl;
        return;
    }

    if (!renderTexture.create(NES::NES_WIDTH, NES::NES_HEIGHT))
    {
        std::cerr << "Failed to create render texture!" << std::endl;
        return;
    }

    window.setFramerateLimit(60);

    sf::Sprite finalSprite;
    finalSprite.setTexture(renderTexture.getTexture());
    finalSprite.setScale(3.0f, 3.0f);

    bool focus = true;

    sf::Event event;
    while (window.isOpen())
    {
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
            else if (event.type == sf::Event::GainedFocus)
            {
                focus = true;
            }
            else if (event.type == sf::Event::LostFocus)
            {
                focus = false;
            }
            else if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::Escape)
                {
                    window.close();
                }
                else if (event.key.code == sf::Keyboard::F2)
                {
                    emulator.TogglePause();
                }
                else if (event.key.code == sf::Keyboard::F1)
                {
                    useShader = !useShader;
                }
            }
            else if (event.type == sf::Event::KeyReleased)
            {
                if (event.key.code == sf::Keyboard::F3)
                {
                    emulator.SkipOneCycle();
                }
            }
        }

        window.clear(sf::Color::Black);

        if (focus && !emulator.IsPaused())
        {
            // Update emulator state
            emulator.Update();

            // Update PPU screen with the latest frame
            texture.update(emulator.GetScreenData());

            // Render to texture first (your NES emulator output would go here)
            renderTexture.clear(sf::Color::Black);
            renderTexture.draw(sprite);
            renderTexture.display();

            // Set up shader uniforms
            crtShader.setUniform("texture", renderTexture.getTexture());
            crtShader.setUniform("resolution", sf::Vector2f(
                static_cast<float>(window.getSize().x),
                static_cast<float>(window.getSize().y)
            ));

            // Render final output with CRT shader
            window.draw(finalSprite, useShader ? &crtShader : nullptr);
        }

        window.display();
    }
}

///////////////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <path_to_rom>" << std::endl;
        return (1);
    }

    try
    {
        sfml(argv[1]);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return (1);
    }

    return (0);
}
