#ifndef __INSTRUMENT_INCLUDED__
#define __INSTRUMENT_INCLUDED__

#include <array>
#include <string>
#include <vector>
#include "Utils.cpp"

#pragma region classDefinitions

struct Macro {
    uint8_t speed = 1;
    uint8_t loopingType = 0;
};

struct ModSynthMacro : Macro {
    std::vector<uint16_t> pointers;
};

struct RestOfMacros : Macro {
    std::vector<uint8_t> data;
};

constexpr uint32_t INSTRUMENT_NAME_LENGTH = 12;
class Instrument {
    public:
        Instrument();

        void setName(std::string);
        void setName(std::u32string);
        std::string getName();

        void setPalette(uint8_t palette);
        uint8_t getPalette();

    private:
        uint32_t name[INSTRUMENT_NAME_LENGTH];
        uint8_t palette;

        std::array<Macro, 5> macros;

};

#pragma endregion
#pragma region instrumentImplementation

Instrument::Instrument(){
    for (int i = 0; i < INSTRUMENT_NAME_LENGTH; i++){
        this->name[i] = 0x20;   // Space
    }
    this->palette = 7;
}

void Instrument::setName(std::string name){
    std::u32string UTF32_name = To_UTF32(name);
    setName(UTF32_name);
}

void Instrument::setName(std::u32string name){
    for (int i = 0; i < INSTRUMENT_NAME_LENGTH && i < name.size(); i++){
        this->name[i] = name[i];
    }
}

std::string Instrument::getName(){
    std::u32string UTF32_name;
    for (int i = 0; i < INSTRUMENT_NAME_LENGTH; i++){
        // printf("%04x ", this->name[i]);
        UTF32_name += this->name[i];
    }
    // printf("\n"); fflush(stdout);
    std::string output = To_UTF8(UTF32_name);
    return output;
}

void Instrument::setPalette(uint8_t palette){
    this->palette = palette;
}

uint8_t Instrument::getPalette(){
    return this->palette;
}

#pragma endregion

#endif // __INSTRUMENT_INCLUDED__
