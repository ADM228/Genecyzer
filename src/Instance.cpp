#ifndef __INSTANCE_CPP_INCLUDED__
#define __INSTANCE_CPP_INCLUDED__

#include <cmath>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <ios>

#include "tinyfiledialogs.h"

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

#include "RIFFLoader.cpp"

#include "Instance.hpp"

#include "Renderer/InstrumentRenderer.cpp"
#include "Renderer/TrackerRenderer.cpp"

const char * const filter[] = {"*.gczr"};

constexpr uint64_t UPDATE_SCALE = 1;
constexpr uint64_t UPDATE_INST_POS = 2;
constexpr uint64_t UPDATE_INST_LIST = 4;
constexpr uint64_t UPDATE_TRACKER = 8;
constexpr uint64_t UPDATE_TRACKER_SELECTION = 16;

constexpr uint16_t MOUSE_DOWN = 1;

#define getGlobalBounds_bottom(sprite) (sprite.getGlobalBounds().top + sprite.getGlobalBounds().height)

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
        auto data = std::ifstream();
        data.open(filename, std::ios_base::binary | std::ios_base::in);
        auto file = RIFF::RIFFReader();
        file.open(data);
        RIFFLoader::loadRIFFFile(file, activeProject);
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


    #ifdef FILETEST

        size_t size = 0;
        RIFF::RIFFWriter tmp; tmp.openMem();
        RIFFLoader::saveRIFFFile(tmp, activeProject);
        tmp.close(); auto ptr = tmp.rw->fh; size = tmp.rw->size;
        printByteArray(ptr, size, 16);
        free(ptr);

        auto outFilename = tinyfd_saveFileDialog("Save the file", "outTest.gczr", 1, filter, "Genecyzer project file");
        if (outFilename != NULL) {
		{ 
			auto outData = std::ofstream(outFilename, std::ios_base::out | std::ios_base::binary);
            RIFF::RIFFWriter writer;

            writer.open(outData);

            // saveInternal(writer);
            RIFFLoader::saveRIFFFile(writer, activeProject);
            writer.close();
			outData.close();
		}
		{
			auto outData = std::ifstream(outFilename, std::ios_base::binary | std::ios_base::in);
			char tmp[1024];
			outData.read(tmp, 1024);
			printByteArray(tmp, 1024);
		}
        }
    #endif
    
}

void Instance::addMonospaceFont(const void * data, uint32_t size, std::vector<uint32_t> codepages){
    font.init(data, size, codepages, 1);
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

#endif // __INSTANCE_CPP_INCLUDED__
