#include "../Instance.hpp"

#include "../Song.cpp"
#include "../Instrument.cpp"

void Instance::renderTracker () {
    
    #define ROW_SEPARATOR 0x04
    #define COL_SEPARATOR 0x06

    #define INTERSECTION_ALL4 0x05
    #define INTERSECTION_NOUP 0x07
    #define INTERSECTION_NORT 0x08

    #define HEADER_HEIGHT 5

    #define TRACKER_ROW_WIDTH(effectColumns) trackerNoteWidth+1+2+(1+3)*effectColumns

    Song & activeSong = activeProject.songs[currentSong];
    uint8_t trackerNoteWidth = ((uint8_t)!singleTileTrackerRender)+2;
    size_t widthInTiles = std::ceil((window.getSize().x/scale)/TILE_SIZE);
    size_t heightInTiles = std::ceil((window.getSize().y/scale)/TILE_SIZE);
    size_t rows = activeSong.patterns[0].rows;
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

    for (auto & column : activeSong.effectColumnAmount) {
        widthOfTracker += TRACKER_ROW_WIDTH(column) + 1;
    }

    TileMatrix text = TileMatrix(widthOfTracker, std::min(heightInTiles, rows));
    int textHeight = text.getHeight();


    {
        std::vector<uint16_t> tracker_separator_columns(0);
        char rowNum[4];
        for (int i = 0; i < rows && i < textHeight; i++){
            std::snprintf(rowNum, 4, "%03X", i);
            auto rowNumMatrix = TextRenderer::render(std::string(rowNum), font, 3, 1, 0);
            text.copyRect(0, i, 3, 1, &rowNumMatrix, 0, 0);
        }
        
        int tileCounter = 4;
        for (int i = 0; i < 8; i++) {

            auto & patternData = activeSong.patternData[activeSong.patterns[0].cells[i]]; 

            for (int j = 0; j < rows && j < textHeight; j++) {
                auto row = patternData[j].render(activeSong.effectColumnAmount[i], singleTileTrackerRender);
                text.copyRect(tileCounter, j, TRACKER_ROW_WIDTH(activeSong.effectColumnAmount[i]), 1, &row, 0, 0);
            }

            tracker_separator_columns.push_back(tileCounter-1); 
            tileCounter += TRACKER_ROW_WIDTH(activeSong.effectColumnAmount[i]) + 1;
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


void Instance::updateTrackerPos () {
    TrackerView.reset(sf::FloatRect(0, 0, window.getSize().x, (double)window.getSize().y/scale));
    TrackerView.setViewport(sf::FloatRect(0.f, (double)(INST_HEIGHT*TILE_SIZE*scale)/(double)window.getSize().y, scale, 1));
}

void Instance::updateTrackerSelection () {
    int tileX = 3;
    uint8_t trackerNoteWidth = ((uint8_t)!singleTileTrackerRender)+2;
    auto & effectColumnAmount = activeProject.songs[currentSong].effectColumnAmount;

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
        } else if (beginX >= tileX+trackerNoteWidth+4 && beginX <= tileX+TRACKER_ROW_WIDTH(effectColumnAmount[i])) {
            // Begins at an effect tile
            for (int j = 0; j < std::max((int)effectColumnAmount[i], 1); j++) {
                if (beginX >= tileX+trackerNoteWidth+4+4*j && beginX <= tileX+trackerNoteWidth+4+4+4*j) { 
                    x1 = tileX+trackerNoteWidth+1+4+4*j;
                    break;
                }
            }
            break;
        }
        tileX += TRACKER_ROW_WIDTH(effectColumnAmount[i])+1;
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
        } else if (endX > tileX+trackerNoteWidth+4 && endX <= tileX+TRACKER_ROW_WIDTH(effectColumnAmount[i])+1) {
            // Ends at an effect tile
            for (int j = 0; j < std::max((int)effectColumnAmount[i], 1); j++) {
                if (endX > tileX+trackerNoteWidth+4+4*j && endX <= tileX+trackerNoteWidth+1+4+4+4*j) { 
                    x2 = tileX+trackerNoteWidth+4+4+4*j;
                    break;
                }
            }
            break;
        }
        tileX += TRACKER_ROW_WIDTH(effectColumnAmount[i])+1;
    }

    trackerMatrix.fillInvert(false);

    if (x1 == -1) x1 = beginX >= tileX ? tileX-3 : 4;
    if (x2 == -1) x2 = endX >= tileX ? tileX : x1;
    if (x1 >= x2 || y1 >= y2) return;
    x2 = std::min(x2, (int)trackerMatrix.getWidth());

    trackerMatrix.fillInvertRect(x1, y1, x2-x1, y2-y1, true);
}

void Instance::renderBeatsTexture() {
    auto & pattern = activeProject.songs[currentSong].patterns[0];
    int rows = std::min(pattern.rows, (size_t)trackerMatrix.getHeight()-5);
    if (!(trackerMatrix.getWidth() && rows)) return;
    auto & maj_beats = pattern.beats_major;
    auto & min_beats = pattern.beats_minor;
    auto colors = new uint8_t[rows]();
    auto pixels = new uint8_t[rows*trackerMatrix.getWidth()*TILE_SIZE/2*sizeof(sf::Color)](); // automatically zeroes out alpha value
    if (min_beats.size() > 0) {
        for (int i = 0; i < rows;) {
            for (int j = 0; j < min_beats.size() && i < rows; j++) {
                colors[i] = 1;
                i += min_beats[j];
            }
        }
    }
    if (maj_beats.size() > 0) {
        for (int i = 0; i < rows;) {
            for (int j = 0; j < maj_beats.size() && i < rows; j++) {
                colors[i] = 2;
                i += maj_beats[j];
            }
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
