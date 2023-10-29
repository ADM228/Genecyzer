#ifndef __TEXTRENDERER_INCLUDED__
#define __TEXTRENDERER_INCLUDED__

#include <algorithm>
#include <string>
#include <vector>
#include "Tile.cpp"
#include "ChrFont.cpp"
#include "Utils.cpp"

uint32_t excNumberTR = 0;
#define debugNum(x) { printf("[TextRenderer #%08X]\n", x); fflush(stdout); }

struct wrappedText {
    std::vector<char32_t> text;
    uint16_t width;
    uint16_t height;
};

class TextRenderer {
    public:
        static std::u32string preprocess(std::u32string string);
        static wrappedText wrapText(std::u32string text, int maxChars = -1, bool preprocess = 1);
        #if defined (__TILE_INCLUDED__) && defined(__CHRFONT_INCLUDED__) 
            static TileMatrix render (wrappedText *text, ChrFont *font, bool inverted = 0);
            static TileMatrix render (std::u32string text, ChrFont *font, int maxChars = -1, bool preprocess = 1, bool inverted = 0);
            #ifdef __STRCONVERT_INCLUDED__
                static TileMatrix render (std::string text, ChrFont *font, int maxChars = -1, bool preprocess = 1, bool inverted = 0);
            #endif
        #endif
    private:
        TextRenderer() {};
        constexpr static uint16_t halfKatakanaTable[] {
                                                                    0x2985, // U+FF5F                             ｟ 
            0x2986, 0x3002, 0x300C, 0x300D, 0x3001, 0x30FB, 0x30F2, 0x30A1, // U+FF60	｠  ｡   ｢   ｣   ､   ･   ｦ   ｧ 
            0x30A3, 0x30A5, 0x30A7, 0x30A9, 0x30E3, 0x30E5, 0x30E7, 0x30C3, // U+FF68   ｨ 	ｩ 	ｪ 	ｫ 	ｬ 	ｭ 	ｮ 	ｯ 
            0x30FC, 0x30A2, 0x30A4, 0x30A6, 0x30A8, 0x30AA, 0x30AB, 0x30AD, // U+FF70	ｰ 	ｱ 	ｲ 	ｳ 	ｴ 	ｵ 	ｶ 	ｷ 
            0x30AF, 0x30B1, 0x30B3, 0x30B5, 0x30B7, 0x30B9, 0x30BB, 0x30BD, // U+FF78	ｸ 	ｹ 	ｺ 	ｻ 	ｼ 	ｽ 	ｾ 	ｿ
            0x30BF, 0x30C1, 0x30C4, 0x30C6, 0x30C8, 0x30CA, 0x30CB, 0x30CC, // U+FF80 	ﾀ 	ﾁ 	ﾂ 	ﾃ 	ﾄ 	ﾅ 	ﾆ 	ﾇ 
            0x30CD, 0x30CE, 0x30CF, 0x30D2, 0x30D5, 0x30D8, 0x30DB, 0x30DE, // U+FF88	ﾈ 	ﾉ 	ﾊ 	ﾋ 	ﾌ 	ﾍ 	ﾎ 	ﾏ
            0x30DF, 0x30E0, 0x30E1, 0x30E2, 0x30E4, 0x30E6, 0x30E8, 0x30E9, // U+FF90 	ﾐ 	ﾑ 	ﾒ 	ﾓ 	ﾔ 	ﾕ 	ﾖ 	ﾗ 
            0x30EA, 0x30EB, 0x30EC, 0x30ED, 0x30EF, 0x30F3, 0x309B, 0x309C, // U+FF98	ﾘ 	ﾙ 	ﾚ 	ﾛ 	ﾜ 	ﾝ 	ﾞ 	ﾟ
            0x3164                                                          // U+FFA0 [HWHF]
        };
        constexpr static uint16_t modWidthSymbolTable[] {
            0x00A2, 0x00A3, 0x00AC, 0x00AF, 0x00A6, 0x00A5, 0x20A9, 0x0000, // U+FFE0	￠  ￡  ￢  ￣  ￤  ￥  ￦ 
            0x2502, 0x2190, 0x2191, 0x2192, 0x2193, 0x25A0, 0x25CB, 0x0000  // U+FFE8	￨ 	￩ 	￪ 	￫ 	￬ 	￭ 	￮ 
        };
};

std::u32string TextRenderer::preprocess(std::u32string string){
    std::vector<char32_t> output_text;
    for (uint32_t i = 0; i < string.length(); i++){
        uint32_t character = string[i];
        if ((character >= 0x0A && character <= 0x0D) || character == 0x85 || 
        character == 0x2028 || character == 0x2029){
            // Line terminators
            output_text.push_back(0x0A);   // LF
            if (character == 0x0D && string[i+1] == 0x0A)   // CRLF
                i++;
        } else if (character == 0x09 || character == 0x20 || character == 0x1680 || 
        (character >= 0x2000 && character <= 0x200A && character != 0x2007) ||
            character == 0x205F || character == 0x3000 || character == 0x180E)
            // Normal breaking spaces
            output_text.push_back(0x20);
        else if (character >= 0x200B && character <= 0x200D)
            // Zero width breaking spaces
            output_text.push_back(0x200B);
        else if (character == 0xA0 || character == 0x2007 || character == 0x202F)
            // Non-breaking spaces
            output_text.push_back(0xA0);
        else if (character == 0x2060 || character == 0xFEFF)
            // Zero width non breaking spaces
            output_text.push_back(0x2060);
        
        // Hiragana and Katakana blocks (3040-30FF)
        else if (character >= 0x304B && character <= 0x3062 && (character & 1) == 0 ||              // Most dakuten hiragana
        character >= 0x30AB && character <= 0x30C2 && (character & 1) == 0 ||                       // Most dakuten katakana
        character == 0x3065 || character == 0x3067 || character == 0x3069 || character == 0x309E || // づ, で, ど, ゞ
        character == 0x30C5 || character == 0x30C7 || character == 0x30C9 || character == 0x30FE || // ヅ, デ, ド
        character >= 0x3070 && character <= 0x307D && ((character & 0x0F) % 3) == 0 ||              // ば, び, ぶ, べ, ぼ
        character >= 0x30D0 && character <= 0x30DD && ((character & 0x0F) % 3) == 0) {              // バ, ビ, ブ, ベ, ボ
            output_text.push_back(character-1); // Non-voiced kana
            output_text.push_back(0x309B);      // ゛
        } else if (character >= 0x3070 && character <= 0x307D && ((character & 0x0F) % 3) == 1 ||   // ぱ, ぴ, ぷ, ぺ, ぽ
        character >= 0x30D0 && character <= 0x30DD && ((character & 0x0F) % 3) == 1) {              // パ, ピ, プ, ペ, ポ 
            output_text.push_back(character-2); // は, ひ, ふ, へ, ほ, ハ, ヒ, フ, ヘ, ホ
            output_text.push_back(0x309C);      // ゜
        } else if (character >= 0x30F7 && character <= 0x30FA){
            // ヷ, ヸ, ヹ, ヺ
            output_text.push_back(character-8); // ワ, ヰ, ヱ, ヲ
            output_text.push_back(0x309B);      // ゛
        } else if (character == 0x3094 || character == 0x30F4){   
            // ゔ, ヴ
            output_text.push_back(character-0x4E);  // う, ウ
            output_text.push_back(0x309B);          // ゛
        } else if (character == 0x3099 || character == 0x309A) 
            // Combining versions of ゛ and ゜
            output_text.push_back(character+2);

        // Halfwidth and fullwidth forms block (FF00-FFEF)
        else if (character >= 0xFF01 && character <= 0xFF5E)
            // Fullwidth versions of ASCII
            output_text.push_back(character-0xFEE0);
        else if (character >= 0xFF5F && character <= 0xFFA0)
            // Halfwidth Katakana + some punctuation + [HWHF]
            output_text.push_back(halfKatakanaTable[character-0xFF5F]);
        //TODO: Halfwidth hangul when i add hangul support
        else if (character >= 0xFFE0 && character <= 0xFFEF)
            // Some symbols
            output_text.push_back(modWidthSymbolTable[character-0xFFE0]);
        
        else
            output_text.push_back(character);
    }

    output_text.push_back(0x00);

    std::u32string out_string(output_text.data());
    return out_string;
}

wrappedText TextRenderer::wrapText(std::u32string text, int maxChars, bool preprocess){
    std::u32string inString;
    if (preprocess) inString = TextRenderer::preprocess(text);
    else inString = text;
    std::vector<uint32_t> charsPerLine;
    std::vector<char32_t> outText;
    // Refactor the text from Unicode to a simply-displayed mess
        uint32_t charOnLine = 0;
        uint32_t lastSpace = 0;
        outText.push_back(0x0A);   //HACK 
        for (uint32_t i = 0; i < inString.length(); i++){
            uint32_t character = inString[i];
            if ((character >= 0x0A && character <= 0x0D) || character == 0x85 || 
            character == 0x2028 || character == 0x2029){
                // Line terminators
                lastSpace = outText.size();
                outText.push_back(0x0A);
                charsPerLine.push_back(charOnLine);
                charOnLine = 0;
                if (character == 0x0D && inString[i+1] == 0x0A) // CRLF
                    i++;
            } else if (character == 0x09 || character == 0x20 || character == 0x1680 || 
            (character >= 0x2000 && character <= 0x200D && character != 0x2007) ||
             character == 0x205F || character == 0x3000 || character == 0x180E){
                // Spaces that can make a newline
                lastSpace = outText.size();
                if (!(character >= 0x200B && character <= 0x200D)){ // If not zero width spaces
                    charOnLine++;
                    if (charOnLine == maxChars){
                        outText.push_back(0x0A);
                        charsPerLine.push_back(charOnLine-1);
                        charOnLine = 0;
                    } else 
                        outText.push_back(0x20);
                } else outText.push_back(0x200B); // Else push ZWSP in case a newline is needed
            } else if (character == 0xA0 || character == 0x2007 || character == 0x202F){
                // Non breaking spaces
                outText.push_back(0x20);
                charOnLine++;
			} else if (character == 0x2060 || character == 0xFEFF){
				// Zero width non breaking spaces
				outText.push_back(0x2060);
            } else if (character >= 0x3040 && character <= 0x30FF){
                // Hiragana
                if (((character-1) & 0xFFFFFFFC) == 0x3098) {    // ゛, ゜ and their combining versions
                    charOnLine++;
                    if (charOnLine > maxChars && outText[lastSpace] == 0x0A){ // If word longer than maxChars
                        lastSpace = outText.size();
                        charsPerLine.push_back(maxChars);
                        outText.push_back(0x0A);   // Nah legit fuck this, just chop the word
                        outText.push_back(((character-1)|0x0002)+1);
                        charOnLine = 1;
                    } else if (charOnLine > maxChars){ // If the last space is a space
                        outText.push_back(((character-1)|0x0002)+1);
                        outText[lastSpace] = 0x0A;
                        charsPerLine.push_back(charOnLine - (outText.size() - lastSpace));
                        charOnLine = outText.size() - lastSpace - 1;
                        
                    } else outText.push_back(((character-1)|0x0002)+1);
                } else {
                    charOnLine++;
                    if (outText.back() != 0x20){
                        lastSpace = outText.size();
                        if (charOnLine > maxChars){ // If the last space is a space
                            lastSpace = outText.size();
                            charsPerLine.push_back(maxChars);
                            outText.push_back(0x0A);   // Nah legit fuck this, just chop the word
                            charOnLine = 1;
                        } else {
                            outText.push_back(0x200B);
                        }
                    } else {
                        if (charOnLine > maxChars){ // If the last space is a space
                            lastSpace = outText.size();
                            charsPerLine.push_back(maxChars);
                            outText.pop_back();
                            outText.push_back(0x0A);   // Nah legit fuck this, just chop the word
                            charOnLine = 1;
                        }
                    }
                    outText.push_back(character);

                }
            } else {
                // Normal alphabets with normal breaking rules
                charOnLine++;
                if (charOnLine > maxChars && outText[lastSpace] == 0x0A){ // If word longer than maxChars
                    lastSpace = outText.size();
                    charsPerLine.push_back(maxChars);
                    outText.push_back(0x0A);   // Nah legit fuck this, just chop the word
                    outText.push_back(character);
                    charOnLine = 1;
                } else if (charOnLine > maxChars){ // If the last space is a space
                    outText.push_back(character);
                    outText[lastSpace] = 0x0A;
                    charsPerLine.push_back(charOnLine - (outText.size() - lastSpace));
                    charOnLine = outText.size() - lastSpace - 1;
                    
                } else outText.push_back(character);
            }
        }
        charsPerLine.push_back(charOnLine);

    wrappedText output;
    output.text = outText;
    output.width = std::max(*max_element(charsPerLine.begin(), charsPerLine.end()), (uint32_t)(maxChars == -1 ? 0 : maxChars));
    output.height = charsPerLine.size();

    return output;
}

#if defined (__TILE_INCLUDED__) && defined(__CHRFONT_INCLUDED__) 

TileMatrix TextRenderer::render(wrappedText *text, ChrFont *font, bool inverted){

    auto string = text->text;
    auto codepages = font->codepages;

    TileMatrix matrix(text->width, text->height, 0x20);

    matrix.fillInvert(inverted);
    uint32_t x = 0, y = 0;

    for (uint32_t i = 1; i < string.size(); i++){
        // if (x > matrix.getWidth()) {printf ("X overflow at row %03d, col %03d; CMP: %03d, %03d\n=== STRDATA ===\n", y, x, matrix.getHeight(), matrix.getWidth());
        // for (int j = 1; j < string.size(); j++){
        //     printf("%04X ", string[j]);
        // }
        // printf ("\n=== STRDATA ===\nSTRLEN: %03d\n\n", string.size()-1);  fflush(stdout);}
        //if (y > matrix.getHeight()) {printf ("Y overflow at row %03d        ; CMP: %03d, %03d\n", y, matrix.getHeight(), matrix.getWidth()); fflush(stdout);}
        if (string[i] == 0x0A){    // Newline
            y++;
            x = 0;
        } else if (string[i] == 0x200B || string[i] == 0x2060){}    // ZWSP, ZWNBSP
        else {
            uint32_t bank = std::find(codepages.begin(), codepages.end(), string[i]&0xFFFFFF80)-codepages.begin();
            if (bank < codepages.size()){
                matrix.setTile(x++, y, (bank<<7)|(string[i]&0x7F));
            } else {
                matrix.setTile(x++, y, 0x7F);
            }
        }
    }
    
    return matrix;

    
}


TileMatrix TextRenderer::render (std::u32string text, ChrFont *font, int maxChars, bool preprocess, bool inverted){
    auto wrappedText = TextRenderer::wrapText(text, maxChars, preprocess);
    TileMatrix matrix = TextRenderer::render(&wrappedText, font, inverted);
    return matrix;
}

TileMatrix TextRenderer::render (std::string text, ChrFont *font, int maxChars, bool preprocess, bool inverted){
    std::u32string text32 = To_UTF32(text);
    TileMatrix matrix = TextRenderer::render(text32, font, maxChars, preprocess, inverted);
    return matrix;
}

#endif

#endif  // __TEXTRENDERER_INCLUDED__