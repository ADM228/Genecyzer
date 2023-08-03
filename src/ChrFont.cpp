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

TileMatrix ChrFont::renderToTiles(std::string string, int32_t maxChars){
    std::u32string string32 = To_UTF32(string);
    std::vector<uint32_t> charsPerLine;
    std::vector<char32_t> text;
    { // Refactor the text from Unicode to a simply-displayed mess
        uint32_t charOnLine = 0;
        uint32_t lastSpace = 0;
        text.push_back(0x0A);   //HACK 
        for (uint32_t i = 0; i < string32.length(); i++){
            if ((string32[i] >= 0x0A && string32[i] <= 0x0D) || string32[i] == 0x85 || 
            string32[i] == 0x2028 || string32[i] == 0x2029){
                // Line terminators
                lastSpace = text.size();
                text.push_back(0x0A);
                charsPerLine.push_back(charOnLine);
                charOnLine = 0;
                if (string32[i] == 0x0D && string32[i] == 0x0A) // CRLF
                    i++;
            } else if (string32[i] == 0x09 || string32[i] == 0x20 || string32[i] == 0x1680 || 
            (string32[i] >= 0x2000 && string32[i] <= 0x200D && string32[i] != 0x2007) ||
            string32[i] == 0x205F || string32[i] == 0x3000 || string32[i] == 0x180E){
                // Spaces that can make a newline
                lastSpace = text.size();
                if (!(string32[i] >= 0x200B && string32[i] <= 0x200D)){ // If not zero width spaces
                    charOnLine++;
                    if (charOnLine == maxChars){
                        text.push_back(0x0A);
                        charsPerLine.push_back(charOnLine-1);
                        charOnLine = 0;
                    } else 
                        text.push_back(0x20);
                } else text.push_back(0x200B); // Else push ZWSP in case a newline is needed
            } else if (string32[i] == 0xA0 || string32[i] == 0x2007 || string32[i] == 0x202F){
                // Non breaking spaces
                text.push_back(0x20);
                charOnLine++;
            } else {
                // Normal alphabets with normal breaking rules
                charOnLine++;
                if (charOnLine > maxChars && text[lastSpace] == 0x0A){ // If word longer than maxChars
                    lastSpace = text.size();
                    charsPerLine.push_back(maxChars);
                    text.push_back(0x0A);   // Nah legit fuck this, just chop the word
                    text.push_back(string32[i]);
                    charOnLine = 1;
                } else if (charOnLine > maxChars){ // If the last space is a space
                    text.push_back(string32[i]);
                    text[lastSpace] = 0x0A;
                    charsPerLine.push_back(charOnLine - (text.size() - lastSpace));
                    charOnLine = text.size() - lastSpace - 1;
                    
                } else text.push_back(string32[i]);
            }
        }
        charsPerLine.push_back(charOnLine);
    }
    uint32_t maxWidth = (maxChars == -1 ? 0 : maxChars);
    TileMatrix matrix(std::max(*max_element(charsPerLine.begin(), charsPerLine.end()), maxWidth), charsPerLine.size(), 0x20);
    uint32_t x = 0, y = 0;
    for (uint32_t i = 1; i < text.size(); i++){
        if (text[i] == 0x0A){    // Newline
            y++;
            x = 0;
        } else if (text[i] == 0x200B){}    // ZWSP
        else {
            uint32_t bank = std::find(codepages.begin(), codepages.end(), text[i]&0xFFFFFF80)-codepages.begin();
            if (bank < codepages.size()){
                matrix.setTile(x++, y, (bank<<7)|(text[i]&0x7F));
            } else {
                matrix.setTile(x++, y, 0x20);
            }
        }
    }
    return matrix;
}
