#ifndef __SONG_INCLUDED__
#define __SONG_INCLUDED__

#include <vector>
#include <array>

#include "Tracker.cpp"
#include "Instrument.cpp"
#include "TextRenderer.cpp"

struct TrackerPattern {
    std::array<uint32_t, 8> cells;
    std::vector<uint16_t> beats_major;
    std::vector<uint16_t> beats_minor;
    size_t rows;
};

class Song {
    public:
        Song();
        
        // The very cells
        std::vector<TrackerPattern> patterns;

        std::vector<std::vector<TrackerCell>> patternData;

        std::array<uint8_t, 8> effectColumnAmount; 
};

Song::Song() {
        TrackerPattern defaultPattern {
        {0, 0, 0, 0, 0, 0, 0, 0},
        std::vector<uint16_t> {16},
        std::vector<uint16_t> {4},
        64
    };
    patterns.push_back(defaultPattern);
    patternData.push_back(std::vector<TrackerCell>(64));
    effectColumnAmount.fill(2);
}

#endif  //__SONG_INCLUDED__