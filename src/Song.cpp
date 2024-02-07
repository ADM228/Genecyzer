#ifndef __SONG_INCLUDED__
#define __SONG_INCLUDED__

#include <vector>
#include <array>

#include "RIFF.cpp"
#include "Tracker.cpp"
#include "Instrument.cpp"

struct TrackerPattern {
    std::array<uint32_t, 8> cells;
    std::vector<uint16_t> beats_major;
    std::vector<uint16_t> beats_minor;
    size_t rows;
};

class Song {
    public:
        // Default song creation
        Song();
        // Create a song from file data
        Song(RIFF::RIFFReader & data, int & error);
        
        // The very cells
        std::vector<TrackerPattern> patterns;

        std::vector<std::vector<TrackerCell>> patternData;

        std::array<uint8_t, 8> effectColumnAmount; 

        std::vector<Instrument> localInstruments;
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

/*
    Genecyzer song format description:
    LIST chunk of type "song".
    Contains the following subchunks:
    "effc" - The amount of effect columns:
        8 bytes [08:15] - the amount of effect columns
        for the corresponding channel.
    "INAM" - Name. 
        Lists the song title.
    "col " - Color.
        Lists the color of the song in the standard
        24-bit color format.
    "LIST" chunk of type "pat " - a pattern (the 
    indexes are assumed to be in order). Subchunks:
        "idx " - The pattern indexes themselves,
        16-bit words, size is fixed at 8*2 = 16 bytes.
        "bmaj" - The major beats. Variable size.
        "bmin" - The minor beats. Variable size.
    "note" chunk - a chunk of a "note struct":
        4 bytes - the amount of note structs in 
        this chunk. Each note struct consists of:
            1 byte - the note value:
                In range 0..96 for C0..B7, 
                253 means to repeat the default 
                tracker cell (and no further data
                follows after this),
                254 means a KEY OFF/stop note,
                255 means an empty note cell.
            1 byte - the flags for the following
            data:
                bit 7 - whether the note has attack
                enabled,
                bit 6 - whether an instrument value
                is set,      
                bit 5 - whether a volume value
                is set,
                bit 4 - whether any effects are 
                declared,
                bit 3 - whether to set this cell as
                the default cell.
            1 byte (optional) - Instrument value
                Only present if bit 6 is set in the
                flags byte.
            1 byte (optional) - Volume value
                Only present if bit 5 is set in the
                flags byte.
            X bytes (optional) - Effect data
                TODO

*/

#endif  //__SONG_INCLUDED__