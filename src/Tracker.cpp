#ifndef __TRACKER_INCLUDED__
#define __TRACKER_INCLUDED__

#include <vector>
#include "Tile.cpp"
#include "Effect.cpp"

#pragma region classDefinitions

#define MAX_NOTE    0x5F
#define KEY_OFF     0xFE
#define EMPTY_NOTE  0xFF

class TrackerCell {
    public:
        TrackerCell();
        uint8_t noteValue;
        uint8_t instrument;
        bool attack;
        std::vector<EffectBase> effects;

        TileMatrix render();
    private:
        constexpr static uint32_t noteTileTable[] {
            0x43, 0x1B, 0x44, 0x1C, 0x45,               // C, C#, D, D#, E
            0x46, 0x1D, 0x47, 0x1E, 0x41, 0x1F, 0x42    // F, F#, G, G#, A, A#, B
        };
        constexpr static uint32_t emptyRow[] {
            0x2E, 0x2E, 0x20, 0x2E, 0x2E    // .. ..
        };
        constexpr static uint32_t keyOffRow[] {
            0x23, 0x23, 0x20, 0x2E, 0x2E    // ## ..
        };
};

#pragma endregion
#pragma region implementation

TrackerCell::TrackerCell(){
    noteValue = EMPTY_NOTE;
    attack = true;
    instrument = 0;
}

TileMatrix TrackerCell::render() {
    TileMatrix output(2+1+2+1+2, 1, 0x20);
    // Render note
    if (noteValue == EMPTY_NOTE){
        uint32_t * emptyRowPtr = const_cast<uint32_t *>(emptyRow);
        output.copyRect(0, 0, 2+1+2, 1, emptyRowPtr);
    } else if (noteValue == KEY_OFF){
        uint32_t * keyOffRowPtr = const_cast<uint32_t *>(keyOffRow);
        output.copyRect(0, 0, 2+1+2, 1, keyOffRowPtr);
    } else {
        char row[5];
        std::snprintf(row, 5, "%1d %02X", noteValue/12, instrument);
        std::vector<uint32_t> row32(0);
        for (int i = 0; i < 4; i++) row32.push_back(row[i]);
        output.copyRect(1, 0, 2+1+2-1, 1, row32.data());
        output.setTile(0, 0, noteTileTable[noteValue%12]);
        output.setTile(2, 0, attack ? 0x20 : 0x1A);
    }
    return output;
}



#endif // __TRACKER_INCLUDED__