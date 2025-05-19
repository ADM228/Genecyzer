#include <vector>
#include "Instance.hpp"

void Instance::renderInstList () {
    auto & instruments = activeProject.globalInstruments;    // TODO: global + local, accomodate here

    if (instrumentsToUpdate.size() == 0){   // Update the entire list


        TileMatrix instrumentMatrix = TileMatrix(INST_WIDTH, INST_HEIGHT, 0x20);
        uint8_t instNumber;
        uint8_t palette;


        for (int i = 0; i < INST_WIDTH; i+=INST_ENTRY_WIDTH){
            instNumber = i>>1;
            for (int j = 0; j < INST_HEIGHT; j++){
                std::string output;
                if (instNumber < instruments.size()){
                    char numchar[5];
                    std::snprintf(numchar, 5, "%02X:", instNumber);
                    std::string num(numchar);
                    output = num + instruments[instNumber].getName() + " ";
                    palette = instruments[instNumber].getPalette();
                    if (palette == 0) palette = 7;
                } else { 
                    char numchar[17];
                    std::snprintf(numchar, 17, "%02X:             ", instNumber);
                    std::string num(numchar);
                    output = num;
                    palette = 7;                   
                }
                TileMatrix string = TextRenderer::render(output, font, 15);
                string.resize(INST_ENTRY_WIDTH, 1);
                string.fillInvert(instNumber == instSelected);
                string.fillPaletteRect(0, 0, INST_ENTRY_WIDTH, 1, palette);
                instrumentMatrix.copyRect(i, j, INST_ENTRY_WIDTH, 1, &string, 0, 0);
                instNumber++;
            }
        }

        instrumentTexture.create(INST_WIDTH*TILE_SIZE, INST_HEIGHT*TILE_SIZE);
        // instrumentTexture->update((new TileMatrix(1, INST_HEIGHT, 0x20))->renderToTexture(font->texture), INST_WIDTH*TILE_SIZE, 0);
        instrumentTexture.update(instrumentMatrix.renderToTexture(font.texture));

        instrumentSprite.setTextureRect(sf::IntRect(0, 0, instrumentTexture.getSize().x, instrumentTexture.getSize().y));
        instrumentSprite.setTexture(instrumentTexture);
    } else {    // Only update certain instruments
        while (instrumentsToUpdate.size() > 0){
            uint8_t instNumber = instrumentsToUpdate.back();
            instrumentsToUpdate.pop_back();
            uint8_t palette;
            std::string output;
            if (instNumber < instruments.size()){
                char numchar[5];
                std::snprintf(numchar, 5, "%02X:", instNumber);
                std::string num(numchar);
                output = num + instruments[instNumber].getName() + " ";
                palette = instruments[instNumber].getPalette();
                if (palette == 0) palette = 7;
            } else { 
                char numchar[17];
                std::snprintf(numchar, 17, "%02X:             ", instNumber);
                std::string num(numchar);
                output = num;
                palette = 7;                   
            }
            TileMatrix string = TextRenderer::render(output, font, 15);
            string.resize(INST_ENTRY_WIDTH, 1);
            string.fillInvert(instNumber == instSelected);
            string.fillPaletteRect(0, 0, INST_ENTRY_WIDTH, 1, palette);
            instrumentTexture.update(string.renderToTexture(font.texture), (instNumber&0xF8)<<(1+3), (instNumber&0x07)<<3);
        }
    }
}


void Instance::updateInstPage () {
    if (((instSelected&0xF8)+TILE_SIZE/2)*INST_ENTRY_WIDTH*scale*2 < window.getSize().x && window.getSize().x >= TILE_SIZE*INST_ENTRY_WIDTH*scale)
        // = (IS>>3)*TILE_SIZE*INST_ENTRY_WIDTH*scale < winWidth/2 - TILE_SIZE*INST_ENTRY_WIDTH*scale/2
        // Align left
        InstrumentView.reset(sf::FloatRect(0, 0, window.getSize().x, INST_HEIGHT*TILE_SIZE));
    else if ((32*TILE_SIZE-(instSelected&0xF8)-TILE_SIZE/2)*INST_ENTRY_WIDTH*scale*2 < window.getSize().x && window.getSize().x >= TILE_SIZE*INST_ENTRY_WIDTH*scale)
        // = (32-IS>>3)*TILE_SIZE*INST_ENTRY_WIDTH*scale < winWidth/2 + TILE_SIZE*INST_ENTRY_WIDTH*scale/2
        // Align right
        InstrumentView.reset(sf::FloatRect(32*TILE_SIZE*INST_ENTRY_WIDTH-(window.getSize().x/(double)scale), 0, window.getSize().x, INST_HEIGHT*TILE_SIZE));
    else
        InstrumentView.reset(sf::FloatRect(((instSelected>>3)+1)*TILE_SIZE*INST_ENTRY_WIDTH-(window.getSize().x/(double)(scale*2))-TILE_SIZE*8, 0, window.getSize().x, INST_HEIGHT*TILE_SIZE));
    InstrumentView.setViewport(sf::FloatRect(0, 0, scale, (double)(INST_HEIGHT*TILE_SIZE)/window.getSize().y*scale));
}