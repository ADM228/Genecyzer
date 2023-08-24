#include <SFML/Graphics.hpp>
#include "Tile.hpp"
#include <vector>

#ifndef __CHRFONT_INCLUDED__
#define __CHRFONT_INCLUDED__

class ChrFont {
    public:
        ChrFont(uint8_t* chrData, uint32_t size, std::vector<uint32_t> codepageTable, bool inverted = 0);

        TileRow renderToTileRow(std::string string);

        TileMatrix renderToTiles(std::u32string string, int32_t maxChars = -1, bool inverted = 0);
        TileMatrix renderToTiles(std::string string, int32_t maxChars = -1, bool inverted = 0);

        sf::Texture renderToTexture(std::u32string string, int32_t maxChars = -1, bool inverted = 0);
        sf::Texture renderToTexture(std::string string, int32_t maxChars = -1, bool inverted = 0);

        uint8_t* chrDataPtr;
        uint32_t chrDataSize;
        sf::Texture texture;
        std::vector<uint32_t> codepages;
};

#endif  // __CHRFONT_INCLUDED__