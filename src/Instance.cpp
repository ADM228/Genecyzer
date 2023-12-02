#ifndef __INSTANCE_INCLUDED__
#define __INSTANCE_INCLUDED__

#include "SFML/Window/Mouse.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

#include "Tile.cpp"
#include "ChrFont.cpp"
#include "Instrument.cpp"
#include "Utils.cpp"
#include "TextRenderer.cpp"
#include "Tracker.cpp"
#include "Effect.cpp"
#include "Project.cpp"
#include "ModularSynth.cpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <chrono>
#include "tinyfiledialogs.h"

char const * filter[] = {"*.gczr"};

constexpr uint64_t UPDATE_SCALE = 1;
constexpr uint64_t UPDATE_INST_POS = 2;
constexpr uint64_t UPDATE_INST_LIST = 4;
constexpr uint64_t UPDATE_TRACKER = 8;
constexpr uint64_t UPDATE_TRACKER_SELECTION = 16;

constexpr uint16_t MOUSE_DOWN = 1;

#define TILE_SIZE 8
#define INST_ENTRY_WIDTH 16

#define getGlobalBounds_bottom(sprite) (sprite.getGlobalBounds().top + sprite.getGlobalBounds().height)

class Instance {

    public:
        Instance();
        void ProcessEvents();
        void Update();

        void addMonospaceFont(const uint8_t* data, uint32_t size, std::vector<uint32_t> codepages);


        bool isWindowOpen(){ return window.isOpen(); };

    protected:
        void eventHandleInstList (int, int, uint8_t, bool);
        void renderInstList();
        void renderTracker();
        void updateInstPage();
        void updateTrackerPos();
        void updateTrackerSelection();
        void renderBeatsTexture();

    private:
        uint8_t instSelected = 0;
        uint8_t scale = 1;
        uint8_t upperHalfMode = 0;
        uint8_t lowerHalfMode = 0;

        bool singleTileTrackerRender = 1;

        int debug = 0;

        sf::Sprite instrumentSprite;

        #pragma region Update
        bool forceUpdateAll = 0;

        uint64_t updateSections = 0;

        std::vector<uint8_t> instrumentsToUpdate;
        int selectionBounds[4];
        #pragma endregion

        sf::RenderWindow window;
        sf::View InstrumentView;
        sf::View TrackerView;

        ChrFont font;

        sf::Texture instrumentTexture;
        TileMatrix trackerMatrix;
        sf::Texture beatsTexture;

        Project activeProject;

        uint16_t mouseFlags;

        sf::Event::MouseButtonEvent lastMousePress;

        ModSynthBezier bezierTest;
};

Instance::Instance() {
    window.create(sf::VideoMode(200, 200), "Genecyzer");
    window.setFramerateLimit(60);
    InstrumentView.reset(sf::FloatRect(0.f, 0.f, 200.f, 200.f));
    TrackerView.reset(sf::FloatRect(0.f,0.f,200.f,200.f));
    forceUpdateAll = 1;
    lastMousePress.x = 0;
    lastMousePress.y = 0;
    selectionBounds[0] = 0;
    selectionBounds[1] = 0;
    

    #ifdef FILETEST
        auto filename = tinyfd_openFileDialog("Open a Genecyzer project file", NULL, 1, filter, "Genecyzer project file", 0);
        auto data = std::fstream();
        data.open(filename, std::ios_base::binary | std::ios_base::in);

    #else


    std::vector<uint8_t> data = {
        0x52, 0x49, 0x46, 0x46,         // "RIFF"
        0x18, 0x00, 0x00, 0x00,         // filesize 24 bytes
        0x47, 0x43, 0x5A, 0x52,         // "GCZR" 
        0x76, 0x65, 0x72, 0x20,         // "ver "
        0x0C, 0x00, 0x00, 0x00,         // chunksize 12 bytes
        0x44, 0x65, 0x76, 0x20,         // "Dev "
        0x4D, 0x61, 0x69, 0x6E,         // "Main"
        0x00, 0x00, 0x00, 0x00          // ver 0
    };

    #endif

    activeProject = Project();
    activeProject.Load(data);


    

    // TODO: convert into raw data

    // constexpr uint8_t notes[] = {
    //     0+3*12, 0+3*12, 0+3*12, 0+4*12, 0+3*12,
    //     3+3*12, 3+4*12, 3+3*12,
    //     6+3*12, 6+4*12, 9+3*12,
    //     0+3*12, 0+3*12, 0+3*12, 0+4*12, 0+3*12,
    //     3+3*12, 3+4*12, 3+3*12,
    //     6+3*12, 6+4*12, 9+3*12
    // };

    // for (int i = 0; i < 22; i++){
    //     cells.push_back(TrackerCell());
    //     cells.back().noteValue = notes[i];
    // }

}

void Instance::addMonospaceFont(const uint8_t *data, uint32_t size, std::vector<uint32_t> codepages){
    unsigned char * fontPointer = const_cast<unsigned char *>(data);
    font.init(fontPointer, size, codepages, 1);
}

void Instance::ProcessEvents(){



    sf::Event event;

    updateSections = 0;

    instrumentsToUpdate.clear();

    while (window.pollEvent(event))
    {

        if (event.type == sf::Event::Closed)
            window.close();
        else if (event.type == sf::Event::Resized){
            
            //scale = std::max(static_cast<int>(std::ceil(event.size.height/(4*8*TILE_SIZE))), 1);
            
            updateSections |= UPDATE_SCALE;

            //int width = std::ceil((event.size.width/scale)/TILE_SIZE);


        } else if (event.type == sf::Event::KeyPressed){
            
            if (event.key.code == sf::Keyboard::Down)
                eventHandleInstList (255, +1, 255, false);
            else if (event.key.code == sf::Keyboard::Up)
                eventHandleInstList (0, -1, 0, true);
            else if (event.key.code == sf::Keyboard::Right)
                eventHandleInstList (256-8, +8, 255, false);
            else if (event.key.code == sf::Keyboard::Left)
                eventHandleInstList (7, -8, 0, true);
            else if (event.key.code == sf::Keyboard::E){
                singleTileTrackerRender ^= 1;
                updateSections |= UPDATE_TRACKER;
            } else if (event.key.code == sf::Keyboard::Equal && event.key.control) {
                scale++;
                updateSections |= UPDATE_SCALE;
            } else if (event.key.code == sf::Keyboard::Hyphen && event.key.control && scale > 1) {
                scale--;
                updateSections |= UPDATE_SCALE;
            } else if (event.key.code == sf::Keyboard::Apostrophe) {
                lowerHalfMode ^= 1;
                forceUpdateAll = 1;
            }
        } else if (event.type == sf::Event::MouseButtonPressed) {
            lastMousePress = event.mouseButton;
            // do sumn for time
            if (event.mouseButton.button == sf::Mouse::Left) mouseFlags |= MOUSE_DOWN;
        } else if (event.type == sf::Event::MouseMoved || event.type == sf::Event::TouchMoved) {
            if (mouseFlags & MOUSE_DOWN) {
                // determine region
                if (
                    lowerHalfMode == 0 &&
                    lastMousePress.y > scale*TILE_SIZE*(8+5) &&
                    lastMousePress.x > scale*TILE_SIZE*(3+1)
                ) {
                    selectionBounds[0] = lastMousePress.x  / (scale*TILE_SIZE);
                    selectionBounds[1] = lastMousePress.y  / (scale*TILE_SIZE);
                    selectionBounds[2] = event.mouseMove.x / (scale*TILE_SIZE);
                    selectionBounds[3] = event.mouseMove.y / (scale*TILE_SIZE);
                    updateSections |= UPDATE_TRACKER_SELECTION;
                } else if (
                    lowerHalfMode == 1 &&
                    lastMousePress.y > scale*TILE_SIZE*8
                ) {
                    selectionBounds[0] = event.mouseMove.x;
                    selectionBounds[1] = event.mouseMove.y;
                }
            }
        } else if (event.type == sf::Event::MouseButtonReleased) {
            if (event.mouseButton.button == sf::Mouse::Left) mouseFlags &= ~MOUSE_DOWN;
        }
    }
}

void Instance::Update(){
        
    #pragma region ConditionalUpdates

    if (forceUpdateAll){
        updateSections = 0xFFFFFFFFFFFFFFFF;
        instrumentsToUpdate.clear();
        forceUpdateAll = 0;
    }

    if (updateSections & UPDATE_INST_POS)
        renderInstList();

    if (updateSections & (UPDATE_INST_POS | UPDATE_SCALE))   
        updateInstPage();
        
    switch (lowerHalfMode) {
        case 0:
            if (updateSections & (UPDATE_SCALE | UPDATE_TRACKER)){
                renderTracker();
                updateTrackerPos();
                renderBeatsTexture();
            }
            if (updateSections & UPDATE_TRACKER_SELECTION)
                updateTrackerSelection();
            break;

        case 1:
            if (updateSections & UPDATE_SCALE)
                updateTrackerPos();
            break;

    }

    #pragma endregion
    #pragma region AlwaysUpdates

    window.clear(sf::Color(255,255,0,0));

    window.setView(InstrumentView);
    window.draw(instrumentSprite);

    window.setView(TrackerView);

    switch (lowerHalfMode) {
        case 0: {
            window.draw(trackerMatrix);

            auto beats = sf::RectangleShape(sf::Vector2f(beatsTexture.getSize().x, beatsTexture.getSize().y));
            beats.setPosition(0, 5*TILE_SIZE);
            beats.setTexture(&beatsTexture);
            beats.setScale(2, TILE_SIZE);
            window.draw(beats);
            break;
        }

        case 1: {
            auto deeznuts = std::array<sf::Vector2f, 2> 
            {sf::Vector2f((float)selectionBounds[0] / scale, (float)selectionBounds[1] / scale - 8*TILE_SIZE), {100, 100}};
            
            // auto start = std::chrono::high_resolution_clock::now();
            bezierTest.updatePosition(deeznuts);
            bezierTest.calculate((float)scale/16, 3.f/scale, false);
            // auto elapsed = std::chrono::high_resolution_clock::now() - start;
            // uint64_t microseconds = std::chrono::duration_cast<std::chrono::microseconds>( elapsed).count();
            // printf("Elapsed: %08lu \n", microseconds); fflush(stdout);
            window.draw(bezierTest);
            break;
        }
    }

    // 24, 36, 54

    // auto selection = sf::RectangleShape(
    //     sf::Vector2f((selectionBounds[2] - selectionBounds[0])*TILE_SIZE, 
    //     (selectionBounds[3] - selectionBounds[1])*TILE_SIZE));
    // selection.setPosition(selectionBounds[0]*TILE_SIZE, 
    //     (selectionBounds[1]-8)*TILE_SIZE);
    // selection.setFillColor(sf::Color(128, 128, 255, 80));
    // window.draw(selection);

    // tiles.render(0,0,&window, font[0]);
    // window.draw(tile.renderVertex, sf::RenderStates(&font[0]));
    // window.draw(tile1.renderVertex, sf::RenderStates(&font[0]));
    // window.draw(tile2.renderVertex, sf::RenderStates(&font[0]));
    // window.draw(tile3.renderVertex, sf::RenderStates(&font[0]));
    // window.setView(TrackerView);
    
    window.display();
    #pragma endregion

}

void Instance::eventHandleInstList (int limit, int modifier, uint8_t replacement, bool more){
    if ((!more && instSelected < limit) || (more && instSelected > limit))
        instrumentsToUpdate.push_back(instSelected + modifier);
    else
        instrumentsToUpdate.push_back(replacement);
    
    if (instrumentsToUpdate.back() != instSelected){
        instrumentsToUpdate.push_back(instSelected);
        instSelected = instrumentsToUpdate[instrumentsToUpdate.size()-2] & 0xFF;
        updateSections |= UPDATE_INST_POS;
    } else
        instrumentsToUpdate.pop_back();
}

void Instance::renderInstList () {
    #define INST_WIDTH  512
    #define INST_HEIGHT 8

    auto instruments = &activeProject.instruments;

    if (instrumentsToUpdate.size() == 0){   // Update the entire list


        TileMatrix instrumentMatrix = TileMatrix(INST_WIDTH, INST_HEIGHT, 0x20);
        uint8_t instNumber;
        uint8_t palette;


        for (int i = 0; i < INST_WIDTH; i+=INST_ENTRY_WIDTH){
            instNumber = i>>1;
            for (int j = 0; j < INST_HEIGHT; j++){
                std::string output;
                if (instNumber < instruments->size()){
                    char numchar[5];
                    std::snprintf(numchar, 5, "%02X:", instNumber);
                    std::string num(numchar);
                    output = num + (*instruments)[instNumber].getName() + " ";
                    palette = (*instruments)[instNumber].getPalette();
                    if (palette == 0) palette = 7;
                } else { 
                    char numchar[17];
                    std::snprintf(numchar, 17, "%02X:             ", instNumber);
                    std::string num(numchar);
                    output = num;
                    palette = 7;                   
                }
                TileMatrix string = TextRenderer::render(output, &font, 15);
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
            if (instNumber < instruments->size()){
                char numchar[5];
                std::snprintf(numchar, 5, "%02X:", instNumber);
                std::string num(numchar);
                output = num + (*instruments)[instNumber].getName() + " ";
                palette = (*instruments)[instNumber].getPalette();
                if (palette == 0) palette = 7;
            } else { 
                char numchar[17];
                std::snprintf(numchar, 17, "%02X:             ", instNumber);
                std::string num(numchar);
                output = num;
                palette = 7;                   
            }
            TileMatrix string = TextRenderer::render(output, &font, 15);
            string.resize(INST_ENTRY_WIDTH, 1);
            string.fillInvert(instNumber == instSelected);
            string.fillPaletteRect(0, 0, INST_ENTRY_WIDTH, 1, palette);
            instrumentTexture.update(string.renderToTexture(font.texture), (instNumber&0xF8)<<(1+3), (instNumber&0x07)<<3);
        }
    }
}

void Instance::renderTracker () {
    
    #define ROW_SEPARATOR 0x04
    #define COL_SEPARATOR 0x06

    #define INTERSECTION_ALL4 0x05
    #define INTERSECTION_NOUP 0x07
    #define INTERSECTION_NORT 0x08

    #define HEADER_HEIGHT 5

    #define TRACKER_ROW_WIDTH(effectColumns) trackerNoteWidth+1+2+(1+3)*effectColumns

    uint8_t trackerNoteWidth = ((uint8_t)!singleTileTrackerRender)+2;
    size_t widthInTiles = std::ceil((window.getSize().x/scale)/TILE_SIZE);
    size_t heightInTiles = std::ceil((window.getSize().y/scale)/TILE_SIZE);
    size_t rows = activeProject.patterns[0].rows;
    #pragma region header
    TileMatrix header = TileMatrix(widthInTiles, HEADER_HEIGHT, 0x20);
    header.fillRow(0, ROW_SEPARATOR);
    header.fillRow(2, ROW_SEPARATOR);
    header.fillRow(4, ROW_SEPARATOR);
    #pragma endregion

    #pragma region text
    //TileMatrix text = TextRenderer::render(testString, &font, widthInTiles, false, false);
    //text.resize(text.getWidth(), std::max((int)text.getHeight(), (int)activeProject.patterns[0].cells[0].size()));

    size_t widthOfTracker = 3;

    for (auto & column : activeProject.effectColumnAmount) {
        widthOfTracker += TRACKER_ROW_WIDTH(column) + 1;
    }

    TileMatrix text = TileMatrix(widthOfTracker, std::min(heightInTiles, rows));
    int textHeight = text.getHeight();


    {
        std::vector<uint16_t> tracker_separator_columns(0);
        char rowNum[4];
        for (int i = 0; i < rows && i < textHeight; i++){
            std::snprintf(rowNum, 4, "%03X", i);
            auto rowNumMatrix = TextRenderer::render(std::string(rowNum), &font, 3, 1, 0);
            text.copyRect(0, i, 3, 1, &rowNumMatrix, 0, 0);
        }
        
        int tileCounter = 4;
        for (int i = 0; i < 8; i++) {

            auto* patternData = &activeProject.patternData[activeProject.patterns[0].cells[i]]; 

            for (int j = 0; j < rows && j < textHeight; j++) {
                auto row = (*patternData)[j].render(activeProject.effectColumnAmount[i], singleTileTrackerRender);
                text.copyRect(tileCounter, j, TRACKER_ROW_WIDTH(activeProject.effectColumnAmount[i]), 1, &row, 0, 0);
            }

            tracker_separator_columns.push_back(tileCounter-1); 
            tileCounter += TRACKER_ROW_WIDTH(activeProject.effectColumnAmount[i]) + 1;
        }

        for (auto column : tracker_separator_columns) {
            if (widthInTiles > column){
                header.setTile(column, 4, INTERSECTION_NOUP);
                text.fillCol(column, COL_SEPARATOR);
            }
        }
    }
    #pragma endregion

    #pragma region putTogether
    trackerMatrix = TileMatrix(widthInTiles+1, textHeight+HEADER_HEIGHT, 0x20);
    
    trackerMatrix.copyRect(0, 0, widthInTiles, HEADER_HEIGHT, &header, 0, 0);
    trackerMatrix.copyRect(0, HEADER_HEIGHT, std::min(widthInTiles, widthOfTracker), textHeight, &text, 0, 0);
    trackerMatrix.setTexture(font.texture);
    #pragma endregion

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

void Instance::updateTrackerPos () {
    TrackerView.reset(sf::FloatRect(0, 0, window.getSize().x, (double)window.getSize().y/scale));
    TrackerView.setViewport(sf::FloatRect(0.f, (double)(INST_HEIGHT*TILE_SIZE*scale)/(double)window.getSize().y, scale, 1));
}

void Instance::updateTrackerSelection () {
    int tileX = 3;
    uint8_t trackerNoteWidth = ((uint8_t)!singleTileTrackerRender)+2;

    int x1 = selectionBounds[0], x2 = selectionBounds[2];
    int y1 = selectionBounds[1], y2 = selectionBounds[3];
    int beginX = std::max(std::min(x1, x2), 3),
        endX = std::min(std::max(x1, x2), (int)trackerMatrix.getWidth());
    int beginY = std::min(y1, y2),
        endY = std::max(y1, y2);
    x1 = -1, x2 = -1, y1 = std::max(beginY-8, 5), y2 = std::min(endY-8, (int)trackerMatrix.getHeight());    

    for (int i = 0; i < 8; i++){
        if (beginX >= tileX && beginX < tileX+trackerNoteWidth+1) {
            // Begins at the note tile
            x1 = tileX+1;
            break;
        } else if (beginX >= tileX+trackerNoteWidth+1 && beginX < tileX+trackerNoteWidth+4) {
            // Begins at instrument tile
            x1 = tileX+trackerNoteWidth+1+1;
            break;
        } else if (beginX >= tileX+trackerNoteWidth+4 && beginX <= tileX+TRACKER_ROW_WIDTH(activeProject.effectColumnAmount[i])) {
            // Begins at an effect tile
            for (int j = 0; j < std::max((int)activeProject.effectColumnAmount[i], 1); j++) {
                if (beginX >= tileX+trackerNoteWidth+4+4*j && beginX <= tileX+trackerNoteWidth+4+4+4*j) { 
                    x1 = tileX+trackerNoteWidth+1+4+4*j;
                    break;
                }
            }
            break;
        }
        tileX += TRACKER_ROW_WIDTH(activeProject.effectColumnAmount[i])+1;
    }

    tileX = 3;
    for (int i = 0; i < 8; i++){
        if (endX > tileX && endX <= tileX+trackerNoteWidth+1) {
            // Ends at the note tile
            x2 = tileX+trackerNoteWidth+1;
            break;
        } else if (endX > tileX+trackerNoteWidth+1 && endX <= tileX+trackerNoteWidth+4) {
            // Ends at instrument tile
            x2 = tileX+trackerNoteWidth+1+3;
            break;
        } else if (endX > tileX+trackerNoteWidth+4 && endX <= tileX+TRACKER_ROW_WIDTH(activeProject.effectColumnAmount[i])+1) {
            // Ends at an effect tile
            for (int j = 0; j < std::max((int)activeProject.effectColumnAmount[i], 1); j++) {
                if (endX > tileX+trackerNoteWidth+4+4*j && endX <= tileX+trackerNoteWidth+1+4+4+4*j) { 
                    x2 = tileX+trackerNoteWidth+4+4+4*j;
                    break;
                }
            }
            break;
        }
        tileX += TRACKER_ROW_WIDTH(activeProject.effectColumnAmount[i])+1;
    }

    trackerMatrix.fillInvert(false);

    if (x1 == -1) x1 = beginX >= tileX ? tileX-3 : 4;
    if (x2 == -1) x2 = endX >= tileX ? tileX : x1;
    if (x1 >= x2 || y1 >= y2) return;
    x2 = std::min(x2, (int)trackerMatrix.getWidth());

    trackerMatrix.fillInvertRect(x1, y1, x2-x1, y2-y1, true);
}

void Instance::renderBeatsTexture() {
    int rows = std::min(activeProject.patterns[0].rows, (size_t)trackerMatrix.getHeight()-5);
    if (!(trackerMatrix.getWidth() && rows)) return;
    auto * maj_beats = &activeProject.patterns[0].beats_major;
    auto * min_beats = &activeProject.patterns[0].beats_minor;
    auto colors = new uint8_t[rows]();
    auto pixels = new uint8_t[rows*trackerMatrix.getWidth()*TILE_SIZE/2*sizeof(sf::Color)](); // automatically zeroes out alpha value
    for (int i = 0; i < rows;) {
        for (int j = 0; j < min_beats->size() && i < rows; j++) {

            colors[i] = 1;
            i += (*min_beats)[j];
        }
    }
    for (int i = 0; i < rows;) {
        for (int j = 0; j < maj_beats->size() && i < rows; j++) {
            colors[i] = 2;
            i += (*maj_beats)[j];
        }
    }

    size_t pixelIndex = 0;

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < trackerMatrix.getWidth(); j++, pixelIndex+=16) {
            if (colors[i] > 0) {
                for (int k = 0; k < 16; k+=4){
                    pixels[pixelIndex+k+0] = 128;
                    pixels[pixelIndex+k+1] = 128;
                    pixels[pixelIndex+k+2] = 255;
                    pixels[pixelIndex+k+3] = colors[i] == 1 ? 48 : 96;
                }
            }
            if (trackerMatrix[i+HEADER_HEIGHT][j].tileIndex == COL_SEPARATOR){
                pixels[pixelIndex+1*4+3] = 0;
                pixels[pixelIndex+2*4+3] = 0;
            }
        }
    }

    beatsTexture.create(trackerMatrix.getWidth()*TILE_SIZE/2, rows);
    beatsTexture.update(pixels);

    delete[] colors;
    delete[] pixels;
}

#endif // __INSTANCE_INCLUDED__
