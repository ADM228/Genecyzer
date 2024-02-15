#ifndef __SONG_INCLUDED__
#define __SONG_INCLUDED__

#include <vector>
#include <array>

#include "Tracker.cpp"
#include "Instrument.cpp"

struct TrackerPattern {
    std::array<uint16_t, 8> cells;
    std::vector<uint16_t> beats_major;
    std::vector<uint16_t> beats_minor;
    size_t rows;
};

class Song {
    public:
        // Default song creation
        static Song createDefault();
        
        // The very cells
        std::vector<TrackerPattern> patterns;

        std::vector<std::vector<TrackerCell>> patternData;

        std::array<uint8_t, 8> effectColumnAmount; 

        std::vector<Instrument> localInstruments;
};

Song Song::createDefault() {
    Song output;
    TrackerPattern defaultPattern {
        {0, 0, 0, 0, 0, 0, 0, 0},
        std::vector<uint16_t> {16},
        std::vector<uint16_t> {4},
        64
    };
    output.patterns.push_back(defaultPattern);
    output.patternData.push_back(std::vector<TrackerCell>(64));
    output.effectColumnAmount.fill(2);
    return output;
}



#endif  //__SONG_INCLUDED__