#include <SFML/Graphics.hpp>
#include "Tile.hpp"
#include <vector>

#ifndef __CHRFONT_INCLUDED__
#define __CHRFONT_INCLUDED__

class ChrFont {
    public:
        ChrFont(uint8_t* chrData, uint32_t size, std::vector<uint32_t> codepageTable);
        TileRow renderToTileRow(std::string string);
        TileMatrix renderToTiles(std::string string, int32_t maxChars = -1);
        sf::Texture renderToTexture(std::string string, int32_t maxChars = -1) 
        {return renderToTiles(string, maxChars).renderToTexture(texture);};
        sf::Texture texture;
        std::vector<uint32_t> codepages;
};

#endif