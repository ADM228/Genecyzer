#ifndef __INSTANCE_HEADER_INCLUDED__
#define __INSTANCE_HEADER_INCLUDED__

#include <SFML/Graphics.hpp>

#include <array>
#include <cstdint>
#include <vector>

#include "ChrFont.cpp"
#include "Tile.cpp"
#include "Project.cpp"
#include "ModularSynth.cpp"

#define INST_ENTRY_WIDTH 16
#define TILE_SIZE 8
#define INST_WIDTH  512
#define INST_HEIGHT 8

class Instance {

    public:
        Instance();
        void ProcessEvents();
        void Update();

        void addMonospaceFont(const void * data, uint32_t size, std::vector<uint32_t> codepages);
        void addMonospaceFont(const void * data, uint32_t size, const uint32_t * codepages, size_t codepagesSize);

        bool isWindowOpen(){ return window.isOpen(); };

    protected:
        void eventHandleInstList (int, int, uint8_t, bool);
        void renderInstList();
        void renderTracker();
        void updateInstPage();
        void updateTrackerPos();
        void updateTrackerSelection();
        void renderBeatsTexture();

    protected:

        static constexpr uint8_t ROW_SEPARATOR = 0x04;
        static constexpr uint8_t COL_SEPARATOR = 0x06;

        static constexpr uint8_t INTERSECTION_ALL4 = 0x05;
        static constexpr uint8_t INTERSECTION_NOUP = 0x07;
        static constexpr uint8_t INTERSECTION_NORT = 0x08;

        static constexpr uint8_t HEADER_HEIGHT = 5;

    private:
        uint32_t currentSong = 0;
        uint8_t instSelected = 0;
        uint8_t scale = 1;
        uint8_t upperHalfMode = 0;
        uint8_t lowerHalfMode = 0;

        bool singleTileTrackerRender = 1;

        int debug = 0;

        sf::Sprite instrumentSprite;

        #pragma region Update
        bool forceUpdateAll = 0;

        struct {
            bool scale;
            bool inst_pos;
            bool inst_list;
            bool tracker;
            bool tracker_selection;
        } updateSections;

        std::vector<uint8_t> instrumentsToUpdate;
        std::array<int, 4> selectionBounds;
        #pragma endregion

        sf::RenderWindow window;
        sf::View InstrumentView;
        sf::View TrackerView;

        ChrFont font;

        sf::Texture instrumentTexture;
        TileMatrix trackerMatrix;
        sf::Texture beatsTexture;

        Project activeProject;

        uint16_t mouseFlags = 0;

        sf::Event::MouseButtonEvent lastMousePress;

        ModSynthBezier bezierTest;
};

#endif  // __INSTANCE_HEADER_INCLUDED__