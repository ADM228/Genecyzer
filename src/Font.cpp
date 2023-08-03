#include "Font.hpp"

ChrFont::ChrFont(uint8_t* chrData, uint32_t size, std::vector<uint32_t> codepageTable){
    char buffer[16];
    uint8_t colorBuffer[8*8];
    uint32_t amount = size>>4;
    std::vector<sf::Uint8> pixels(8*8*4*amount);
    printf("Loading %d characters, which makes pixels %zd bytes long\n", amount, pixels.size());
    const uint8_t tableRG[] = {0, 255, 160, 0};
    const uint8_t tableB[] = {0, 255, 176, 0};

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
    texture.create(8,8*amount);
    texture.update(pixels.data(), 8, 8*amount, 0, 0);
    texture.setSmooth(false);
    codepages = codepageTable;
}
