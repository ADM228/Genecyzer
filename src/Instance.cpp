#ifndef __INSTANCE_CPP_INCLUDED__
#define __INSTANCE_CPP_INCLUDED__

#include <algorithm>
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
    // Init all variables
    selectionBounds.fill(-1);
    forceUpdateAll = 1;
    lastMousePress.position.x = 0;
    lastMousePress.position.y = 0;

    // Init graphics
    window.create(sf::VideoMode({200, 200}), "Genecyzer");
    window.setFramerateLimit(60);
    InstrumentView = sf::View(sf::FloatRect({0.f, 0.f}, {200.f, 200.f}));
    TrackerView = sf::View(sf::FloatRect({0.f, 0.f}, {200.f, 200.f}));

    if (openFileIntoProject()) saveProjectToFile();
    else activeProject = Project::createDefault();
}

void Instance::addMonospaceFont(const void * data, uint32_t size, std::vector<uint32_t> codepages){
    font.init(data, size, codepages, 1);
}

void Instance::addMonospaceFont(const void * data, uint32_t size, const uint32_t * codepages, size_t codepagesSize){
    font.init(data, size, codepages, codepagesSize, 1);
}

void Instance::ProcessEvents(){

    memset(&updateSections, 0, sizeof(updateSections));

    instrumentsToUpdate.clear();

    while (const std::optional event = window.pollEvent())
    {

        if (event->is<sf::Event::Closed>())
            window.close();
        else if (event->is<sf::Event::Resized>()){
            
            //scale = std::max(static_cast<int>(std::ceil(event.size.height/(4*8*TILE_SIZE))), 1);
            
            updateSections.scale = 1;

            //int width = std::ceil((event.size.width/scale)/TILE_SIZE);


        } else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()){
            
            if (keyPressed->scancode == sf::Keyboard::Scancode::Down)
                eventHandleInstList (255, +1, 255, false);
            else if (keyPressed->scancode == sf::Keyboard::Scancode::Up)
                eventHandleInstList (0, -1, 0, true);
            else if (keyPressed->scancode == sf::Keyboard::Scancode::Right)
                eventHandleInstList (256-8, +8, 255, false);
            else if (keyPressed->scancode == sf::Keyboard::Scancode::Left)
                eventHandleInstList (7, -8, 0, true);
            else if (keyPressed->scancode == sf::Keyboard::Scancode::E){
                singleTileTrackerRender ^= 1;
                updateSections.tracker = 1;
            } else if (keyPressed->scancode == sf::Keyboard::Scancode::Equal && keyPressed->control) {
                scale++;
                updateSections.scale = 1;
            } else if (keyPressed->scancode == sf::Keyboard::Scancode::Hyphen && keyPressed->control && scale > 1) {
                scale--;
                updateSections.scale = 1;
            } else if (keyPressed->scancode == sf::Keyboard::Scancode::Apostrophe) {
                lowerHalfMode ^= 1;
                forceUpdateAll = 1;
            }
        } else if (const auto* mouseEvent = event->getIf<sf::Event::MouseButtonPressed>()) {
            lastMousePress = *mouseEvent;
            // do sumn for time
            if (mouseEvent->button == sf::Mouse::Button::Left) mouseFlags |= MOUSE_DOWN;
        } else if ( const auto* mouseMoveEvent = event->getIf<sf::Event::MouseMoved>()) {
            //  || (const auto* touchMoveEvent = event->getIf<sf::Event::TouchMoved>())
            if (mouseFlags & MOUSE_DOWN) {
                // determine region
                if (
                    lowerHalfMode == 0 &&
                    lastMousePress.position.y > scale*TILE_SIZE*(8+5) &&
                    lastMousePress.position.x > scale*TILE_SIZE*(3+1)
                ) {
                    selectionBounds[0] = lastMousePress.position.x  / (scale*TILE_SIZE);
                    selectionBounds[1] = lastMousePress.position.y  / (scale*TILE_SIZE);
                    selectionBounds[2] = mouseMoveEvent->position.x / (scale*TILE_SIZE);
                    selectionBounds[3] = mouseMoveEvent->position.y / (scale*TILE_SIZE);
                    updateSections.tracker_selection = 1;
                } else if (
                    lowerHalfMode == 1 &&
                    lastMousePress.position.y > scale*TILE_SIZE*8
                ) {
                    selectionBounds[0] = mouseMoveEvent->position.x;
                    selectionBounds[1] = mouseMoveEvent->position.y;
                }
            }
        } else if (const auto* mouseEvent = event->getIf<sf::Event::MouseButtonReleased>()) {
            if (mouseEvent->button == sf::Mouse::Button::Left) mouseFlags &= ~MOUSE_DOWN;
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
            beats.setPosition({0, 5*TILE_SIZE});
            beats.setTexture(&beatsTexture);
            beats.setScale({2, TILE_SIZE});
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

#pragma region fileOps

bool Instance::openFileIntoProject () {
    auto filenamePtr = tinyfd_openFileDialog("Open a Genecyzer project file", NULL, 1, filter, "Genecyzer project file", 0);
    if (filenamePtr == NULL) {
        return false;
    }
    std::string filename(filenamePtr);
    auto data = std::ifstream(filename, std::ios_base::binary | std::ios_base::in);
    auto file = RIFF::RIFFReader();
    file.open(data);
    RIFFLoader::loadRIFFFile(file, activeProject);

    return true;
}

bool Instance::saveProjectToFile () {
    auto outFilenamePtr = tinyfd_saveFileDialog("Save the file", "outTest.gczr", 1, filter, "Genecyzer project file");
    if (outFilenamePtr == NULL) {
        return false;
    }

    std::string outFilename(outFilenamePtr);
    auto outData = std::ofstream(outFilename, std::ios_base::out | std::ios_base::binary);
    RIFF::RIFFWriter writer;

    writer.open(outData);

    // saveInternal(writer);
    RIFFLoader::saveRIFFFile(writer, activeProject);
    writer.close();
    outData.close();

    return true;
}

#pragma endregion

#endif // __INSTANCE_CPP_INCLUDED__
