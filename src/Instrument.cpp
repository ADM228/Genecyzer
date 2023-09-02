#ifndef __INSTRUMENT_INCLUDED__
#define __INSTRUMENT_INCLUDED__

#include <string>
#include <vector>
#include "StrConvert.cpp"

#pragma region classDefinitions

class Macro {
    public:
        Macro(uint8_t = 1, uint8_t = 0);
        uint8_t speed;
        uint8_t loopingType;
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

};

#pragma endregion
#pragma region macroImplementation

Macro::Macro(uint8_t speed, uint8_t loopingType){
    this->speed = speed;
    this->loopingType = loopingType;
    this->data.resize(0);
}

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
