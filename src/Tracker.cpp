#ifndef __TRACKER_INCLUDED__
#define __TRACKER_INCLUDED__

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <vector>
#include <array>
#include <functional>

#include "Tile.cpp"
#include "Effect.cpp"

#pragma region classDefinitions

class TrackerCell {
    public:
        TrackerCell();
        uint8_t noteValue;
        uint8_t instrument;
        std::vector<EffectBase> effects;

        bool attack () const {return ((flags & 1) != 0);};
        void attack (bool in) {flags = (flags & ~1) || (in ? 1 : 0);};

        bool hideInstrument () const {return ((flags & 2) != 0);};
        void hideInstrument (bool in) {flags = (flags & ~2) || (in ? 2 : 0);};

        TileMatrix render(uint16_t effectColumns = 0, bool singleTile = true);

        const bool operator==(const TrackerCell & other) const;
        const bool operator!=(const TrackerCell & other) const;

        static constexpr uint8_t MAX_NOTE    = 0x5F;
        static constexpr uint8_t KEY_OFF     = 0xFE;
        static constexpr uint8_t EMPTY_NOTE  = 0xFF;

        static constexpr uint8_t SPACE       = 0x20;    // space
        static constexpr uint8_t KEYOFF      = 0x23;    // #
        static constexpr uint8_t EMPTY       = 0x2E;    // .
        static constexpr uint8_t NOATTACK    = 0x1A;    // | but very left-aligned
        static constexpr uint8_t SHARP       = 0x23;    // #
        static constexpr uint8_t NOSHARP     = 0x2D;    // -
    private:
        uint8_t flags = 0;  // Value not undefined

        static constexpr uint32_t singleNoteTileTable[] {
            'C', 0x1B, 'D', 0x1C, 'E',              // C, C#, D, D#, E
            'F', 0x1D, 'G', 0x1E, 'A', 0x1F, 'B'    // F, F#, G, G#, A, A#, B
        };
        static constexpr uint32_t doubleNoteTileTable[] {
            'C', 'C', 'D', 'D', 'E',                // C, C#, D, D#, E
            'F', 'F', 'G', 'G', 'A', 'A', 'B',      // F, F#, G, G#, A, A#, B
            NOSHARP, SHARP, NOSHARP,  SHARP, NOSHARP,  
            NOSHARP, SHARP, NOSHARP,  SHARP, NOSHARP, SHARP, NOSHARP
        };
        static constexpr uint32_t emptyRow[] {
            EMPTY, EMPTY,  EMPTY,  SPACE,  EMPTY,  EMPTY   // ... ..
        };
        static constexpr uint32_t keyOffRow[] {
            KEYOFF, KEYOFF, KEYOFF, SPACE,  EMPTY,  EMPTY   // ### ..
        };
};

#pragma endregion
#pragma region implementation

TrackerCell::TrackerCell(){
    noteValue = EMPTY_NOTE;
    attack(true);
    hideInstrument(true);
    instrument = 0;
}

TileMatrix TrackerCell::render(uint16_t effectColumns, bool singleTile) {
    uint8_t tileAppend;
    uint8_t firstIndex;
    const uint32_t * noteTileTable;
    if (singleTile) { 
        tileAppend = 0; 
        firstIndex = 1; 
        noteTileTable = singleNoteTileTable;
    } else {
        tileAppend = 1; 
        firstIndex = 0; 
        noteTileTable = doubleNoteTileTable;
    }
    if (!effectColumns) effectColumns = 1;
    TileMatrix output(tileAppend+2+1+2+effectColumns*(3+1), 1, 0x20);
    // Render note
    if (noteValue == EMPTY_NOTE){
        const uint32_t * emptyRowPtr = emptyRow+firstIndex;
        output.copyRect(0, 0, tileAppend+2+1+2, 1, emptyRowPtr);
    } else if (noteValue == KEY_OFF){
        const uint32_t * keyOffRowPtr = keyOffRow+firstIndex;
        output.copyRect(0, 0, tileAppend+2+1+2, 1, keyOffRowPtr);
    } else {
        char row[5];
        std::array<uint32_t, 5> row32;
        if (hideInstrument()) {
            std::snprintf (row, 3, "%1d ", noteValue/12);
            for (int i = 0; i < 2; i++) row32[i] = row[i];
            row32[2] = EMPTY; row32[3] = EMPTY;
        } else {
            std::snprintf(row, 5, "%1d %02X", noteValue/12, instrument);
            for (int i = 0; i < 4; i++) row32[i] = row[i];
        }
        output.copyRect(tileAppend+1, 0, 2+1+2-1, 1, row32.data());
        output.setTile(0, 0, noteTileTable[noteValue%12]);
        if (!singleTile) output.setTile(1, 0, noteTileTable[12+noteValue%12]);
        output.setTile(tileAppend+2, 0, attack() ? SPACE : NOATTACK);
    }
    // Render effects
    {
        int i = 0;
        for (; i < effects.size() && i < effectColumns; i++)
            output.fillRect(tileAppend+2+1+2+1+i*(3+1), 0, 3, 1, 0x7F);
        if (effectColumns > effects.size()){
            for (; i < effectColumns; i++)
                output.fillRect(tileAppend+2+1+2+1+i*(3+1), 0, 3, 1, EMPTY);
        }
    }
    return output;
}

template<>
struct std::hash<TrackerCell> {
    size_t operator()(const TrackerCell & cell) const noexcept {
        char buffer[9];
        std::snprintf(buffer, 9, "%2X/%2X/%d%d", cell.noteValue, cell.instrument, cell.hideInstrument(), cell.attack());
        //TODO EFFECTS
        return std::hash<std::string>{}(std::string(buffer));
    }
    /*
    for (auto & effect : effects)
        std::snprintf("%X", effect.exportString)
    
    */


};

const bool TrackerCell::operator==(const TrackerCell & other) const {
    return (
        noteValue == other.noteValue &&
        instrument == other.instrument &&
        flags == other.flags &&
        effects == other.effects
    );
}

const bool TrackerCell::operator!=(const TrackerCell & other) const {
    return (
        noteValue != other.noteValue ||
        instrument != other.instrument ||
        flags != other.flags ||
        effects != other.effects
    );
}


#endif // __TRACKER_INCLUDED__