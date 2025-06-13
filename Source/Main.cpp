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

void sfml(NES::Cartridge& cartridge)
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

    std::shared_ptr<NES::Mapper> mapper = std::make_shared<NES::Mappers::NROM>(cartridge);
    NES::PictureBus pbus;
    NES::PPU ppu(pbus);
    NES::APU apu;
    NES::Controller ctrl1, ctrl2;
    std::unique_ptr<NES::CPU> cpu;
    NES::MainBus mbus(ppu, apu, ctrl1, ctrl2, [&](NES::Byte value){
        cpu->SkipOAMDMACycles();
        auto pagePtr = mbus.GetPagePtr(value);
        if (pagePtr != nullptr)
        {
            ppu.DoDMA(pagePtr);
        }
    });
    cpu = std::make_unique<NES::CPU>(mbus);

    if (!pbus.SetMapper(mapper) || !mbus.SetMapper(mapper))
    {
        std::cerr << "Failed to set mapper!" << std::endl;
        return;
    }

    ppu.SetVBlankCallback([&]() {
        cpu->NMIInterrupt();
    });

    cpu->Reset();
    ppu.Reset();

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

    std::chrono::high_resolution_clock::time_point lastWakeUp = std::chrono::high_resolution_clock::now();
    std::chrono::high_resolution_clock::duration elapsedTime = lastWakeUp - lastWakeUp;

    bool focus = true;
    bool pause = false;

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
                lastWakeUp = std::chrono::high_resolution_clock::now();
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
                    pause = !pause;
                    if (!pause)
                    {
                        lastWakeUp = std::chrono::high_resolution_clock::now();
                    }
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
                    for (int i = 0; i < 29781; i++)
                    {
                        ppu.Step();
                        ppu.Step();
                        ppu.Step();

                        cpu->Step();
                    }
                }
            }
        }

        window.clear(sf::Color::Black);

        if (focus && !pause)
        {
            const auto now = std::chrono::high_resolution_clock::now();
            elapsedTime += now - lastWakeUp;
            lastWakeUp = now;

            while (elapsedTime > std::chrono::nanoseconds(559))
            {
                ppu.Step();
                ppu.Step();
                ppu.Step();

                cpu->Step();

                elapsedTime -= std::chrono::nanoseconds(559);
            }

            // Update PPU screen with the latest frame
            texture.update(ppu.GetScreen().data());

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
        NES::Cartridge cartridge(argv[1]);
        std::cout << "Cartridge loaded successfully!" << std::endl;
        sfml(cartridge);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return (1);
    }

    return (0);
}
