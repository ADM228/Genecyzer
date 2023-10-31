#pragma region header

#include <SFML/Graphics.hpp>
#include <vector>

#ifndef __CHRFONT_INCLUDED__
#define __CHRFONT_INCLUDED__

#define TILE_SIZE 8
#define COLORS 4

class ChrFont {
    public:
        ChrFont() {};
        ChrFont(uint8_t* chrData, uint32_t size, std::vector<uint32_t> codepageTable, bool inverted = 0);
        void init(uint8_t* chrData, uint32_t size, std::vector<uint32_t> codepageTable, bool inverted = 0);

        uint8_t* chrDataPtr;
        uint32_t chrDataSize;
        sf::Texture texture;
        std::vector<uint32_t> codepages;
};

#pragma endregion

#include "Utils.cpp"

ChrFont::ChrFont(uint8_t* chrData, uint32_t size, std::vector<uint32_t> codepageTable, bool inverted){
    init(chrData, size, codepageTable, inverted);
}

void ChrFont::init(uint8_t* chrData, uint32_t size, std::vector<uint32_t> codepageTable, bool inverted){
    this->chrDataPtr = chrData;
    this->chrDataSize = size;
    this->codepages = codepageTable;

    uint8_t colorBuffer[TILE_SIZE*TILE_SIZE];
    uint32_t amount = size>>4;
    auto pixels = new uint8_t [TILE_SIZE*TILE_SIZE*COLORS*amount*(1+inverted)];
    const uint8_t tableRG[] = {0, 255, 160, 0};
    const uint8_t invTableRG[] = {0, 0, 160, 255};
    const uint8_t tableB[] = {0, 255, 176, 0};
    const uint8_t invTableB[] = {0, 0, 176, 255};

    if (inverted) {
        int baseIndex;
        for (uint32_t tile = 0; tile < amount; tile++) {
            for(int i = 0; i < TILE_SIZE; i++){
                colorBuffer[i*TILE_SIZE] = (chrData[(tile<<4)|(i<<1)]>>7)&1 | (chrData[(tile<<4)|(i<<1)|1]>>6)&2;
                colorBuffer[i*TILE_SIZE+1] = (chrData[(tile<<4)|(i<<1)]>>6)&1 | (chrData[(tile<<4)|(i<<1)|1]>>5)&2;
                colorBuffer[i*TILE_SIZE+2] = (chrData[(tile<<4)|(i<<1)]>>5)&1 | (chrData[(tile<<4)|(i<<1)|1]>>4)&2;
                colorBuffer[i*TILE_SIZE+3] = (chrData[(tile<<4)|(i<<1)]>>4)&1 | (chrData[(tile<<4)|(i<<1)|1]>>3)&2;
                colorBuffer[i*TILE_SIZE+4] = (chrData[(tile<<4)|(i<<1)]>>3)&1 | (chrData[(tile<<4)|(i<<1)|1]>>2)&2;
                colorBuffer[i*TILE_SIZE+5] = (chrData[(tile<<4)|(i<<1)]>>2)&1 | (chrData[(tile<<4)|(i<<1)|1]>>1)&2;
                colorBuffer[i*TILE_SIZE+6] = (chrData[(tile<<4)|(i<<1)]>>1)&1 | chrData[(tile<<4)|(i<<1)|1]&2;
                colorBuffer[i*TILE_SIZE+7] = chrData[(tile<<4)|(i<<1)]&1 | (chrData[(tile<<4)|(i<<1)|1]<<1)&2;
            }
            for (int i = 0; i < sizeof(colorBuffer); i+=TILE_SIZE){
                for (int j = 0; j < TILE_SIZE; j++){
                    baseIndex = tile*512+(i*2+j)*COLORS;
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
            for(int i = 0; i < TILE_SIZE; i++){
                colorBuffer[i*TILE_SIZE] = (chrData[(tile<<4)|(i<<1)]>>7)&1 | (chrData[(tile<<4)|(i<<1)|1]>>6)&2;
                colorBuffer[i*TILE_SIZE+1] = (chrData[(tile<<4)|(i<<1)]>>6)&1 | (chrData[(tile<<4)|(i<<1)|1]>>5)&2;
                colorBuffer[i*TILE_SIZE+2] = (chrData[(tile<<4)|(i<<1)]>>5)&1 | (chrData[(tile<<4)|(i<<1)|1]>>4)&2;
                colorBuffer[i*TILE_SIZE+3] = (chrData[(tile<<4)|(i<<1)]>>4)&1 | (chrData[(tile<<4)|(i<<1)|1]>>3)&2;
                colorBuffer[i*TILE_SIZE+4] = (chrData[(tile<<4)|(i<<1)]>>3)&1 | (chrData[(tile<<4)|(i<<1)|1]>>2)&2;
                colorBuffer[i*TILE_SIZE+5] = (chrData[(tile<<4)|(i<<1)]>>2)&1 | (chrData[(tile<<4)|(i<<1)|1]>>1)&2;
                colorBuffer[i*TILE_SIZE+6] = (chrData[(tile<<4)|(i<<1)]>>1)&1 | chrData[(tile<<4)|(i<<1)|1]&2;
                colorBuffer[i*TILE_SIZE+7] = chrData[(tile<<4)|(i<<1)]&1 | (chrData[(tile<<4)|(i<<1)|1]<<1)&2;
            }
            for (int i = 0; i < sizeof(colorBuffer); i++){
                pixels[tile*256+i*4] = tableRG[colorBuffer[i]];
                pixels[tile*256+i*4+1] = tableRG[colorBuffer[i]];
                pixels[tile*256+i*4+2] = tableB[colorBuffer[i]];
                pixels[tile*256+i*4+3] = colorBuffer[i] == 0 ? 0 : 255;
            }
        }
    }
    texture.create(inverted?2*TILE_SIZE:TILE_SIZE,TILE_SIZE*amount);
    texture.update(pixels, inverted?2*TILE_SIZE:TILE_SIZE, TILE_SIZE*amount, 0, 0);
    texture.setSmooth(false);

    delete[] pixels;
}



#endif  // __CHRFONT_INCLUDED__