#ifndef __TEXTRENDERER_INCLUDED__
#define __TEXTRENDERER_INCLUDED__

#include <string>
#include <vector>
#include "Tile.cpp"

class TextRenderer {
    public:
        static std::u32string preprocess(std::u32string string);
        static std::u32string render(std::u32string string, std::vector<uint32_t> *charsOnLine, int maxChars = -1, bool preprocess = 1);
        static TileMatrix render (std::u32string text, int maxChars = -1, bool preprocess = 1);
    private:
        TextRenderer() {};
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
        else
            output_text.push_back(character);
    }
    std::u32string out_string(output_text.data());
    return out_string;
}

#endif  // __TEXTRENDERER_INCLUDED__