#include "SFML/System/Vector2.hpp"
#pragma region header

#include <SFML/Graphics.hpp>
#include <vector>

#ifndef __TILE_INCLUDED__
#define __TILE_INCLUDED__

#define TILE_SIZE 8

#define HFLIP 0x01
#define VFLIP 0x02
#define FLIPMASK 0x03

#define REDMASK 0x10
#define GRNMASK 0x20
#define BLUMASK 0x40
#define PALMASK 0x70
#define INVMASK 0x80

struct Tile {
    uint32_t tileIndex;
    uint8_t flip_palette;
};

class TileMatrix : public sf::Drawable {
    public:
        TileMatrix() {};
        TileMatrix(uint16_t width, uint16_t height);
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

        #pragma endregion
        #pragma region rendering

        void setTexture(sf::Texture & texture) { _texture = &texture; };

        void setPosition(sf::Vector2f position) { _pos = position; };

        // Render TileMatrix to a sf::Texture
        sf::Texture renderToTexture(sf::Texture texture);

        #pragma endregion
        #pragma region getttingParams

        uint16_t getWidth (){ return _width; };
        uint16_t getHeight (){ return _height; };

              std::vector<Tile>& operator[](std::size_t idx)       { return _tiles[idx]; }
        const std::vector<Tile>& operator[](std::size_t idx) const { return _tiles[idx]; }

        #pragma endregion

    private:

        // Render TileMatrix to a sf::RenderWindow
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

        uint16_t _width, _height;
        std::vector<std::vector<Tile>> _tiles;
        sf::Texture * _texture;
        sf::Vector2f _pos {0, 0};

};

#pragma endregion

#include <iostream>

#ifdef BREAK_ON_EXCEPTIONS
#include <stdexcept>    
#define inv_arg(x) throw std::invalid_argument(x)
#else
uint32_t exception_count = 0;
#define inv_arg(x) fprintf(stderr, "[Tile.cpp #%08X]: ", exception_count++); fprintf(stderr, x); fprintf(stderr, "\n")
#endif

TileMatrix::TileMatrix(uint16_t width, uint16_t height){
    _tiles = std::vector<std::vector<Tile>>(height, std::vector<Tile>(width, {0, PALMASK}));
    _height = height;
    _width = width;
}

TileMatrix::TileMatrix(uint16_t width, uint16_t height, uint32_t fillTile){
    _tiles = std::vector<std::vector<Tile>>(height, std::vector<Tile>(width, {fillTile, PALMASK}));
    _height = height;
    _width = width;
}

void TileMatrix::resize(uint16_t width, uint16_t height, uint32_t fillTile){
    if (width != this->_width){
        for (uint16_t i = 0; i < this->_height; i++){
            _tiles[i].resize(width, Tile{fillTile, PALMASK});
        }
        this->_width = width;
    }
    if (height != this->_height){
        _tiles.resize(height, std::vector<Tile>(width, {fillTile, PALMASK}));
        this->_height = height;
    }
}

#pragma region tileSetting

void TileMatrix::setTile(uint16_t x, uint16_t y, uint32_t tile){
    if (y >= _tiles.size()) {inv_arg("[TileMatrix::setTile]: y is out of bounds"); return;}
    if (x >= _tiles[y].size()) {inv_arg("[TileMatrix::setTile]: x is out of bounds"); return;}
    _tiles[y][x].tileIndex = tile;
}

void TileMatrix::fill(uint32_t tile){
    for (uint16_t i = 0; i < _tiles.size(); i++) {
        for (uint16_t j = 0; j < _tiles[i].size(); j++)
            _tiles[i][j].tileIndex = tile;    
    }
}

void TileMatrix::fillRow(uint16_t row, uint32_t tile){
    if (row >= _tiles.size()) {inv_arg("[TileMatrix::fillRow]: row is out of bounds"); return;}
    for (uint16_t j = 0; j < _tiles[row].size(); j++)
        _tiles[row][j].tileIndex = tile; 
}

void TileMatrix::fillCol(uint16_t col, uint32_t tile){
    if (col >= _width) {inv_arg("[TileMatrix::fillCol]: col is out of bounds");}
        for (uint16_t i = 0; i < _tiles.size(); i++)
            if (col < _tiles[i].size()) _tiles[i][col].tileIndex = tile;   
}

void TileMatrix::fillRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t tile){
    if (x >= _width) {inv_arg("[TileMatrix::fillRect]: x is out of bounds"); return;}
    if (y >= _tiles.size()) {inv_arg("[TileMatrix::fillRect]: y is out of bounds"); return;}
    if (width+x > _width) {inv_arg("[TileMatrix::fillRect]: width+x is out of bounds");}
    if (height+y > _tiles.size()) {inv_arg("[TileMatrix::fillRect]: height+y is out of bounds");}
    for (uint16_t i = y; i < height+y && i < _tiles.size(); i++) {
        for (uint16_t j = x; j < width+x && j < _tiles[i].size(); j++) {
             _tiles[i][j].tileIndex = tile;
        }
    }
}

#pragma endregion
#pragma region flipSetting

void TileMatrix::setFlip(uint16_t x, uint16_t y, bool hFlip, bool vFlip){
    if (y >= _tiles.size()) {inv_arg("[TileMatrix::setFlip]: y is out of bounds"); return;}
    if (x >= _tiles[y].size()) {inv_arg("[TileMatrix::setFlip]: x is out of bounds"); return;}
    _tiles[y][x].flip_palette = (_tiles[y][x].flip_palette & ~FLIPMASK) | vFlip<<1|hFlip;
}

void TileMatrix::setFlipRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, bool hFlip, bool vFlip){
    if (x >= _width) {inv_arg("[TileMatrix::setFlipRect]: x is out of bounds"); return;}
    if (y >= _tiles.size()) {inv_arg("[TileMatrix::setFlipRect]: y is out of bounds"); return;}
    if (width+x > _width) {inv_arg("[TileMatrix::setFlipRect]: width+x is out of bounds");}
    if (height+y > _tiles.size()) {inv_arg("[TileMatrix::setFlipRect]: height+y is out of bounds");}
    for (uint16_t i = y; i < height+y && i < _tiles.size(); i++) {
        for (uint16_t j = x; j < width+x && j < _tiles[i].size(); j++) {
            _tiles[i][j].flip_palette = (_tiles[i][j].flip_palette & ~FLIPMASK) | vFlip<<1|hFlip;
        }
    }
}

#pragma endregion
#pragma region invSetting

void TileMatrix::setInvert(uint16_t x, uint16_t y, bool invert){
    if (y >= _tiles.size()) {inv_arg("[TileMatrix::setInvert]: y is out of bounds"); return;}
    if (x >= _tiles[y].size()) {inv_arg("[TileMatrix::setInvert]: x is out of bounds"); return;}
    _tiles[y][x].flip_palette = (_tiles[y][x].flip_palette & ~INVMASK) | invert << 7;
}

void TileMatrix::fillInvert(bool invert){
    for (uint16_t i = 0; i < _tiles.size(); i++) {
        for (uint16_t j = 0; j < _tiles[i].size(); j++)
            _tiles[i][j].flip_palette = (_tiles[i][j].flip_palette & ~INVMASK) | invert << 7;
    }
}


void TileMatrix::fillInvertRow(uint16_t row, bool invert){
    if (row >= _tiles.size()) {inv_arg("[TileMatrix::fillInvertRow]: row is out of bounds"); return;}
    for (uint16_t j = 0; j < _tiles[row].size(); j++)
        _tiles[row][j].flip_palette = (_tiles[row][j].flip_palette & ~INVMASK) | invert << 7;
}

void TileMatrix::fillInvertCol(uint16_t col, bool invert){
    if (col >= _width) {inv_arg("[TileMatrix::fillInvertCol]: col is out of bounds");}
    for (uint16_t i = 0; i < _tiles.size(); i++)
        if (col < _tiles[i].size()) _tiles[i][col].flip_palette = (_tiles[i][col].flip_palette & ~INVMASK) | invert << 7;
}

void TileMatrix::fillInvertRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, bool invert){
    if (x >= _width) {inv_arg("[TileMatrix::fillInvertRect]: x is out of bounds"); return;}
    if (y >= _tiles.size()) {inv_arg("[TileMatrix::fillInvertRect]: y is out of bounds"); return;}
    if (width+x > _width) {inv_arg("[TileMatrix::fillInvertRect]: width+x is out of bounds");}
    if (height+y > _tiles.size()) {inv_arg("[TileMatrix::fillInvertRect]: height+y is out of bounds");}
    for (uint16_t i = y; i < height+y && i < _tiles.size(); i++) {
        for (uint16_t j = x; j < width+x && j < _tiles[i].size(); j++)
            _tiles[i][j].flip_palette = (_tiles[i][j].flip_palette & ~INVMASK) | invert << 7;
    }
}

#pragma endregion
#pragma region paletteSetting

void TileMatrix::setPalette(uint16_t x, uint16_t y, uint8_t palette){
    if (y >= _tiles.size()) {inv_arg("[TileMatrix::setPalette]: y is out of bounds"); return;}
    if (x >= _tiles[y].size()) {inv_arg("[TileMatrix::setPalette]: x is out of bounds"); return;}
    _tiles[y][x].flip_palette = (_tiles[y][x].flip_palette & ~PALMASK) | ((palette << 4) & PALMASK);
}

void TileMatrix::fillPalette(uint8_t palette){
    for (uint16_t i = 0; i < _tiles.size(); i++) {
        for (uint16_t j = 0; j < _tiles[i].size(); j++)
            _tiles[i][j].flip_palette = (_tiles[i][j].flip_palette & ~PALMASK) | ((palette << 4) & PALMASK);
    }
}

void TileMatrix::fillPaletteRow(uint16_t row, uint8_t palette){
    if (row >= _tiles.size()) {inv_arg("[TileMatrix::fillPaletteRow]: row is out of bounds"); return;}
    for (uint16_t j = 0; j < _tiles[row].size(); j++)
        _tiles[row][j].flip_palette = (_tiles[row][j].flip_palette & ~PALMASK) | ((palette << 4) & PALMASK);
}

void TileMatrix::fillPaletteCol(uint16_t col, uint8_t palette){
    if (col >= _width) {inv_arg("[TileMatrix::fillPaletteCol]: col is out of bounds");}
    for (uint16_t i = 0; i < _tiles.size(); i++)
        if (col < _tiles[i].size()) _tiles[i][col].flip_palette = (_tiles[i][col].flip_palette & ~PALMASK) | ((palette << 4) & PALMASK);
}

void TileMatrix::fillPaletteRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t palette){
    if (x >= _width) {inv_arg("[TileMatrix::fillPaletteRect]: x is out of bounds"); return;}
    if (y >= _tiles.size()) {inv_arg("[TileMatrix::fillPaletteRect]: y is out of bounds"); return;}
    if (width+x > _width) {inv_arg("[TileMatrix::fillPaletteRect]: width+x is out of bounds");}
    if (height+y > _tiles.size()) {inv_arg("[TileMatrix::fillPaletteRect]: height+y is out of bounds");}
    for (uint16_t i = y; i < height+y && i < _tiles.size(); i++) {
        for (uint16_t j = x; j < width+x && j < _tiles[i].size(); j++)
            _tiles[i][j].flip_palette = (_tiles[i][j].flip_palette & ~PALMASK) | ((palette << 4) & PALMASK);
    }
}

#pragma endregion
#pragma region copying

void TileMatrix::copyRow(uint16_t row, uint32_t src[]){
    if (row >= _tiles.size()) {inv_arg("[TileMatrix::copyRow]: row is out of bounds"); return;}
    for (uint16_t i = 0; i < _tiles[row].size(); i++){_tiles[row][i].tileIndex = src[i];}
}

void TileMatrix::copyCol(uint16_t col, uint32_t src[]){
    if (col >= _width) {inv_arg("[TileMatrix::copyCol]: col is out of bounds");}
    for (uint16_t i = 0; i < _tiles.size(); i++){
        if (col < _tiles[i].size()) _tiles[i][col].tileIndex = src[i];
    }
}

void TileMatrix::copyRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t src[]){
    if (x >= _width) {inv_arg("[TileMatrix::copyRect]: x is out of bounds"); return;}
    if (y >= _tiles.size()) {inv_arg("[TileMatrix::copyRect]: y is out of bounds"); return;}
    if (width+x > _width) {inv_arg("[TileMatrix::copyRect]: width+x is out of bounds, tiles are gonna get shifted");}
    if (height+y > _tiles.size()) {inv_arg("[TileMatrix::copyRect]: height+y is out of bounds");}
    uint32_t ptr = 0;
    for (uint16_t i = y; i < height+y && i < _tiles.size(); i++){
        for (uint16_t j = x; j < width+x && j < _tiles[i].size(); j++){
            _tiles[i][j].tileIndex = src[ptr++];
        }
    }
}

void TileMatrix::copyRect(uint16_t out_x, uint16_t out_y, uint16_t width, uint16_t height, TileMatrix *src, uint16_t in_x, uint16_t in_y){

    #pragma region errorHandling
    if (in_x >= src->getWidth()) {inv_arg("[TileMatrix::copyRect]: x is out of bounds (source)"); return;}
    if (in_y >= src->_tiles.size()) {inv_arg("[TileMatrix::copyRect]: y is out of bounds (source)"); return;}
    if (width+in_x > src->getWidth()) {inv_arg("[TileMatrix::copyRect]: width+x is out of bounds (source)");}
    if (height+in_y > src->_tiles.size()) {inv_arg("[TileMatrix::copyRect]: height+y is out of bounds (source)");}

    if (out_x >= _width) {inv_arg("[TileMatrix::copyRect]: x is out of bounds (destination)"); return;}
    if (out_y >= _tiles.size()) {inv_arg("[TileMatrix::copyRect]: y is out of bounds (destination)"); return;}
    if (width+out_x > _width) {inv_arg("[TileMatrix::copyRect]: width+x is out of bounds (destination)");}
    if (height+out_y > _tiles.size()) {inv_arg("[TileMatrix::copyRect]: height+y is out of bounds (destination)");}
    #pragma endregion

    for (uint16_t i = 0; i < height && in_y+i < src->_tiles.size() && out_y+i < _tiles.size(); i++){
        for (uint16_t j = 0; j < width && in_x+j < src->getWidth() && out_x+j < _width; j++){
            _tiles[out_y+i][out_x+j] = src->_tiles[in_y+i][in_x+j];
        }
    }
}

#pragma endregion
#pragma region rendering

void TileMatrix::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    sf::Vector2f texturePos {0, 0};
    uint8_t flip_palette;
    sf::Color color;
    float x = _pos.x, y = _pos.y;
    if (_texture == nullptr) return;
    else states.texture = _texture;
    for (uint16_t i = 0; i < _tiles.size() && i*TILE_SIZE < target.getSize().y; i++){
        for (uint16_t j = 0; j < _tiles[i].size() && j*TILE_SIZE < target.getSize().x; j++){
            flip_palette = _tiles[i][j].flip_palette;
            texturePos = {
                static_cast<float>(flip_palette&INVMASK?TILE_SIZE:0),
                static_cast<float>((_tiles[i][j].tileIndex) << 3)
            };
            color = sf::Color(
                flip_palette&REDMASK?255:0,
                flip_palette&GRNMASK?255:0,
                flip_palette&BLUMASK?255:0);
            sf::Vertex vertices[4] = {
                sf::Vertex(sf::Vector2f(x+j*TILE_SIZE,            y+i*TILE_SIZE),
                    color, texturePos+sf::Vector2f(
                        flip_palette&HFLIP?TILE_SIZE:0,
                        flip_palette&VFLIP?TILE_SIZE:0)
                    ),
                sf::Vertex(sf::Vector2f(x+j*TILE_SIZE+TILE_SIZE,  y+i*TILE_SIZE),
                    color, texturePos+sf::Vector2f(
                        flip_palette&HFLIP?0:TILE_SIZE,
                        flip_palette&VFLIP?TILE_SIZE:0)
                    ),
                sf::Vertex(sf::Vector2f(x+j*TILE_SIZE+TILE_SIZE,  y+i*TILE_SIZE+TILE_SIZE),
                    color, texturePos+sf::Vector2f(
                        flip_palette&HFLIP?0:TILE_SIZE,
                        flip_palette&VFLIP?0:TILE_SIZE)
                    ),
                sf::Vertex(sf::Vector2f(x+j*TILE_SIZE,            y+i*TILE_SIZE+TILE_SIZE),
                    color, texturePos+sf::Vector2f(
                        flip_palette&HFLIP?TILE_SIZE:0,
                        flip_palette&VFLIP?0:TILE_SIZE)
                    )
            };

            target.draw(vertices, 4, sf::TriangleFan, states);
        }
    }
}

sf::Texture TileMatrix::renderToTexture(sf::Texture texture){
    sf::Vector2f texturePos {0, 0};
    uint8_t flip_palette;
    sf::Color color;
    sf::RenderTexture target;
    target.create(_width*TILE_SIZE, _height*TILE_SIZE);
    for (uint16_t i = 0; i < _tiles.size(); i++){
        uint16_t y = _height - i - 1;
        for (uint16_t j = 0; j < _tiles[i].size(); j++){
            flip_palette = _tiles[i][j].flip_palette;
            texturePos = {
                static_cast<float>(flip_palette&INVMASK?TILE_SIZE:0),
                static_cast<float>((_tiles[i][j].tileIndex) << 3)
            };
            color = sf::Color(
                flip_palette&REDMASK?255:0,
                flip_palette&GRNMASK?255:0,
                flip_palette&BLUMASK?255:0);
            sf::Vertex vertices[4] = {
                sf::Vertex(sf::Vector2f(j*TILE_SIZE,            y*TILE_SIZE+TILE_SIZE),
                    color, texturePos+sf::Vector2f(
                        flip_palette&HFLIP?TILE_SIZE:0,
                        flip_palette&VFLIP?TILE_SIZE:0)
                    ),
                sf::Vertex(sf::Vector2f(j*TILE_SIZE+TILE_SIZE,  y*TILE_SIZE+TILE_SIZE),
                    color, texturePos+sf::Vector2f(
                        flip_palette&HFLIP?0:TILE_SIZE,
                        flip_palette&VFLIP?TILE_SIZE:0)
                    ),
                sf::Vertex(sf::Vector2f(j*TILE_SIZE+TILE_SIZE,  y*TILE_SIZE),
                    color, texturePos+sf::Vector2f(
                        flip_palette&HFLIP?0:TILE_SIZE,
                        flip_palette&VFLIP?0:TILE_SIZE)
                    ),
                sf::Vertex(sf::Vector2f(j*TILE_SIZE,            y*TILE_SIZE),
                    color, texturePos+sf::Vector2f(
                        flip_palette&HFLIP?TILE_SIZE:0,
                        flip_palette&VFLIP?0:TILE_SIZE))
            };
            target.draw(vertices, 4, sf::TriangleFan, sf::RenderStates(&texture));
        }
    }
    return target.getTexture();
}

#pragma endregion

#endif  // __TILE_INCLUDED__