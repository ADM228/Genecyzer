#include "SFML/System/Vector2.hpp"
#pragma region header

#include <SFML/Graphics.hpp>
#include <vector>

#ifndef __TILE_INCLUDED__
#define __TILE_INCLUDED__

#define TILE_SIZE 8

struct Tile {
    uint32_t tileIndex;
    uint8_t flip_palette;
};

class TileMatrix : public sf::Drawable {
    public:
        TileMatrix() {};
        TileMatrix(uint16_t __width, uint16_t __height);
        TileMatrix(uint16_t __width, uint16_t __height, uint32_t __fillTile);

        void resize(uint16_t __width, uint16_t __height, uint32_t __fillTile = 0x20);

        #pragma region tileSetting

        /**
         * @brief Places the tile at coordinates, throws an error if the coordinates are out of bounds
         * @note Semi-useless due to [] existing
         * @param __x 
         * @param __y 
         * @param __tile 
         */
        void setTile(uint16_t __x, uint16_t __y, uint32_t __tile);

        /**
         * @brief Fills the entire tile matrix with the tile
         * @param __tile 
         */
        void fill(uint32_t __tile);
        
        /**
         * @brief Fills the row (all tiles at Y coordinate) with the tile
         * @note Throws an error if the Y coordinate is out of bounds
         * @param __row 
         * @param __tile 
         */
        void fillRow(uint16_t __row, uint32_t __tile);

        /**
         * @brief Fills the column (all tiles at X coordinate) with the tile
         * @note Throws an error if the X coordinate is out of bounds
         * @param __col
         * @param __tile
         */
        void fillCol(uint16_t __col, uint32_t __tile);

        /**
         * @brief Fills a rectangle with the tile
         * @note Throws an error if the coordinates are out of bounds
         * @param __x
         * @param __y
         * @param __width
         * @param __height
         * @param __tile
         */
        void fillRect(uint16_t __x, uint16_t __y, uint16_t __width, uint16_t __height, uint32_t __tile);

        #pragma endregion
        #pragma region flipSetting

        /**
         * @brief Sets the flipping parameters of a tile
         * 
         * @param __x 
         * @param __y 
         * @param __hFlip 
         * @param __vFlip 
         */
        void setFlip(uint16_t __x, uint16_t __y, bool __hFlip, bool __vFlip);

        /**
         * @brief Sets the flipping parameters of a rectangle
         * 
         * @param __x 
         * @param __y 
         * @param __width 
         * @param __height 
         * @param __hFlip 
         * @param __vFlip 
         */
        void setFlipRect(uint16_t __x, uint16_t __y, uint16_t __width, uint16_t __height, bool __hFlip, bool __vFlip);

        #pragma endregion
        #pragma region paletteSetting

        /**
         * @brief Sets the palette of the tile at coordinates
         * @note Throws an error if the coordinates are out of bounds
         * @param __x 
         * @param __y 
         * @param __palette 
         */
        void setPalette(uint16_t __x, uint16_t __y, uint8_t __palette);

        /**
         * @brief Sets the palette of the entire tile matrix
         * 
         * @param __palette 
         */
        void fillPalette(uint8_t __palette);

        /**
         * @brief Sets the palette of the row (all tiles at Y coordinate)
         * @note Throws an error if the Y coordinate is out of bounds
         * @param __row 
         * @param __palette 
         */
        void fillPaletteRow(uint16_t __row, uint8_t __palette);

        /**
         * @brief Sets the palette of the row (all tiles at X coordinate)
         * @note Throws an error if the X coordinate is out of bounds
         * @param __col 
         * @param __palette 
         */
        void fillPaletteCol(uint16_t __col, uint8_t __palette);

        /**
         * @brief Sets the palette of a rectangle
         * @note Throws an error if the coordinates are out of bounds
         * @param __x 
         * @param __y 
         * @param __width 
         * @param __height 
         * @param __palette 
         */
        void fillPaletteRect(uint16_t __x, uint16_t __y, uint16_t __width, uint16_t __height, uint8_t __palette);

        #pragma endregion
        #pragma region invSetting

        /**
         * @brief Sets the inversion of the tile at coordinates
         * @note Throws an error if the coordinates are out of bounds
         * @param __x 
         * @param __y 
         * @param __invert 
         */
        void setInvert(uint16_t __x, uint16_t __y, bool __invert);

        /**
         * @brief Sets the inversion of the entire tile matrix
         * 
         * @param __invert 
         */
        void fillInvert(bool __invert);

        /**
         * @brief Sets the inversion of the row (all tiles at Y coordinate)
         * @note Throws an error if the Y coordinate is out of bounds
         * @param __row 
         * @param __invert 
         */
        void fillInvertRow(uint16_t __row, bool __invert);

        /**
         * @brief Sets the inversion of the row (all tiles at X coordinate)
         * @note Throws an error if the X coordinate is out of bounds
         * @param __col 
         * @param __invert 
         */
        void fillInvertCol(uint16_t __col, bool __invert);

        /**
         * @brief Sets the inversion of a rectangle
         * @note Throws an error if the coordinates are out of bounds
         * @param __x 
         * @param __y 
         * @param __width 
         * @param __height 
         * @param __invert 
         */
        void fillInvertRect(uint16_t __x, uint16_t __y, uint16_t __width, uint16_t __height, bool __invert);

        #pragma endregion
        #pragma region copying

        /**
         * @brief Copies row from input array into the specified row (tiles with the Y coordinate)
         * @note Throws an error if the Y coordinate is out of bounds
         * @param __row 
         * @param __src 
         */
        void copyRow(uint16_t __row, const uint32_t * __src);

        /**
         * @brief Copies column from input array into the specified row (tiles with the X coordinate)
         * @note Throws an error if the X coordinate is out of bounds
         * @param __col 
         * @param __src 
         */
        void copyCol(uint16_t __col, const uint32_t * __src);

        /**
         * @brief Copies a rectangle from input one-dimensional array, from left to right, from top to bottom; 
         * @note Throws an error if the coordinates are out of bounds
         * @param __x 
         * @param __y 
         * @param __width 
         * @param __height 
         * @param __src 
         */
        void copyRect(uint16_t __x, uint16_t __y, uint16_t __width, uint16_t __height, const uint32_t * __src);

        /**
         * @brief Copies a rectangle from another TileMatrix
         * @note Throws an error if the coordinates are out of bounds
         * @param __out_x 
         * @param __out_y 
         * @param __width 
         * @param __height 
         * @param __src 
         * @param __in_x 
         * @param __in_y 
         */
        void copyRect(uint16_t __out_x, uint16_t __out_y, uint16_t __width, uint16_t __height, const TileMatrix *__src, uint16_t __in_x, uint16_t __in_y);

        #pragma endregion
        #pragma region rendering

        /**
         * @brief Set the texture
         * 
         * @param __texture 
         */
        void setTexture(sf::Texture & __texture) { texture = &__texture; };

        /**
         * @brief Set the position
         * 
         * @param __position 
         */
        void setPosition(sf::Vector2f __position) { pos = __position; };

        /**
         * @brief Render the tile matrix to a texture
         * 
         * @param __texture 
         * @return sf::Texture 
         */
        sf::Texture renderToTexture(sf::Texture __texture);

        #pragma endregion
        #pragma region gettingParams

        const inline uint16_t getWidth () const { return width; };
        const inline uint16_t getHeight() const { return height; };

              std::vector<Tile>& operator[](std::size_t idx)       { return tiles[idx]; }
        const std::vector<Tile>& operator[](std::size_t idx) const { return tiles[idx]; }

        #pragma endregion

        static constexpr uint8_t HFLIP = 0x01;
        static constexpr uint8_t VFLIP = 0x02;
        static constexpr uint8_t FLIPMASK = 0x03;

        static constexpr uint8_t REDMASK = 0x10;
        static constexpr uint8_t GRNMASK = 0x20;
        static constexpr uint8_t BLUMASK = 0x40;
        static constexpr uint8_t PALMASK = 0x70;
        static constexpr uint8_t INVMASK = 0x80;

    private:

        /**
         * @brief Internal function, renders TileMatrix to a sf::RenderTarget
         * @note Called sf::RenderTarget::draw(TileMatrix, args)
         * @param target 
         * @param states 
         */
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

        uint16_t width, height;
        std::vector<std::vector<Tile>> tiles;
        sf::Texture * texture;
        sf::Vector2f pos {0, 0};

};

#pragma endregion

#ifdef BREAK_ON_EXCEPTIONS
#include <stdexcept>    
#define inv_arg(x) throw std::invalid_argument(x)
#else
uint32_t exception_count = 0;
#define inv_arg(x) fprintf(stderr, "[Tile.cpp #%08X]: ", exception_count++); fprintf(stderr, x); fprintf(stderr, "\n")
#endif

TileMatrix::TileMatrix(uint16_t __width, uint16_t __height){
    tiles = std::vector<std::vector<Tile>>(__height, std::vector<Tile>(__width, {0, PALMASK}));
    height = __height;
    width = __width;
}

TileMatrix::TileMatrix(uint16_t __width, uint16_t __height, uint32_t fillTile){
    tiles = std::vector<std::vector<Tile>>(__height, std::vector<Tile>(__width, {fillTile, PALMASK}));
    height = __height;
    width = __width;
}

void TileMatrix::resize(uint16_t __width, uint16_t __height, uint32_t fillTile){
    if (__width != width){
        for (uint16_t i = 0; i < height; i++){
            tiles[i].resize(__width, Tile{fillTile, PALMASK});
        }
        width = __width;
    }
    if (__height != height){
        tiles.resize(__height, std::vector<Tile>(__width, {fillTile, PALMASK}));
        height = __height;
    }
}

#pragma region tileSetting

void TileMatrix::setTile(uint16_t x, uint16_t y, uint32_t tile){
    if (y >= tiles.size()) {inv_arg("[TileMatrix::setTile]: y is out of bounds"); return;}
    if (x >= tiles[y].size()) {inv_arg("[TileMatrix::setTile]: x is out of bounds"); return;}
    tiles[y][x].tileIndex = tile;
}

void TileMatrix::fill(uint32_t tile){
    for (uint16_t i = 0; i < tiles.size(); i++) {
        for (uint16_t j = 0; j < tiles[i].size(); j++)
            tiles[i][j].tileIndex = tile;    
    }
}

void TileMatrix::fillRow(uint16_t row, uint32_t tile){
    if (row >= tiles.size()) {inv_arg("[TileMatrix::fillRow]: row is out of bounds"); return;}
    for (uint16_t j = 0; j < tiles[row].size(); j++)
        tiles[row][j].tileIndex = tile; 
}

void TileMatrix::fillCol(uint16_t col, uint32_t tile){
    if (col >= width) {inv_arg("[TileMatrix::fillCol]: col is out of bounds");}
        for (uint16_t i = 0; i < tiles.size(); i++)
            if (col < tiles[i].size()) tiles[i][col].tileIndex = tile;   
}

void TileMatrix::fillRect(uint16_t x, uint16_t y, uint16_t __width, uint16_t __height, uint32_t tile){
    if (x >= width) {inv_arg("[TileMatrix::fillRect]: x is out of bounds"); return;}
    if (y >= tiles.size()) {inv_arg("[TileMatrix::fillRect]: y is out of bounds"); return;}
    if (__width+x > width) {inv_arg("[TileMatrix::fillRect]: width+x is out of bounds");}
    if (__height+y > tiles.size()) {inv_arg("[TileMatrix::fillRect]: height+y is out of bounds");}
    for (uint16_t i = y; i < __height+y && i < tiles.size(); i++) {
        for (uint16_t j = x; j < __width+x && j < tiles[i].size(); j++) {
             tiles[i][j].tileIndex = tile;
        }
    }
}

#pragma endregion
#pragma region flipSetting

void TileMatrix::setFlip(uint16_t x, uint16_t y, bool hFlip, bool vFlip){
    if (y >= tiles.size()) {inv_arg("[TileMatrix::setFlip]: y is out of bounds"); return;}
    if (x >= tiles[y].size()) {inv_arg("[TileMatrix::setFlip]: x is out of bounds"); return;}
    tiles[y][x].flip_palette = (tiles[y][x].flip_palette & ~FLIPMASK) | vFlip<<1|hFlip;
}

void TileMatrix::setFlipRect(uint16_t x, uint16_t y, uint16_t __width, uint16_t __height, bool hFlip, bool vFlip){
    if (x >= width) {inv_arg("[TileMatrix::setFlipRect]: x is out of bounds"); return;}
    if (y >= tiles.size()) {inv_arg("[TileMatrix::setFlipRect]: y is out of bounds"); return;}
    if (__width+x > width) {inv_arg("[TileMatrix::setFlipRect]: width+x is out of bounds");}
    if (__height+y > tiles.size()) {inv_arg("[TileMatrix::setFlipRect]: height+y is out of bounds");}
    for (uint16_t i = y; i < __height+y && i < tiles.size(); i++) {
        for (uint16_t j = x; j < __width+x && j < tiles[i].size(); j++) {
            tiles[i][j].flip_palette = (tiles[i][j].flip_palette & ~FLIPMASK) | vFlip<<1|hFlip;
        }
    }
}

#pragma endregion
#pragma region invSetting

void TileMatrix::setInvert(uint16_t x, uint16_t y, bool invert){
    if (y >= tiles.size()) {inv_arg("[TileMatrix::setInvert]: y is out of bounds"); return;}
    if (x >= tiles[y].size()) {inv_arg("[TileMatrix::setInvert]: x is out of bounds"); return;}
    tiles[y][x].flip_palette = (tiles[y][x].flip_palette & ~INVMASK) | invert << 7;
}

void TileMatrix::fillInvert(bool invert){
    for (uint16_t i = 0; i < tiles.size(); i++) {
        for (uint16_t j = 0; j < tiles[i].size(); j++)
            tiles[i][j].flip_palette = (tiles[i][j].flip_palette & ~INVMASK) | invert << 7;
    }
}


void TileMatrix::fillInvertRow(uint16_t row, bool invert){
    if (row >= tiles.size()) {inv_arg("[TileMatrix::fillInvertRow]: row is out of bounds"); return;}
    for (uint16_t j = 0; j < tiles[row].size(); j++)
        tiles[row][j].flip_palette = (tiles[row][j].flip_palette & ~INVMASK) | invert << 7;
}

void TileMatrix::fillInvertCol(uint16_t col, bool invert){
    if (col >= width) {inv_arg("[TileMatrix::fillInvertCol]: col is out of bounds");}
    for (uint16_t i = 0; i < tiles.size(); i++)
        if (col < tiles[i].size()) tiles[i][col].flip_palette = (tiles[i][col].flip_palette & ~INVMASK) | invert << 7;
}

void TileMatrix::fillInvertRect(uint16_t x, uint16_t y, uint16_t __width, uint16_t __height, bool invert){
    if (x >= width) {inv_arg("[TileMatrix::fillInvertRect]: x is out of bounds"); return;}
    if (y >= tiles.size()) {inv_arg("[TileMatrix::fillInvertRect]: y is out of bounds"); return;}
    if (__width+x > width) {inv_arg("[TileMatrix::fillInvertRect]: width+x is out of bounds");}
    if (__height+y > tiles.size()) {inv_arg("[TileMatrix::fillInvertRect]: height+y is out of bounds");}
    for (uint16_t i = y; i < __height+y && i < tiles.size(); i++) {
        for (uint16_t j = x; j < __width+x && j < tiles[i].size(); j++)
            tiles[i][j].flip_palette = (tiles[i][j].flip_palette & ~INVMASK) | invert << 7;
    }
}

#pragma endregion
#pragma region paletteSetting

void TileMatrix::setPalette(uint16_t x, uint16_t y, uint8_t palette){
    if (y >= tiles.size()) {inv_arg("[TileMatrix::setPalette]: y is out of bounds"); return;}
    if (x >= tiles[y].size()) {inv_arg("[TileMatrix::setPalette]: x is out of bounds"); return;}
    tiles[y][x].flip_palette = (tiles[y][x].flip_palette & ~PALMASK) | ((palette << 4) & PALMASK);
}

void TileMatrix::fillPalette(uint8_t palette){
    for (uint16_t i = 0; i < tiles.size(); i++) {
        for (uint16_t j = 0; j < tiles[i].size(); j++)
            tiles[i][j].flip_palette = (tiles[i][j].flip_palette & ~PALMASK) | ((palette << 4) & PALMASK);
    }
}

void TileMatrix::fillPaletteRow(uint16_t row, uint8_t palette){
    if (row >= tiles.size()) {inv_arg("[TileMatrix::fillPaletteRow]: row is out of bounds"); return;}
    for (uint16_t j = 0; j < tiles[row].size(); j++)
        tiles[row][j].flip_palette = (tiles[row][j].flip_palette & ~PALMASK) | ((palette << 4) & PALMASK);
}

void TileMatrix::fillPaletteCol(uint16_t col, uint8_t palette){
    if (col >= width) {inv_arg("[TileMatrix::fillPaletteCol]: col is out of bounds");}
    for (uint16_t i = 0; i < tiles.size(); i++)
        if (col < tiles[i].size()) tiles[i][col].flip_palette = (tiles[i][col].flip_palette & ~PALMASK) | ((palette << 4) & PALMASK);
}

void TileMatrix::fillPaletteRect(uint16_t x, uint16_t y, uint16_t __width, uint16_t __height, uint8_t palette){
    if (x >= width) {inv_arg("[TileMatrix::fillPaletteRect]: x is out of bounds"); return;}
    if (y >= tiles.size()) {inv_arg("[TileMatrix::fillPaletteRect]: y is out of bounds"); return;}
    if (__width+x > width) {inv_arg("[TileMatrix::fillPaletteRect]: width+x is out of bounds");}
    if (__height+y > tiles.size()) {inv_arg("[TileMatrix::fillPaletteRect]: height+y is out of bounds");}
    for (uint16_t i = y; i < __height+y && i < tiles.size(); i++) {
        for (uint16_t j = x; j < __width+x && j < tiles[i].size(); j++)
            tiles[i][j].flip_palette = (tiles[i][j].flip_palette & ~PALMASK) | ((palette << 4) & PALMASK);
    }
}

#pragma endregion
#pragma region copying

void TileMatrix::copyRow(uint16_t row, const uint32_t * src){
    if (row >= tiles.size()) {inv_arg("[TileMatrix::copyRow]: row is out of bounds"); return;}
    for (uint16_t i = 0; i < tiles[row].size(); i++){tiles[row][i].tileIndex = src[i];}
}

void TileMatrix::copyCol(uint16_t col, const uint32_t * src){
    if (col >= width) {inv_arg("[TileMatrix::copyCol]: col is out of bounds");}
    for (uint16_t i = 0; i < tiles.size(); i++){
        if (col < tiles[i].size()) tiles[i][col].tileIndex = src[i];
    }
}

void TileMatrix::copyRect(uint16_t x, uint16_t y, uint16_t __width, uint16_t __height, const uint32_t * src){
    if (x >= width) {inv_arg("[TileMatrix::copyRect]: x is out of bounds"); return;}
    if (y >= tiles.size()) {inv_arg("[TileMatrix::copyRect]: y is out of bounds"); return;}
    if (__width+x > width) {inv_arg("[TileMatrix::copyRect]: width+x is out of bounds, tiles are gonna get shifted");}
    if (__height+y > tiles.size()) {inv_arg("[TileMatrix::copyRect]: height+y is out of bounds");}
    uint32_t ptr = 0;
    for (uint16_t i = y; i < __height+y && i < tiles.size(); i++){
        for (uint16_t j = x; j < __width+x && j < tiles[i].size(); j++){
            tiles[i][j].tileIndex = src[ptr++];
        }
    }
}

void TileMatrix::copyRect(uint16_t out_x, uint16_t out_y, uint16_t __width, uint16_t __height, const TileMatrix *src, uint16_t in_x, uint16_t in_y){

    #pragma region errorHandling
    if (in_x >= src->width) {inv_arg("[TileMatrix::copyRect]: x is out of bounds (source)"); return;}
    if (in_y >= src->tiles.size()) {inv_arg("[TileMatrix::copyRect]: y is out of bounds (source)"); return;}
    if (__width+in_x > src->width) {inv_arg("[TileMatrix::copyRect]: width+x is out of bounds (source)");}
    if (__height+in_y > src->tiles.size()) {inv_arg("[TileMatrix::copyRect]: height+y is out of bounds (source)");}

    if (out_x >= width) {inv_arg("[TileMatrix::copyRect]: x is out of bounds (destination)"); return;}
    if (out_y >= tiles.size()) {inv_arg("[TileMatrix::copyRect]: y is out of bounds (destination)"); return;}
    if (__width+out_x > width) {inv_arg("[TileMatrix::copyRect]: width+x is out of bounds (destination)");}
    if (__height+out_y > tiles.size()) {inv_arg("[TileMatrix::copyRect]: height+y is out of bounds (destination)");}
    #pragma endregion

    for (uint16_t i = 0; i < __height && in_y+i < src->tiles.size() && out_y+i < tiles.size(); i++){
        for (uint16_t j = 0; j < __width && in_x+j < src->width && out_x+j < width; j++){
            tiles[out_y+i][out_x+j] = src->tiles[in_y+i][in_x+j];
        }
    }
}

#pragma endregion
#pragma region rendering

void TileMatrix::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    sf::Vector2f texturePos {0, 0};
    uint8_t flip_palette;
    sf::Color color;
    float x = pos.x, y = pos.y;
    if (texture == nullptr) return;
    else states.texture = texture;
    for (uint16_t i = 0; i < tiles.size() && i*TILE_SIZE < target.getSize().y; i++){
        for (uint16_t j = 0; j < tiles[i].size() && j*TILE_SIZE < target.getSize().x; j++){
            flip_palette = tiles[i][j].flip_palette;
            texturePos = {
                static_cast<float>(flip_palette&INVMASK?TILE_SIZE:0),
                static_cast<float>((tiles[i][j].tileIndex) << 3)
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
    target.create(width*TILE_SIZE, height*TILE_SIZE);
    for (uint16_t i = 0; i < tiles.size(); i++){
        uint16_t y = height - i - 1;
        for (uint16_t j = 0; j < tiles[i].size(); j++){
            flip_palette = tiles[i][j].flip_palette;
            texturePos = {
                static_cast<float>(flip_palette&INVMASK?TILE_SIZE:0),
                static_cast<float>((tiles[i][j].tileIndex) << 3)
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