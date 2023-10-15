#pragma region header

#include <SFML/Graphics.hpp>
#include <vector>

#ifndef __TILE_INCLUDED__
#define __TILE_INCLUDED__

#define TILE_SIZE 8

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

#define HFLIP 0x01
#define VFLIP 0x02
#define FLIPMASK 0x03

#define REDMASK 0x10
#define GRNMASK 0x20
#define BLUMASK 0x40
#define PALMASK 0x70
#define INVMASK 0x80

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

              uint32_t& operator[](std::size_t idx)       { return _tiles[idx]; }
        const uint32_t& operator[](std::size_t idx) const { return _tiles[idx]; }

        std::vector <uint32_t> _tiles;
        std::vector <uint8_t> _flip_palette;

};

class TileMatrix {
    public:
        TileMatrix() {};
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

              TileRow& operator[](std::size_t idx)       { return _tiles[idx]; }
        const TileRow& operator[](std::size_t idx) const { return _tiles[idx]; }

        #pragma endregion

    private:

        uint16_t _width, _height;
        std::vector<TileRow> _tiles;

};

#pragma endregion

#include <iostream>

#ifdef BREAK_ON_EXCEPTIONS
#include <stdexcept>    
#define inv_arg(x) throw std::invalid_argument(x)
#else
uint32_t exception_count = 0;
#define inv_arg(x) fprintf(stderr, "[Tile.cpp #%08X]: ", exception_count++); fprintf(stderr, x); fprintf(stderr, "\n"); return
#endif

Tile::Tile(uint32_t x, uint32_t y){
    pos.x = x * TILE_SIZE;
    pos.y = y * TILE_SIZE;
    texturePos = sf::Vector2f{0, 0};
    updateRenderVertex();
}

Tile::Tile(uint32_t x, uint32_t y, uint32_t tile){
    pos.x = x * TILE_SIZE;
    pos.y = y * TILE_SIZE;
    texturePos = sf::Vector2f{0, static_cast<float>(tile << 3)};
    updateRenderVertex();
}

Tile::Tile(uint32_t x, uint32_t y, uint32_t tile, bool hFlip, bool vFlip){
    pos.x = x * TILE_SIZE;
    pos.y = y * TILE_SIZE;
    texturePos = sf::Vector2f{0, static_cast<float>(tile << 3)};
    _hFlip = hFlip;
    _vFlip = vFlip;
    updateRenderVertex();
}

void Tile::setFlip(bool hFlip, bool vFlip){
    _hFlip = hFlip;
    _vFlip = vFlip;
    updateRenderVertex();
}

void Tile::setTile(uint32_t tile){
    texturePos = sf::Vector2f{0, static_cast<float>(tile << 3)};
    updateRenderVertex();
}

void Tile::updateRenderVertex(){
    renderVertex = sf::VertexArray(sf::TriangleFan);
    renderVertex.append(sf::Vertex(pos, texturePos+sf::Vector2f(_hFlip?TILE_SIZE:0,_vFlip?TILE_SIZE:0)));
    renderVertex.append(sf::Vertex(pos+sf::Vector2f(TILE_SIZE,        0), texturePos+sf::Vector2f(_hFlip?0:TILE_SIZE,_vFlip?TILE_SIZE:0)));
    renderVertex.append(sf::Vertex(pos+sf::Vector2f(TILE_SIZE,TILE_SIZE), texturePos+sf::Vector2f(_hFlip?0:TILE_SIZE,_vFlip?0:TILE_SIZE)));
    renderVertex.append(sf::Vertex(pos+sf::Vector2f(0        ,TILE_SIZE), texturePos+sf::Vector2f(_hFlip?TILE_SIZE:0,_vFlip?0:TILE_SIZE)));
}

// Tile row shit

TileRow::TileRow(uint16_t length){
    _tiles = std::vector<uint32_t>(length);
    _tiles.assign(length, 0);
    _flip_palette = std::vector<uint8_t>(length);
    _flip_palette.assign(length, PALMASK);
}

TileRow::TileRow(uint16_t length, uint32_t fillTile){
    _tiles = std::vector<uint32_t>(length);
    _tiles.assign(length, fillTile);
    _flip_palette = std::vector<uint8_t>(length);
    _flip_palette.assign(length, PALMASK);
}

TileRow::TileRow(uint16_t length, uint32_t src[]){
    _tiles = std::vector<uint32_t>(length);
    for (uint16_t i = 0; i < _tiles.size(); i++){_tiles[i] = src[i];}
    _flip_palette = std::vector<uint8_t>(length);
    _flip_palette.assign(length, PALMASK);
}

void TileRow::setFlip(uint16_t offset, bool hFlip, bool vFlip){
    _flip_palette[offset] = (_flip_palette[offset] & ~FLIPMASK) | vFlip << 1 | hFlip;
}

void TileRow::fillFlip(uint16_t offset, uint16_t length, bool hFlip, bool vFlip){
    for (uint16_t i = offset; i < offset+length; i++)
        _flip_palette[i] = (_flip_palette[i] & ~FLIPMASK) | vFlip << 1 | hFlip;
}

void TileRow::setPalette(uint16_t offset, uint8_t palette){
    _flip_palette[offset] = (_flip_palette[offset] & ~PALMASK) | ((palette << 4) & PALMASK);
}

void TileRow::fillPalette(uint8_t palette){
    for (uint16_t i = 0; i < _flip_palette.size(); i++)
        _flip_palette[i] = (_flip_palette[i] & ~PALMASK) | ((palette << 4) & PALMASK);
}

void TileRow::fillPalette(uint16_t offset, uint16_t length, uint8_t palette){
    for (uint16_t i = offset; i < offset+length; i++)
        _flip_palette[i] = (_flip_palette[i] & ~PALMASK) | ((palette << 4) & PALMASK);
}

void TileRow::setInvert(uint16_t offset, bool invert){
    _flip_palette[offset] = (_flip_palette[offset] & ~INVMASK) | invert << 7;
}

void TileRow::fillInvert(bool invert){
    for (uint16_t i = 0; i < _flip_palette.size(); i++)
        _flip_palette[i] = (_flip_palette[i] & ~INVMASK) | invert << 7;
}

void TileRow::fillInvert(uint16_t offset, uint16_t length, bool invert){
    for (uint16_t i = offset; i < offset+length; i++)
        _flip_palette[i] = (_flip_palette[i] & ~INVMASK) | invert << 7;
}

void TileRow::copy (uint32_t src[]){
    for (uint16_t i = 0; i < _tiles.size(); i++){_tiles[i] = src[i];}
}

void TileRow::copy (uint16_t offset, uint16_t length, uint32_t src[]){
    if (offset >= _tiles.size()) {inv_arg("[TileRow::copy]: offset is out of bounds");}
    if (offset+length > _tiles.size()) {inv_arg("[TileRow::copy]: offset+length is out of bounds");}
    for (uint16_t i = offset; i < offset+length; i++){_tiles[i] = src[i];}
}

TileMatrix::TileMatrix(uint16_t width, uint16_t height){
    _tiles = std::vector<TileRow>(height);
    _tiles.assign(height, TileRow(width));
    _height = height;
    _width = width;
}

TileMatrix::TileMatrix(uint16_t width, uint16_t height, TileRow tiles[]){
    _tiles = std::vector<TileRow>(height);
    for (uint16_t i = 0; i < height; i++) {_tiles[i] = tiles[i];}
    _height = height;
    _width = width;
}

TileMatrix::TileMatrix(uint16_t width, uint16_t height, uint32_t fillTile){
    _tiles = std::vector<TileRow>(height);
    _tiles.assign(height, TileRow(width, fillTile));
    _height = height;
    _width = width;
}

void TileMatrix::resize(uint16_t width, uint16_t height, uint32_t fillTile){
    if (width != this->_width){
        for (uint16_t i = 0; i < this->_height; i++){
            _tiles[i]._tiles.resize(width, fillTile);
            _tiles[i]._flip_palette.resize(width, PALMASK);
        }
        this->_width = width;
    }
    if (height != this->_height){
        _tiles.resize(height, TileRow(this->_width, fillTile));
        this->_height = height;
    }
}

#pragma region tileSetting

void TileMatrix::setTile(uint16_t x, uint16_t y, uint32_t tile){
    if (x >= _width) {inv_arg("[TileMatrix::setTile]: x is out of bounds");}
    if (y >= _height) {inv_arg("[TileMatrix::setTile]: y is out of bounds");}
    _tiles[y]._tiles[x] = tile;
}

void TileMatrix::fill(uint32_t tile){
    for (uint16_t i = 0; i < _height; i++) {_tiles[i]._tiles.assign(_width, tile);}
}

void TileMatrix::fillRow(uint16_t row, uint32_t tile){
    if (row >= _height) {inv_arg("[TileMatrix::fillRow]: row is out of bounds");}
    _tiles[row]._tiles.assign(_width, tile);
}

void TileMatrix::fillCol(uint16_t col, uint32_t tile){
    if (col >= _width) {inv_arg("[TileMatrix::fillCol]: col is out of bounds");}
    for (uint16_t i = 0; i < _height; i++) {_tiles[i]._tiles[col] = tile;}
}

void TileMatrix::fillRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t tile){
    if (x >= _width) {inv_arg("[TileMatrix::fillRect]: x is out of bounds");}
    if (y >= _height) {inv_arg("[TileMatrix::fillRect]: y is out of bounds");}
    if (width+x > _width) {inv_arg("[TileMatrix::fillRect]: width+x is out of bounds");}
    if (height+y > _height) {inv_arg("[TileMatrix::fillRect]: height+y is out of bounds");}
    for (uint16_t i = y; i < height+y; i++) {
        for (uint16_t j = x; j < width+x; j++) {_tiles[i]._tiles[j] = tile;}
    }
}

#pragma endregion
#pragma region flipSetting

void TileMatrix::setFlip(uint16_t x, uint16_t y, bool hFlip, bool vFlip){
    if (x >= _width) {inv_arg("[TileMatrix::setFlip]: x is out of bounds");}
    if (y >= _height) {inv_arg("[TileMatrix::setFlip]: y is out of bounds");}
    _tiles[y]._flip_palette[x] = (_tiles[y]._flip_palette[x] & ~FLIPMASK) | vFlip<<1|hFlip;
}

void TileMatrix::setFlipRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, bool hFlip, bool vFlip){
    if (x >= _width) {inv_arg("[TileMatrix::setFlipRect]: x is out of bounds");}
    if (y >= _height) {inv_arg("[TileMatrix::setFlipRect]: y is out of bounds");}
    if (width+x > _width) {inv_arg("[TileMatrix::setFlipRect]: width+x is out of bounds");}
    if (height+y > _height) {inv_arg("[TileMatrix::setFlipRect]: height+y is out of bounds");}
    for (uint16_t i = y; i < _height; i++) {
        for (uint16_t j = x; j < width; j++) {_tiles[i]._flip_palette[j] = (_tiles[i]._flip_palette[j] & ~FLIPMASK) | vFlip<<1|hFlip;}
    }
}

#pragma endregion
#pragma region invSetting

void TileMatrix::setInvert(uint16_t x, uint16_t y, bool invert){
    if (x >= _width) {inv_arg("[TileMatrix::setInvert]: x is out of bounds");}
    if (y >= _height) {inv_arg("[TileMatrix::setInvert]: y is out of bounds");}
    _tiles[y].setInvert(x, invert);
}

void TileMatrix::fillInvert(bool invert){
    for (uint16_t i = 0; i < _height; i++) {_tiles[i].fillInvert(invert);}
}

void TileMatrix::fillInvertRow(uint16_t row, bool invert){
    if (row >= _height) {inv_arg("[TileMatrix::fillInvertRow]: row is out of bounds");}
    _tiles[row].fillInvert(invert);
}

void TileMatrix::fillInvertCol(uint16_t col, bool invert){
    if (col >= _width) {inv_arg("[TileMatrix::fillInvertCol]: col is out of bounds");}
    for (uint16_t i = 0; i < _height; i++) {_tiles[i].setInvert(col, invert);}
}

void TileMatrix::fillInvertRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, bool invert){
    if (x >= _width) {inv_arg("[TileMatrix::fillInvertRect]: x is out of bounds");}
    if (y >= _height) {inv_arg("[TileMatrix::fillInvertRect]: y is out of bounds");}
    if (width+x > _width) {inv_arg("[TileMatrix::fillInvertRect]: width+x is out of bounds");}
    if (height+y > _height) {inv_arg("[TileMatrix::fillInvertRect]: height+y is out of bounds");}
    for (uint16_t i = y; i < height+y; i++) {
        _tiles[i].fillInvert(x, width, invert);
    }
}

#pragma endregion
#pragma region paletteSetting

void TileMatrix::setPalette(uint16_t x, uint16_t y, uint8_t palette){
    if (x >= _width) {inv_arg("[TileMatrix::setPalette]: x is out of bounds");}
    if (y >= _height) {inv_arg("[TileMatrix::setPalette]: y is out of bounds");}
    _tiles[y].setPalette(x, palette);
}

void TileMatrix::fillPalette(uint8_t palette){
    for (uint16_t i = 0; i < _height; i++) {_tiles[i].fillPalette(palette);}
}

void TileMatrix::fillPaletteRow(uint16_t row, uint8_t palette){
    if (row >= _height) {inv_arg("[TileMatrix::fillPaletteRow]: row is out of bounds");}
    _tiles[row].fillPalette(palette);
}

void TileMatrix::fillPaletteCol(uint16_t col, uint8_t palette){
    if (col >= _width) {inv_arg("[TileMatrix::fillPaletteCol]: col is out of bounds");}
    for (uint16_t i = 0; i < _height; i++) {_tiles[i].setPalette(col, palette);}
}

void TileMatrix::fillPaletteRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t palette){
    if (x >= _width) {inv_arg("[TileMatrix::fillPaletteRect]: x is out of bounds");}
    if (y >= _height) {inv_arg("[TileMatrix::fillPaletteRect]: y is out of bounds");}
    if (width+x > _width) {inv_arg("[TileMatrix::fillPaletteRect]: width+x is out of bounds");}
    if (height+y > _height) {inv_arg("[TileMatrix::fillPaletteRect]: height+y is out of bounds");}
    for (uint16_t i = y; i < height+y; i++) {
        _tiles[i].fillPalette(x, width, palette);
    }
}

#pragma endregion
#pragma region copying

void TileMatrix::copyRow(uint16_t row, uint32_t src[]){
    if (row >= _height) {inv_arg("[TileMatrix::copyRow]: row is out of bounds");}
    for (uint16_t i = 0; i < _width; i++){_tiles[row]._tiles[i] = src[i];}
}

void TileMatrix::copyCol(uint16_t col, uint32_t src[]){
    if (col >= _width) {inv_arg("[TileMatrix::copyCol]: col is out of bounds");}
    for (uint16_t i = 0; i < _height; i++){_tiles[i]._tiles[col] = src[i];}
}

void TileMatrix::copyRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t src[]){
    if (x >= _width) {inv_arg("[TileMatrix::copyRect]: x is out of bounds");}
    if (y >= _height) {inv_arg("[TileMatrix::copyRect]: y is out of bounds");}
    if (width+x > _width) {inv_arg("[TileMatrix::copyRect]: width+x is out of bounds");}
    if (height+y > _height) {inv_arg("[TileMatrix::copyRect]: height+y is out of bounds");}
    uint32_t ptr = 0;
    for (uint16_t i = y; i < height+y; i++){
        for (uint16_t j = x; j < width+x; j++){
            _tiles[i]._tiles[j] = src[ptr++];
        }
    }
}

void TileMatrix::copyRect(uint16_t out_x, uint16_t out_y, uint16_t width, uint16_t height, TileMatrix *src, uint16_t in_x, uint16_t in_y){

    #pragma region errorHandling
    if (in_x >= src->getWidth()) {inv_arg("[TileMatrix::copyRect]: x is out of bounds (source)");}
    if (in_y >= src->getHeight()) {inv_arg("[TileMatrix::copyRect]: y is out of bounds (source)");}
    if (width+in_x > src->getWidth()) {inv_arg("[TileMatrix::copyRect]: width+x is out of bounds (source)");}
    if (height+in_y > src->getHeight()) {inv_arg("[TileMatrix::copyRect]: height+y is out of bounds (source)");}

    if (out_x >= _width) {inv_arg("[TileMatrix::copyRect]: x is out of bounds (destination)");}
    if (out_y >= _height) {inv_arg("[TileMatrix::copyRect]: y is out of bounds (destination)");}
    if (width+out_x > _width) {inv_arg("[TileMatrix::copyRect]: width+x is out of bounds (destination)");}
    if (height+out_y > _height) {inv_arg("[TileMatrix::copyRect]: height+y is out of bounds (destination)");}
    #pragma endregion

    for (uint16_t i = 0; i < height; i++){
        for (uint16_t j = 0; j < width; j++){
            _tiles[out_y+i]._tiles[out_x+j] = src->_tiles[in_y+i]._tiles[in_x+j];
            _tiles[out_y+i]._flip_palette[out_x+j] = src->_tiles[in_y+i]._flip_palette[in_x+j];
        }
    }
}

#pragma endregion
#pragma region rendering

void TileMatrix::render(uint16_t x, uint16_t y, sf::RenderWindow *window, sf::Texture texture){
    sf::Vector2f texturePos {0, 0};
    uint8_t flip_palette;
    sf::Color color;
    for (uint16_t i = 0; i < _height && i*TILE_SIZE+y < window->getSize().y; i++){
        for (uint16_t j = 0; j < _width && j*TILE_SIZE+x < window->getSize().x; j++){
            flip_palette = _tiles[i]._flip_palette[j];
            texturePos = {
                static_cast<float>(flip_palette&INVMASK?TILE_SIZE:0),
                static_cast<float>((_tiles[i]._tiles[j]) << 3)
            };
            color = sf::Color(
                flip_palette&REDMASK?255:0,
                flip_palette&GRNMASK?255:0,
                flip_palette&BLUMASK?255:0);
            sf::Vertex vertices[4] = {
                sf::Vertex(sf::Vector2f(j*TILE_SIZE,            i*TILE_SIZE),
                    color, texturePos+sf::Vector2f(
                        flip_palette&HFLIP?TILE_SIZE:0,
                        flip_palette&VFLIP?TILE_SIZE:0)
                    ),
                sf::Vertex(sf::Vector2f(j*TILE_SIZE+TILE_SIZE,  i*TILE_SIZE),
                    color, texturePos+sf::Vector2f(
                        flip_palette&HFLIP?0:TILE_SIZE,
                        flip_palette&VFLIP?TILE_SIZE:0)
                    ),
                sf::Vertex(sf::Vector2f(j*TILE_SIZE+TILE_SIZE,  i*TILE_SIZE+TILE_SIZE),
                    color, texturePos+sf::Vector2f(
                        flip_palette&HFLIP?0:TILE_SIZE,
                        flip_palette&VFLIP?0:TILE_SIZE)
                    ),
                sf::Vertex(sf::Vector2f(j*TILE_SIZE,            i*TILE_SIZE+TILE_SIZE),
                    color, texturePos+sf::Vector2f(
                        flip_palette&HFLIP?TILE_SIZE:0,
                        flip_palette&VFLIP?0:TILE_SIZE)
                    )
            };
            window->draw(vertices, 4, sf::TriangleFan, sf::RenderStates(&texture));
        }
    }
}

sf::Texture TileMatrix::renderToTexture(sf::Texture texture){
    sf::Vector2f texturePos {0, 0};
    uint8_t flip_palette;
    sf::Color color;
    sf::RenderTexture target;
    target.create(_width*TILE_SIZE, _height*TILE_SIZE);
    for (uint16_t i = 0; i < _height; i++){
        uint16_t y = _height - i - 1;
        for (uint16_t j = 0; j < _width; j++){
            flip_palette = _tiles[i]._flip_palette[j];
            texturePos = {
                static_cast<float>(flip_palette&INVMASK?TILE_SIZE:0),
                static_cast<float>((_tiles[i]._tiles[j]) << 3)
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