#include "SFML/System/Vector2.hpp"
#pragma region header

#include <SFML/Graphics.hpp>
#include "Tile.cpp"
#include <vector>

#ifndef __CACHED_TILE_INCLUDED__
#define __CACHED_TILE_INCLUDED__

class AutoCachedTileMatrix : public TileMatrix {
    public:
        AutoCachedTileMatrix() {};
        AutoCachedTileMatrix(uint16_t __width, uint16_t __height)
            : TileMatrix(__width, __height), 
            cachedTexture(sf::Vector2u(__width * TILE_SIZE, __height * TILE_SIZE)) {
                updateVertices();
                fullCacheTexture();
            };
        AutoCachedTileMatrix(uint16_t __width, uint16_t __height, uint32_t __fillTile)
            : TileMatrix(__width, __height, __fillTile),
            cachedTexture(sf::Vector2u(__width * TILE_SIZE, __height * TILE_SIZE)) {
                updateVertices();
                fullCacheTexture();
            };

        inline void resize(uint16_t __width, uint16_t __height, uint32_t __fillTile = 0x20) override {
            TileMatrix::resize(__width, __height, __fillTile);
            cachedTexture.resize({__width * TILE_SIZE, __height * TILE_SIZE});
            updateVertices();
            fullCacheTexture();
        };

        #pragma region tileSetting

        /**
         * @brief Places the tile at coordinates, throws an error if the coordinates are out of bounds
         * @note Semi-useless due to [] existing
         * @param __x 
         * @param __y 
         * @param __tile 
         */
        inline void setTile(uint16_t __x, uint16_t __y, uint32_t __tile) override {
            TileMatrix::setTile(__x, __y, __tile);
            cacheTexture(__x, __y, 1, 1);
        };

        /**
         * @brief Fills the entire tile matrix with the tile
         * @param __tile 
         */
        inline void fill(uint32_t __tile) override {
            TileMatrix::fill(__tile);
            fullCacheTexture();
        };
        
        /**
         * @brief Fills the row (all tiles at Y coordinate) with the tile
         * @note Throws an error if the Y coordinate is out of bounds
         * @param __row 
         * @param __tile 
         */
        inline void fillRow(uint16_t __row, uint32_t __tile) override {
            TileMatrix::fillRow(__row, __tile);
            cacheTextureRow(__row);
        };

        /**
         * @brief Fills the column (all tiles at X coordinate) with the tile
         * @note Throws an error if the X coordinate is out of bounds
         * @param __col
         * @param __tile
         */
        inline void fillCol(uint16_t __col, uint32_t __tile) override {
            TileMatrix::fillCol(__col, __tile);
            cacheTextureCol(__col);
        };

        /**
         * @brief Fills a rectangle with the tile
         * @note Throws an error if the coordinates are out of bounds
         * @param __x
         * @param __y
         * @param __width
         * @param __height
         * @param __tile
         */
        inline void fillRect(uint16_t __x, uint16_t __y, uint16_t __width, uint16_t __height, uint32_t __tile) override {
            TileMatrix::fillRect(__x, __y, __width, __height, __tile);
            cacheTexture(__x, __y, __width, __height);
        };

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
        inline void setFlip(uint16_t __x, uint16_t __y, bool __hFlip, bool __vFlip) override {
            TileMatrix::setFlip(__x, __y, __hFlip, __vFlip);
            cacheTexture(__x, __y, 1, 1);
        };

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
        inline void setFlipRect(uint16_t __x, uint16_t __y, uint16_t __width, uint16_t __height, bool __hFlip, bool __vFlip) override {
            TileMatrix::setFlipRect(__x, __y, __width, __height, __hFlip, __vFlip);
            cacheTexture(__x, __y, __width, __height);
        };

        #pragma endregion
        #pragma region paletteSetting

        /**
         * @brief Sets the palette of the tile at coordinates
         * @note Throws an error if the coordinates are out of bounds
         * @param __x 
         * @param __y 
         * @param __palette 
         */
        inline void setPalette(uint16_t __x, uint16_t __y, uint8_t __palette) override {
            TileMatrix::setPalette(__x, __y, __palette);
            cacheTexture(__x, __y, 1, 1);
        };

        /**
         * @brief Sets the palette of the entire tile matrix
         * 
         * @param __palette 
         */
        inline void fillPalette(uint8_t __palette) override {
            TileMatrix::fillPalette(__palette);
            fullCacheTexture();
        };

        /**
         * @brief Sets the palette of the row (all tiles at Y coordinate)
         * @note Throws an error if the Y coordinate is out of bounds
         * @param __row 
         * @param __palette 
         */
        inline void fillPaletteRow(uint16_t __row, uint8_t __palette) override {
            TileMatrix::fillPaletteRow(__row, __palette);
            cacheTextureRow(__row);
        };

        /**
         * @brief Sets the palette of the row (all tiles at X coordinate)
         * @note Throws an error if the X coordinate is out of bounds
         * @param __col 
         * @param __palette 
         */
        inline void fillPaletteCol(uint16_t __col, uint8_t __palette) override {
            TileMatrix::fillPaletteCol(__col, __palette);
            cacheTextureCol(__col);
        };

        /**
         * @brief Sets the palette of a rectangle
         * @note Throws an error if the coordinates are out of bounds
         * @param __x 
         * @param __y 
         * @param __width 
         * @param __height 
         * @param __palette 
         */
        inline void fillPaletteRect(uint16_t __x, uint16_t __y, uint16_t __width, uint16_t __height, uint8_t __palette) override {
            TileMatrix::fillPaletteRect(__x, __y, __width, __height, __palette);
            cacheTexture(__x, __y, __width, __height);
        };

        #pragma endregion
        #pragma region invSetting

        /**
         * @brief Sets the inversion of the tile at coordinates
         * @note Throws an error if the coordinates are out of bounds
         * @param __x 
         * @param __y 
         * @param __invert 
         */
        inline void setInvert(uint16_t __x, uint16_t __y, bool __invert) override {
            TileMatrix::setInvert(__x, __y, __invert);
            cacheTexture(__x, __y, 1, 1);
        };

        /**
         * @brief Sets the inversion of the entire tile matrix
         * 
         * @param __invert 
         */
        inline void fillInvert(bool __invert) override {
            TileMatrix::fillInvert(__invert);
            fullCacheTexture();
        };

        /**
         * @brief Sets the inversion of the row (all tiles at Y coordinate)
         * @note Throws an error if the Y coordinate is out of bounds
         * @param __row 
         * @param __invert 
         */
        inline void fillInvertRow(uint16_t __row, bool __invert) override {
            TileMatrix::fillInvertRow(__row, __invert);
            cacheTextureRow(__row);
        };

        /**
         * @brief Sets the inversion of the row (all tiles at X coordinate)
         * @note Throws an error if the X coordinate is out of bounds
         * @param __col 
         * @param __invert 
         */
        inline void fillInvertCol(uint16_t __col, bool __invert) override {
            TileMatrix::fillInvertCol(__col, __invert);
            cacheTextureCol(__col);
        };

        /**
         * @brief Sets the inversion of a rectangle
         * @note Throws an error if the coordinates are out of bounds
         * @param __x 
         * @param __y 
         * @param __width 
         * @param __height 
         * @param __invert 
         */
        inline void fillInvertRect(uint16_t __x, uint16_t __y, uint16_t __width, uint16_t __height, bool __invert) override {
            TileMatrix::fillInvertRect(__x, __y, __width, __height, __invert);
            cacheTexture(__x, __y, __width, __height);
        };

        #pragma endregion
        #pragma region copying

        /**
         * @brief Copies row from input array into the specified row (tiles with the Y coordinate)
         * @note Throws an error if the Y coordinate is out of bounds
         * @param __row 
         * @param __src 
         */
        void copyRow(uint16_t __row, const uint32_t * __src) override {
            TileMatrix::copyRow(__row, __src);
            cacheTextureRow(__row);
        };

        /**
         * @brief Copies column from input array into the specified row (tiles with the X coordinate)
         * @note Throws an error if the X coordinate is out of bounds
         * @param __col 
         * @param __src 
         */
        void copyCol(uint16_t __col, const uint32_t * __src) override {
            TileMatrix::copyCol(__col, __src);
            cacheTextureCol(__col);
        };

        /**
         * @brief Copies a rectangle from input one-dimensional array, from left to right, from top to bottom; 
         * @note Throws an error if the coordinates are out of bounds
         * @param __x 
         * @param __y 
         * @param __width 
         * @param __height 
         * @param __src 
         */
        void copyRect(uint16_t __x, uint16_t __y, uint16_t __width, uint16_t __height, const uint32_t * __src) override {
            TileMatrix::copyRect(__x, __y, __width, __height, __src);
            cacheTexture(__x, __y, __width, __height);
        };

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
        void copyRect(uint16_t __out_x, uint16_t __out_y, uint16_t __width, uint16_t __height, const TileMatrix *__src, uint16_t __in_x, uint16_t __in_y) override {
            TileMatrix::copyRect(__out_x, __out_y, __width, __height, __src, __in_x, __in_y);
            cacheTexture(__out_x, __out_y, __width, __height);
        };

        #pragma endregion
        #pragma region rendering

        /**
         * @brief Set the texture
         * 
         * @param __texture 
         */
        void setTexture(sf::Texture & __texture) override {
            TileMatrix::setTexture(__texture);
            fullCacheTexture();
        };

        /**
         * @brief Set the position
         * 
         * @param __position 
         */
        void setPosition(sf::Vector2f __position) override {
            TileMatrix::setPosition(__position);
            updateVertices();
        };

        /**
         * @brief Render the tile matrix to a texture
         * 
         * @param __texture 
         * @return sf::Texture 
         */
        sf::Texture renderToTexture() const { return cachedTexture.getTexture(); };

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
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

        void cacheTexture(uint16_t __x, uint16_t __y, uint16_t __width, uint16_t __height);
        inline void cacheTextureRow(uint16_t __row) { cacheTexture(0, __row, getWidth(), 1); }
        inline void cacheTextureCol(uint16_t __col) { cacheTexture(__col, 0, 1, getHeight()); }
        inline void fullCacheTexture() { cacheTexture(0, 0, getWidth(), getHeight()); };

        inline void updateVertices() {
            float w = cachedTexture.getSize().x;
            float h = cachedTexture.getSize().y;
            vertices[0] = sf::Vertex{pos,                      sf::Color::White, {0, 0}};
            vertices[1] = sf::Vertex{pos + sf::Vector2f(w, 0), sf::Color::White, {w, 0}};
            vertices[2] = sf::Vertex{pos + sf::Vector2f(w, h), sf::Color::White, {w, h}};
            vertices[3] = sf::Vertex{pos + sf::Vector2f(0, h), sf::Color::White, {0, h}};
        }

        sf::RenderTexture cachedTexture;

        sf::Vertex vertices[4];

};

#pragma endregion

#pragma region cachingTexture

void AutoCachedTileMatrix::cacheTexture(uint16_t __x, uint16_t __y, uint16_t __width, uint16_t __height) {
    sf::Vector2f texturePos {0, 0};
    uint8_t flip_palette;
    sf::Color color;
    if (getTexture() == nullptr) return;
    for (uint16_t i = __y; i < (__y + __height); i++){
        uint16_t y = getHeight() - i - 1;
        for (uint16_t j = __x; j < (__x + __width); j++){
            flip_palette = tiles[i][j].flip_palette;
            texturePos = sf::Vector2f(
                flip_palette&INVMASK?TILE_SIZE:0,
                (tiles[i][j].tileIndex) << 3
            );
            color = sf::Color(
                flip_palette&REDMASK?255:0,
                flip_palette&GRNMASK?255:0,
                flip_palette&BLUMASK?255:0);
            sf::Vertex vertices[4] = {
                sf::Vertex{sf::Vector2f(j*TILE_SIZE,            y*TILE_SIZE+TILE_SIZE),
                    color, texturePos+sf::Vector2f(
                        flip_palette&HFLIP?TILE_SIZE:0,
                        flip_palette&VFLIP?TILE_SIZE:0)
                    },
                sf::Vertex{sf::Vector2f(j*TILE_SIZE+TILE_SIZE,  y*TILE_SIZE+TILE_SIZE),
                    color, texturePos+sf::Vector2f(
                        flip_palette&HFLIP?0:TILE_SIZE,
                        flip_palette&VFLIP?TILE_SIZE:0)
                    },
                sf::Vertex{sf::Vector2f(j*TILE_SIZE+TILE_SIZE,  y*TILE_SIZE),
                    color, texturePos+sf::Vector2f(
                        flip_palette&HFLIP?0:TILE_SIZE,
                        flip_palette&VFLIP?0:TILE_SIZE)
                    },
                sf::Vertex{sf::Vector2f(j*TILE_SIZE,            y*TILE_SIZE),
                    color, texturePos+sf::Vector2f(
                        flip_palette&HFLIP?TILE_SIZE:0,
                        flip_palette&VFLIP?0:TILE_SIZE)
                    }
            };
            cachedTexture.draw(vertices, 4, sf::PrimitiveType::TriangleFan, sf::RenderStates(getTexture()));
        }
    }
}

void AutoCachedTileMatrix::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    states.texture = &cachedTexture.getTexture();
    target.draw(vertices, 4, sf::PrimitiveType::TriangleFan, states);
}

#pragma endregion

#endif  // __CACHED_TILE_INCLUDED__