#ifdef _MSC_VER 
#pragma warning(disable: 4242) // try to disable warning 4242 present in stb_image.h, a dependency of SFML/Graphics.hpp
#endif
#include <SFML/Graphics.hpp>
#ifdef _MSC_VER 
#pragma warning(default: 4242) 
#endif
#include "chrLoad.cpp"
#include "Tile.hpp"
#include "ChrFont.hpp"

#define INCBIN_PREFIX bin_
#define INCBIN_STYLE INCBIN_STYLE_SNAKE
#include "incbin.h"
INCBIN(font, "../SNESFM/graphics/tilesetUnicode.chr");



int main()
{
    sf::RenderWindow window(sf::VideoMode(200, 200), "Genecyzer");
    sf::View InstrumentView(sf::FloatRect(0.f, 0.f, 200.f, 200.f));
    sf::View TrackerView(sf::FloatRect(0.f,0.f,200.f,200.f));

    unsigned char * fontPointer = const_cast<unsigned char *>(bin_font_data);
    std::vector<uint32_t> codepages = {0x0000, 0x0080, 0x0380, 0x0400, 0x0480, 0x3000, 0x3080};
    ChrFont font(fontPointer, bin_font_size, codepages);

    sf::Sprite sprite0;
    sf::Sprite sprite7;
    int instPage = 0;
    uint8_t scale = 5;
    uint8_t mode = 0;

    

    window.setView(TrackerView);

    sprite0.setTexture(font.texture);
    sprite0.setScale(sf::Vector2f((float)scale, (float)scale));
    sprite0.setTextureRect(sf::IntRect(0, 8*0x30, 8, 8));
    
    //((float)scale, (float)scale));
    //sprite7.setTextureRect(sf::IntRect(0, 8*0x37, 8, 8));


    uint32_t tilesd[] = {0,1,2,2};
    TileMatrix tiles(4,4, 0x30);
    tiles.copyRect(0, 2, 4, 1, tilesd);
    sf::Texture text;
    std::string longassstring = "English Ελληνικά Русский にほんこ゛ antidisestablishmentarianism\nThe quick fox jumped over the lazy dog\nСъешь же ещё этих мягких французских булочек, да выпей чаю";
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            else if (event.type == sf::Event::Resized){
                //TrackerView.reset(sf::FloatRect(0, 0, 32, 32));
                //TrackerView.setViewport(sf::FloatRect(0.f, 0.f, 32.f/event.size.width, 32.f/event.size.height));
                InstrumentView.reset(sf::FloatRect(instPage*scale*8*16, 0, event.size.width, 64));
                InstrumentView.setViewport(sf::FloatRect(0, 0, scale, 64.f/event.size.height*scale));
                TileMatrix textMatrix = font.renderToTiles(longassstring, static_cast<int>((event.size.width/scale)/8));
                text.create(textMatrix.getWidth()*8, textMatrix.getHeight()*8);
                text.update(textMatrix.renderToTexture(font.texture));
                sprite7.setTextureRect(sf::IntRect(0, 0, text.getSize().x, text.getSize().y));
                sprite7.setTexture(text);
            } else if (event.type == sf::Event::KeyPressed){
                if (event.key.code == sf::Keyboard::Left){
                    instPage++;
                    InstrumentView.reset(sf::FloatRect(instPage*scale*8*16, 0, 64, 64));
                }
            }
        }

        window.clear(sf::Color(255,255,0,0));
        window.setView(InstrumentView);

        sprite7.setScale(sf::Vector2f(1.f, 1.f));
        window.draw(sprite7);
        // tiles.render(0,0,&window, font[0]);
        // window.draw(tile.renderVertex, sf::RenderStates(&font[0]));
        // window.draw(tile1.renderVertex, sf::RenderStates(&font[0]));
        // window.draw(tile2.renderVertex, sf::RenderStates(&font[0]));
        // window.draw(tile3.renderVertex, sf::RenderStates(&font[0]));
        // window.setView(TrackerView);
        
        window.display();
    }

    return 0;
}
