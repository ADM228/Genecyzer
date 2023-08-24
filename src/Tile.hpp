#include <SFML/Graphics.hpp>
#include <vector>

#ifndef __TILE_INCLUDED__
#define __TILE_INCLUDED__

class Tile {
    public:
        Tile(uint32_t x, uint32_t y);
        Tile(uint32_t x, uint32_t y, uint32_t tile);
        Tile(uint32_t x, uint32_t y, uint32_t tile, bool hFlip, bool vFlip);
        void setTile(uint32_t tile);
        void setFlip(bool hFlip, bool vFlip);
        bool getFlip() {return new bool[2] {_hFlip, _vFlip}; };
        sf::VertexArray renderVertex;
    private:
        void updateRenderVertex();
        sf::Vector2f pos;
        sf::Vector2f texturePos;
        bool _hFlip = false;
        bool _vFlip = false;
};

constexpr uint8_t HFLIP = 0x01;
constexpr uint8_t VFLIP = 0x02;
constexpr uint8_t FLIPMASK = 0x03;

constexpr uint8_t REDMASK = 0x10;
constexpr uint8_t GRNMASK = 0x20;
constexpr uint8_t BLUMASK = 0x40;
constexpr uint8_t PALMASK = 0x70;
constexpr uint8_t INVMASK = 0x80;

class TileRow {
    public:
        TileRow() {};
        TileRow(uint16_t length);
        TileRow(uint16_t length, uint32_t src[]);
        TileRow(uint16_t length, uint32_t fillTile);

        void setTile(uint16_t offset, uint32_t tile) {_tiles[offset] = tile;};
        void fillTile(uint32_t tile) {_tiles.assign(_tiles.size(), tile);};
        void fillTile(uint16_t offset, uint16_t length, uint32_t tile) {_tiles.assign(length, tile);};

        void setFlip(uint16_t offset, bool hFlip, bool vFlip);
        void fillFlip(uint16_t offset, uint16_t length, bool hFlip, bool vFlip);

        void setPalette(uint16_t offset, uint8_t palette);
        void fillPalette(uint8_t palette);
        void fillPalette(uint16_t offset, uint16_t length, uint8_t palette);

        void setInvert(uint16_t offset, bool invert);
        void fillInvert(bool invert);
        void fillInvert(uint16_t offset, uint16_t length, bool invert);

        void copy(uint32_t src[]);
        void copy(uint16_t offset, uint16_t length, uint32_t src[]);

        std::vector <uint32_t> _tiles;
        std::vector <uint8_t> _flip_palette;

};

class TileMatrix {
    public:
        TileMatrix(uint16_t width, uint16_t height);
        TileMatrix(uint16_t width, uint16_t height, TileRow tiles[]);
        TileMatrix(uint16_t width, uint16_t height, uint32_t fillTile);

        void resize(uint16_t width, uint16_t height, uint32_t fillTile = 0x20);

        #pragma region tileSetting

        // Places the tile at coordinates, throws an error if the coordinates are out of bounds
        void setTile(uint16_t x, uint16_t y, uint32_t tile);

        // Fills the entire tile matrix with the tile
        void fill(uint32_t tile);

        // Fills the row (all tiles at Y coordinate) with the tile, throws an error if the Y coordinate is out of bounds
        void fillRow(uint16_t row, uint32_t tile);

        // Fills the column (all tiles at X coordinate) with the tile, throws an error if the X coordinate is out of bounds
        void fillCol(uint16_t col, uint32_t tile);

        // Fills a rectangle with the tile, throws an error if the coordinates are out of bounds
        void fillRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t tile);

        #pragma endregion
        #pragma region flipSetting

        // Sets the flipping parameters of a tile
        void setFlip(uint16_t x, uint16_t y, bool hFlip, bool vFlip);

        // Sets the flipping parameters of a rectangle
        void setFlipRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, bool hFlip, bool vFlip);

        #pragma endregion
        #pragma region paletteSetting

        // Sets the palette of the tile at coordinates, throws an error if the coordinates are out of bounds
        void setPalette(uint16_t x, uint16_t y, uint8_t palette);

        // Sets the palette of the entire tile matrix
        void fillPalette(uint8_t palette);

        // Sets the palette of the row (all tiles at Y coordinate), throws an error if the Y coordinate is out of bounds
        void fillPaletteRow(uint16_t row, uint8_t palette);

        // Sets the palette of the row (all tiles at X coordinate), throws an error if the X coordinate is out of bounds
        void fillPaletteCol(uint16_t col, uint8_t palette);

        // Sets the palette of a rectangle, throws an error if the coordinates are out of bounds
        void fillPaletteRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t palette);

        #pragma endregion
        #pragma region invSetting

        // Sets the inversion of the tile at coordinates, throws an error if the coordinates are out of bounds
        void setInvert(uint16_t x, uint16_t y, bool invert);

        // Sets the inversion of the entire tile matrix
        void fillInvert(bool invert);

        // Sets the inversion of the row (all tiles at Y coordinate), throws an error if the Y coordinate is out of bounds
        void fillInvertRow(uint16_t row, bool invert);

        // Sets the inversion of the row (all tiles at X coordinate), throws an error if the X coordinate is out of bounds
        void fillInvertCol(uint16_t col, bool invert);

        // Sets the inversion of a rectangle, throws an error if the coordinates are out of bounds
        void fillInvertRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, bool invert);

        #pragma endregion
        #pragma region copying

        // Copies row from input array into the specified row (tiles with the Y coordinate), throws an error if the Y coordinate is out of bounds
        void copyRow(uint16_t row, uint32_t src[]);

        // Copies column from input array into the specified row (tiles with the X coordinate), throws an error if the X coordinate is out of bounds
        void copyCol(uint16_t col, uint32_t src[]);

        // Copies a rectangle from input one-dimensional array, from left to right, from top to bottom; throws an error if the coordinates are out of bounds
        void copyRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t src[]);

        // Copies a rectangle from another TileMatrix, throws an error if the coordinates are out of bounds
        void copyRect(uint16_t out_x, uint16_t out_y, uint16_t width, uint16_t height, TileMatrix *src, uint16_t in_x, uint16_t in_y);

        // Unimplemented
        void copyRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, TileRow src[]);

        #pragma endregion
        #pragma region rendering

        // Render TileMatrix to a sf::RenderWindow
        void render(uint16_t x, uint16_t y, sf::RenderWindow *window, sf::Texture texture);
        // Render TileMatrix to a sf::Texture
        sf::Texture renderToTexture(sf::Texture texture);

        #pragma endregion
        #pragma region getttingParams

        uint16_t getWidth (){ return _width; };
        uint16_t getHeight (){ return _height; };

        #pragma endregion

    private:

        uint16_t _width, _height;
        std::vector<TileRow> _tiles;

};

#endif  // __TILE_INCLUDED__
