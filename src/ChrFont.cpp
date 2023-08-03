#include "ChrFont.hpp"

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


// utility wrapper to adapt locale-bound facets for wstring/wbuffer convert
template<class Facet>
struct deletable_facet : Facet
{
    template<class... Args>
    deletable_facet(Args&&... args) : Facet(std::forward<Args>(args)...) {}
    ~deletable_facet() {}
};

std::u32string To_UTF32(const std::string &s)
{
    std::wstring_convert<deletable_facet<std::codecvt<char32_t, char, std::mbstate_t>>, char32_t> conv;
    return conv.from_bytes(s);
}

TileMatrix ChrFont::renderToTiles(std::string string){
    std::u32string text = To_UTF32(string);
    TileMatrix matrix(text.length(), 1);

    for (uint32_t i = 0; i < text.length(); i++){
        uint32_t bank = std::find(codepages.begin(), codepages.end(), text[i]&0xFFFFFF80)-codepages.begin();
        if (bank < codepages.size()){
            matrix.setTile(i, 0, (bank<<7)|(text[i]&0x7F));
        } else {
            matrix.setTile(i, 0, 0x20);
        }
    }
    return matrix;
}
