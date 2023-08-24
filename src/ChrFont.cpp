#include "ChrFont.hpp"
#include "StrConvert.hpp"

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

TileMatrix ChrFont::renderToTiles(std::u32string string, int32_t maxChars){
    std::vector<uint32_t> charsPerLine;
    std::vector<char32_t> text;
    { // Refactor the text from Unicode to a simply-displayed mess
        uint32_t charOnLine = 0;
        uint32_t lastSpace = 0;
        text.push_back(0x0A);   //HACK 
        for (uint32_t i = 0; i < string.length(); i++){
            uint32_t character = string[i];
            if ((character >= 0x0A && character <= 0x0D) || character == 0x85 || 
            character == 0x2028 || character == 0x2029){
                // Line terminators
                lastSpace = text.size();
                text.push_back(0x0A);
                charsPerLine.push_back(charOnLine);
                charOnLine = 0;
                if (character == 0x0D && character == 0x0A) // CRLF
                    i++;
            } else if (character == 0x09 || character == 0x20 || character == 0x1680 || 
            (character >= 0x2000 && character <= 0x200D && character != 0x2007) ||
             character == 0x205F || character == 0x3000 || character == 0x180E){
                // Spaces that can make a newline
                lastSpace = text.size();
                if (!(character >= 0x200B && character <= 0x200D)){ // If not zero width spaces
                    charOnLine++;
                    if (charOnLine == maxChars){
                        text.push_back(0x0A);
                        charsPerLine.push_back(charOnLine-1);
                        charOnLine = 0;
                    } else 
                        text.push_back(0x20);
                } else text.push_back(0x200B); // Else push ZWSP in case a newline is needed
            } else if (character == 0xA0 || character == 0x2007 || character == 0x202F){
                // Non breaking spaces
                text.push_back(0x20);
                charOnLine++;
			} else if (character == 0x2060 || character == 0xFEFF){
				// Zero width non breaking spaces
				text.push_back(0x2060);
            } else if (character >= 0x3040 && character <= 0x309F){
                // Hiragana
                if (character >= 0x304B && character <= 0x3062 && (character & 1) == 0 ||       // Most dakuten kana
                character == 0x3065 || character == 0x3067 || character == 0x3069 ||            // づ, で, ど (offset by 1)
                character == 0x3094 || character == 0x309E ||                                   // ゔ, ゞ
                character >= 0x3070 && character <= 0x307D && ((character & 0x0F) % 3) != 2){   // ば, ぱ, び, ぴ, ぶ, ぷ ,べ, ぺ, ぼ, ぽ
                    charOnLine += 2;
					if (text.back() != 0x2060){
		                lastSpace = text.size();
		                if (charOnLine > maxChars){
		                    charsPerLine.push_back(charOnLine-1 > maxChars ? maxChars-1 : maxChars);
		                    text.push_back(0x0A);
		                    charOnLine = 2;
		                } else
		                    text.push_back(0x200B);
					}	// TODO: an else for when kanji is converted

                    if (character == 0x3094){   
                        // Special because ゔ doesn't come right after its non voiced counterpart
                        text.push_back(0x3046); // う
                        text.push_back(0x309B); // ゛
                    } else if (character >= 0x3070 && character <= 0x307D && ((character & 0x0F) % 3) == 1) {
                        // ぱ, ぴ, ぷ, ぺ, ぽ 
                        text.push_back(character-2); // は, ひ, ふ, へ, ほ
                        text.push_back(0x309C);      // ゜
                    } else {
                        text.push_back(character-1); // Non-voiced kana
                        text.push_back(0x309B);      // ゛
                    }
                } else if (((character-1) & 0xFFFFFFFC) == 0x3098) {    // ゛, ゜ and their combining versions
                    charOnLine++;
                    if (charOnLine > maxChars && text[lastSpace] == 0x0A){ // If word longer than maxChars
                        lastSpace = text.size();
                        charsPerLine.push_back(maxChars);
                        text.push_back(0x0A);   // Nah legit fuck this, just chop the word
                        text.push_back(((character-1)|0x0002)+1);
                        charOnLine = 1;
                    } else if (charOnLine > maxChars){ // If the last space is a space
                        text.push_back(((character-1)|0x0002)+1);
                        text[lastSpace] = 0x0A;
                        charsPerLine.push_back(charOnLine - (text.size() - lastSpace));
                        charOnLine = text.size() - lastSpace - 1;
                        
                    } else text.push_back(((character-1)|0x0002)+1);
                } else {
                    charOnLine++;
                    lastSpace = text.size();
                    if (charOnLine > maxChars){ // If the last space is a space
                        lastSpace = text.size();
                        charsPerLine.push_back(maxChars);
                        text.push_back(0x0A);   // Nah legit fuck this, just chop the word
                        text.push_back(character);
                        charOnLine = 1;
                    } else {
                        text.push_back(0x200B);
                        text.push_back(character);
                    }
                }
            } else {
                // Normal alphabets with normal breaking rules
                charOnLine++;
                if (charOnLine > maxChars && text[lastSpace] == 0x0A){ // If word longer than maxChars
                    lastSpace = text.size();
                    charsPerLine.push_back(maxChars);
                    text.push_back(0x0A);   // Nah legit fuck this, just chop the word
                    text.push_back(character);
                    charOnLine = 1;
                } else if (charOnLine > maxChars){ // If the last space is a space
                    text.push_back(character);
                    text[lastSpace] = 0x0A;
                    charsPerLine.push_back(charOnLine - (text.size() - lastSpace));
                    charOnLine = text.size() - lastSpace - 1;
                    
                } else text.push_back(character);
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
        } else if (text[i] == 0x200B || text[i] == 0x2060){}    // ZWSP, ZWNBSP
        else {
            uint32_t bank = std::find(codepages.begin(), codepages.end(), text[i]&0xFFFFFF80)-codepages.begin();
            if (bank < codepages.size()){
                matrix.setTile(x++, y, (bank<<7)|(text[i]&0x7F));
            } else {
                matrix.setTile(x++, y, 0x7F);
            }
        }
    }
    return matrix;
}

TileMatrix ChrFont::renderToTiles(std::string string, int32_t maxChars){
    return renderToTiles(To_UTF32(string), maxChars);
}

sf::Texture ChrFont::renderToTexture(std::u32string string, int32_t maxChars){
    return renderToTiles(string, maxChars).renderToTexture(texture);
}

sf::Texture ChrFont::renderToTexture(std::string string, int32_t maxChars){
    return renderToTiles(To_UTF32(string), maxChars).renderToTexture(texture);
}