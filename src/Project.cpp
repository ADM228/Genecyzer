#ifndef __PROJECT_INCLUDED__
#define __PROJECT_INCLUDED__

#include <cstdint>
#include <fstream>
#include <vector>
#include <array>
#include "Tracker.cpp"

struct TrackerPattern {
    std::array<std::vector<TrackerCell>, 8> cells;
    std::vector<uint16_t> beats_major;
    std::vector<uint16_t> beats_minor;
};

class Project {
    public:
        // Create new project
        Project ();
        // Import project from file
        Project (std::ifstream file);
        // Import project from file
        Project (uint8_t * data);

        // Export project's patterns to SNESFM opcode format
        uint8_t * exportSNESFM ();

        // The very cells
        std::vector<TrackerPattern> patterns;

        std::array<uint8_t, 8> effectColumnAmount; 

};

Project::Project() {
    TrackerPattern defaultPattern {
        {},
        std::vector<uint16_t> {0, 16, 32, 48},
        std::vector<uint16_t> {8, 24, 40, 56}
    };
    defaultPattern.cells.fill(std::vector<TrackerCell>(64));
    patterns.push_back(defaultPattern);
    effectColumnAmount.fill(1);
}

#endif