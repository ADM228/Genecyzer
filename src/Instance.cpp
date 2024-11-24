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
    

    auto filename = tinyfd_openFileDialog("Open a Genecyzer project file", NULL, 1, filter, "Genecyzer project file", 0);
    if (filename == NULL) {
        activeProject = Project::createDefault();
    } else {
        std::string sfilename(filename);
        free(filename); 
        auto data = std::ifstream();
        data.open(filename, std::ios_base::binary | std::ios_base::in);
        auto file = RIFF::RIFFReader();
        file.open(data);
        RIFFLoader::loadRIFFFile(file, activeProject);

        size_t size = 0;
        RIFF::RIFFWriter tmp; tmp.openMem();
        RIFFLoader::saveRIFFFile(tmp, activeProject);
        tmp.close(); auto ptr = tmp().fh; size = tmp().size;
        printByteArray(ptr, size, 16);
        free(ptr);

        auto outFilename = tinyfd_saveFileDialog("Save the file", "outTest.gczr", 1, filter, "Genecyzer project file");
        if (outFilename != NULL) {
        std::string sOutFilename(outFilename);
        free (outFilename);
        { 
            auto outData = std::ofstream(sOutFilename, std::ios_base::out | std::ios_base::binary);
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
    }
    
}

void Instance::addMonospaceFont(const void * data, uint32_t size, std::vector<uint32_t> codepages){
    font.init(data, size, codepages, 1);
}

void Instance::addMonospaceFont(const void * data, uint32_t size, const uint32_t * codepages, size_t codepagesSize){
    font.init(data, size, codepages, codepagesSize, 1);
}

void Instance::ProcessEvents(){

    sf::Event event;

    memset(&updateSections, 0, sizeof(updateSections));

    instrumentsToUpdate.clear();

    while (window.pollEvent(event))
    {

        if (event.type == sf::Event::Closed)
            window.close();
        else if (event.type == sf::Event::Resized){
            
            //scale = std::max(static_cast<int>(std::ceil(event.size.height/(4*8*TILE_SIZE))), 1);
            
            updateSections.scale = 1;

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
                updateSections.tracker = 1;
            } else if (event.key.code == sf::Keyboard::Equal && event.key.control) {
                scale++;
                updateSections.scale = 1;
            } else if (event.key.code == sf::Keyboard::Hyphen && event.key.control && scale > 1) {
                scale--;
                updateSections.scale = 1;
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
                    updateSections.tracker_selection = 1;
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
        memset(&updateSections, true, sizeof(updateSections));
        instrumentsToUpdate.clear();
        forceUpdateAll = 0;
    }

    if (updateSections.inst_pos)
        renderInstList();

    if (updateSections.inst_pos || updateSections.scale)   
        updateInstPage();
        
    switch (lowerHalfMode) {
        case 0:
            if (updateSections.tracker || updateSections.scale){
                renderTracker();
                updateTrackerPos();
                renderBeatsTexture();
            }
            if (updateSections.tracker_selection)
                updateTrackerSelection();
            break;

        case 1:
            if (updateSections.scale)
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
                // auto start = std::chrono::high_resolution_clock::now();
            bezierTest.updatePosition(std::array<sf::Vector2f, 2> 
            {sf::Vector2f((float)selectionBounds[0] / scale, (float)selectionBounds[1] / scale - 8*TILE_SIZE), {100, 100}});
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
        updateSections.inst_pos = 1;
    } else
        instrumentsToUpdate.pop_back();
}

#endif // __INSTANCE_CPP_INCLUDED__
