#include <SFML/System/Vector2.hpp>
#include <vector>
#include "Instance.hpp"

void Instance::renderInstList () {
    auto & instruments = activeProject.globalInstruments;    // TODO: global + local, accomodate here

    if (instrumentsToUpdate.size() == 0){   // Update the entire list


        TileMatrix instrumentMatrix = TileMatrix(INST_WIDTH, INST_ENTRIES_PER_COLUMN, 0x20);
        uint8_t instNumber;
        uint8_t palette;


        for (int i = 0; i < INST_COLUMNS; i++){
            instNumber = i * INST_ENTRIES_PER_COLUMN;
            for (int j = 0; j < INST_ENTRIES_PER_COLUMN; j++){
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
                instrumentMatrix.copyRect(i * INST_ENTRY_WIDTH, j, INST_ENTRY_WIDTH, 1, &string, 0, 0);
                instNumber++;
            }
        }

        instrumentTexture.resize(sf::Vector2u{INST_WIDTH*TILE_SIZE, INST_ENTRIES_PER_COLUMN*TILE_SIZE});
        instrumentTexture.update(instrumentMatrix.renderToTexture(font.texture));

        instrumentSprite.setTextureRect(sf::IntRect(
            sf::Vector2i{0, 0},
            sf::Vector2i(instrumentTexture.getSize().x, instrumentTexture.getSize().y)
        ));
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
            instrumentTexture.update(string.renderToTexture(font.texture), sf::Vector2u(
                (instNumber / INST_ENTRIES_PER_COLUMN) * INST_ENTRY_WIDTH * TILE_SIZE,
                (instNumber % INST_ENTRIES_PER_COLUMN) * TILE_SIZE
            ));
        }
    }
}


void Instance::updateInstPage () {
    if (
        ((instSelected&0xF8)+TILE_SIZE/2)*INST_ENTRY_WIDTH*scale*2 < window.getSize().x &&
        window.getSize().x >= TILE_SIZE*INST_ENTRY_WIDTH*scale
    )
        // (columnIdx + 1/2) * TILE_SIZE * INST_ENTRY_WIDTH * scale < windowWidth/2
        // i.e. if the center X of the column < center X of window
        // And the window width > than one instrument column

        // Align left
        InstrumentView = sf::View(sf::FloatRect(
            {0, 0}, 
            {(float)window.getSize().x, INST_ENTRIES_PER_COLUMN*TILE_SIZE}
        ));

    else if (
        (INST_COLUMNS*TILE_SIZE-(instSelected&0xF8)-TILE_SIZE/2)*INST_ENTRY_WIDTH*scale*2 < window.getSize().x &&
        window.getSize().x >= TILE_SIZE*INST_ENTRY_WIDTH*scale
    )
        // (INST_COLUMNS - (columnIdx + 1/2)) * TILE_SIZE * INST_ENTRY_WIDTH * scale < windowWidth/2
        // i.e. if the distance between the right edge and the center X of the column <
        // < center X of window
        // And the window width > than one instrument column
        
        // Align right
        InstrumentView = sf::View(sf::FloatRect(
            sf::Vector2f(INST_WIDTH*TILE_SIZE-(window.getSize().x/(double)scale), 0),
            sf::Vector2f(window.getSize().x, INST_ENTRIES_PER_COLUMN*TILE_SIZE)
        ));
    else
        // Center the window around the selected column
        // Left X: ((columnIdx + 1/2) * TILE_SIZE * INST_ENTRY_WIDTH) - ((windowWidth / 2) / scale)
        // i.e. center X of column - center X of window / scale
        // The rest are obvious
        InstrumentView = sf::View(sf::FloatRect(sf::Vector2f((
                ((instSelected>>3)+1)*TILE_SIZE*INST_ENTRY_WIDTH - 
                TILE_SIZE*(int)(INST_ENTRY_WIDTH / 2) -
                (window.getSize().x/(double)(scale*2))), 0),
            sf::Vector2f(window.getSize().x, INST_ENTRIES_PER_COLUMN*TILE_SIZE)));

    InstrumentView.setViewport(sf::FloatRect(
        {0, 0}, sf::Vector2f(scale, (double)(INST_ENTRIES_PER_COLUMN*TILE_SIZE)/window.getSize().y*scale)
    ));
}