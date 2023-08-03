#include <SFML/Graphics.hpp>
#include "incbin.h"
#include "Tile.hpp"
#include <vector>

#ifndef __FONT_INCLUDED__
#define __FONT_INCLUDED__

class ChrFont {
    public:
        ChrFont(uint8_t* chrData, uint32_t size, std::vector<uint32_t> codepageTable);
        TileRow renderToTileRow(std::string string);
        TileMatrix renderToTiles(std::string string);
        sf::Texture renderToTexture(std::string string);
        sf::Texture texture;
        std::vector<uint32_t> codepages;
};

#endif