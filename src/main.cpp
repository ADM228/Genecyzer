#include <cmath>
#include <cstdio>

#include <SFML/Graphics.hpp>

#include "binIncludes.cpp"
#include "Instance.cpp"


int main()
{
    
    Instance instance;

    // Font stuff
    std::vector<uint32_t> codepages = {0x0000, 0x0080, 0x0380, 0x0400, 0x0480, 0x3000, 0x3080};
    instance.addMonospaceFont(bin_font_data, bin_font_size, codepages);


    // sf::Sprite sprite0;
    // window.setView(TrackerView);
    // sprite0.setTexture(font.texture);
    // sprite0.setScale(sf::Vector2f((float)scale, (float)scale));
    // sprite0.setTextureRect(sf::IntRect(0, 8*0x30, 8, 8));

    while (instance.isWindowOpen())
    {
        instance.ProcessEvents();
        instance.Update();
    }

    return 0;
}
