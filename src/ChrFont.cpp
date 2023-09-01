#pragma region header

#include <SFML/Graphics.hpp>
#include "Tile.cpp"
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

#pragma endregion

#include "StrConvert.cpp"

ChrFont::ChrFont(uint8_t* chrData, uint32_t size, std::vector<uint32_t> codepageTable, bool inverted){
    this->chrDataPtr = chrData;
    this->chrDataSize = size;
    this->codepages = codepageTable;

    uint8_t colorBuffer[8*8];
    uint32_t amount = size>>4;
    std::vector<sf::Uint8> pixels(8*8*4*amount);
    const uint8_t tableRG[] = {0, 255, 160, 0};
    const uint8_t invTableRG[] = {0, 0, 256-160, 255};
    const uint8_t tableB[] = {0, 255, 176, 0};
    const uint8_t invTableB[] = {0, 0, 256-176, 255};

    if (inverted) {
        pixels.resize(8*8*4*2*amount);
        int baseIndex;
        for (uint32_t tile = 0; tile < amount; tile++) {
            for(int i = 0; i < 8; i++){
                colorBuffer[i*8] = (chrData[(tile<<4)|(i<<1)]>>7)&1 | (chrData[(tile<<4)|(i<<1)|1]>>6)&2;
                colorBuffer[i*8+1] = (chrData[(tile<<4)|(i<<1)]>>6)&1 | (chrData[(tile<<4)|(i<<1)|1]>>5)&2;
                colorBuffer[i*8+2] = (chrData[(tile<<4)|(i<<1)]>>5)&1 | (chrData[(tile<<4)|(i<<1)|1]>>4)&2;
                colorBuffer[i*8+3] = (chrData[(tile<<4)|(i<<1)]>>4)&1 | (chrData[(tile<<4)|(i<<1)|1]>>3)&2;
                colorBuffer[i*8+4] = (chrData[(tile<<4)|(i<<1)]>>3)&1 | (chrData[(tile<<4)|(i<<1)|1]>>2)&2;
                colorBuffer[i*8+5] = (chrData[(tile<<4)|(i<<1)]>>2)&1 | (chrData[(tile<<4)|(i<<1)|1]>>1)&2;
                colorBuffer[i*8+6] = (chrData[(tile<<4)|(i<<1)]>>1)&1 | chrData[(tile<<4)|(i<<1)|1]&2;
                colorBuffer[i*8+7] = chrData[(tile<<4)|(i<<1)]&1 | (chrData[(tile<<4)|(i<<1)|1]<<1)&2;
            }
            for (int i = 0; i < sizeof(colorBuffer); i+=8){
                for (int j = 0; j < 8; j++){
                    baseIndex = tile*512+(i*2+j)*4;
                    pixels[baseIndex] = tableRG[colorBuffer[i+j]];
                    pixels[baseIndex+1] = tableRG[colorBuffer[i+j]];
                    pixels[baseIndex+2] = tableB[colorBuffer[i+j]];
                    pixels[baseIndex+3] = colorBuffer[i+j] == 0 ? 0 : 255;

                    pixels[baseIndex+32] = invTableRG[colorBuffer[i+j]];
                    pixels[baseIndex+32+1] = invTableRG[colorBuffer[i+j]];
                    pixels[baseIndex+32+2] = invTableB[colorBuffer[i+j]];
                    pixels[baseIndex+32+3] = colorBuffer[i+j] == 0 ? 0 : 255;
                }
            }
        }
    } else {
        for (uint32_t tile = 0; tile < amount; tile++) {
            for(int i = 0; i < 8; i++){
                colorBuffer[i*8] = (chrData[(tile<<4)|(i<<1)]>>7)&1 | (chrData[(tile<<4)|(i<<1)|1]>>6)&2;
                colorBuffer[i*8+1] = (chrData[(tile<<4)|(i<<1)]>>6)&1 | (chrData[(tile<<4)|(i<<1)|1]>>5)&2;
                colorBuffer[i*8+2] = (chrData[(tile<<4)|(i<<1)]>>5)&1 | (chrData[(tile<<4)|(i<<1)|1]>>4)&2;
                colorBuffer[i*8+3] = (chrData[(tile<<4)|(i<<1)]>>4)&1 | (chrData[(tile<<4)|(i<<1)|1]>>3)&2;
                colorBuffer[i*8+4] = (chrData[(tile<<4)|(i<<1)]>>3)&1 | (chrData[(tile<<4)|(i<<1)|1]>>2)&2;
                colorBuffer[i*8+5] = (chrData[(tile<<4)|(i<<1)]>>2)&1 | (chrData[(tile<<4)|(i<<1)|1]>>1)&2;
                colorBuffer[i*8+6] = (chrData[(tile<<4)|(i<<1)]>>1)&1 | chrData[(tile<<4)|(i<<1)|1]&2;
                colorBuffer[i*8+7] = chrData[(tile<<4)|(i<<1)]&1 | (chrData[(tile<<4)|(i<<1)|1]<<1)&2;
            }
            for (int i = 0; i < sizeof(colorBuffer); i++){
                pixels[tile*256+i*4] = tableRG[colorBuffer[i]];
                pixels[tile*256+i*4+1] = tableRG[colorBuffer[i]];
                pixels[tile*256+i*4+2] = tableB[colorBuffer[i]];
                pixels[tile*256+i*4+3] = colorBuffer[i] == 0 ? 0 : 255;
            }
        }
    }
    texture.create(inverted?16:8,8*amount);
    texture.update(pixels.data(), inverted?16:8, 8*amount, 0, 0);
    texture.setSmooth(false);
}



#endif  // __CHRFONT_INCLUDED__