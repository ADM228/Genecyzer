#include "Tile.hpp"
#include <iostream>
#include <stdexcept>

Tile::Tile(uint32_t x, uint32_t y){
    pos.x = x * 8;
    pos.y = y * 8;
    texturePos = sf::Vector2f{0, 0};
    updateRenderVertex();
}

Tile::Tile(uint32_t x, uint32_t y, uint32_t tile){
    pos.x = x * 8;
    pos.y = y * 8;
    texturePos = sf::Vector2f{0, static_cast<float>(tile << 3)};
    updateRenderVertex();
}

Tile::Tile(uint32_t x, uint32_t y, uint32_t tile, bool hFlip, bool vFlip){
    pos.x = x * 8;
    pos.y = y * 8;
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
    renderVertex.append(sf::Vertex(pos, texturePos+sf::Vector2f(_hFlip?8:0,_vFlip?8:0)));
    renderVertex.append(sf::Vertex(pos+sf::Vector2f(8,0), texturePos+sf::Vector2f(_hFlip?0:8,_vFlip?8:0)));
    renderVertex.append(sf::Vertex(pos+sf::Vector2f(8,8), texturePos+sf::Vector2f(_hFlip?0:8,_vFlip?0:8)));
    renderVertex.append(sf::Vertex(pos+sf::Vector2f(0,8), texturePos+sf::Vector2f(_hFlip?8:0,_vFlip?0:8)));
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

void TileRow::copy (uint32_t src[]){
    for (uint16_t i = 0; i < _tiles.size(); i++){_tiles[i] = src[i];}
}

void TileRow::copy (uint16_t offset, uint16_t length, uint32_t src[]){
    if (offset >= _tiles.size()) {throw std::invalid_argument("[TileRow::copy]: offset is out of bounds");}
    if (offset+length > _tiles.size()) {throw std::invalid_argument("[TileRow::copy]: offset+length is out of bounds");}
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

#pragma region tileSetting

void TileMatrix::setTile(uint16_t x, uint16_t y, uint32_t tile){
    if (x >= _width) {throw std::invalid_argument("[TileMatrix::setTile]: x is out of bounds");}
    if (y >= _height) {throw std::invalid_argument("[TileMatrix::setTile]: y is out of bounds");}
    _tiles[y]._tiles[x] = tile;
}

void TileMatrix::fill(uint32_t tile){
    for (uint16_t i = 0; i < _height; i++) {_tiles[i]._tiles.assign(_width, tile);}
}

void TileMatrix::fillRow(uint16_t row, uint32_t tile){
    if (row >= _height) {throw std::invalid_argument("[TileMatrix::fillRow]: row is out of bounds");}
    _tiles[row]._tiles.assign(_width, tile);
}

void TileMatrix::fillCol(uint16_t col, uint32_t tile){
    if (col >= _width) {throw std::invalid_argument("[TileMatrix::fillCol]: col is out of bounds");}
    for (uint16_t i = 0; i < _height; i++) {_tiles[i]._tiles[col] = tile;}
}

void TileMatrix::fillRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t tile){
    if (x >= _width) {throw std::invalid_argument("[TileMatrix::fillRect]: x is out of bounds");}
    if (y >= _height) {throw std::invalid_argument("[TileMatrix::fillRect]: y is out of bounds");}
    if (width+x > _width) {throw std::invalid_argument("[TileMatrix::fillRect]: width+x is out of bounds");}
    if (height+y > _height) {throw std::invalid_argument("[TileMatrix::fillRect]: height+y is out of bounds");}
    for (uint16_t i = y; i < _height; i++) {
        for (uint16_t j = x; j < width; j++) {_tiles[i]._tiles[j] = tile;}
    }
}

#pragma endregion
#pragma region flipSetting

void TileMatrix::setFlip(uint16_t x, uint16_t y, bool hFlip, bool vFlip){
    if (x >= _width) {throw std::invalid_argument("[TileMatrix::setFlip]: x is out of bounds");}
    if (y >= _height) {throw std::invalid_argument("[TileMatrix::setFlip]: y is out of bounds");}
    _tiles[y]._flip_palette[x] = (_tiles[y]._flip_palette[x] & ~FLIPMASK) | vFlip<<1|hFlip;
}

void TileMatrix::setFlipRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, bool hFlip, bool vFlip){
    if (x >= _width) {throw std::invalid_argument("[TileMatrix::setFlipRect]: x is out of bounds");}
    if (y >= _height) {throw std::invalid_argument("[TileMatrix::setFlipRect]: y is out of bounds");}
    if (width+x > _width) {throw std::invalid_argument("[TileMatrix::setFlipRect]: width+x is out of bounds");}
    if (height+y > _height) {throw std::invalid_argument("[TileMatrix::setFlipRect]: height+y is out of bounds");}
    for (uint16_t i = y; i < _height; i++) {
        for (uint16_t j = x; j < width; j++) {_tiles[i]._flip_palette[j] = (_tiles[i]._flip_palette[j] & ~FLIPMASK) | vFlip<<1|hFlip;}
    }
}

#pragma endregion
#pragma region paletteSetting

void TileMatrix::setPalette(uint16_t x, uint16_t y, uint8_t palette){
    if (x >= _width) {throw std::invalid_argument("[TileMatrix::setPalette]: x is out of bounds");}
    if (y >= _height) {throw std::invalid_argument("[TileMatrix::setPalette]: y is out of bounds");}
    _tiles[y].setPalette(x, palette);
}

void TileMatrix::fillPalette(uint8_t palette){
    for (uint16_t i = 0; i < _height; i++) {_tiles[i].fillPalette(palette);}
}

void TileMatrix::fillPaletteRow(uint16_t row, uint8_t palette){
    if (row >= _height) {throw std::invalid_argument("[TileMatrix::fillPaletteRow]: row is out of bounds");}
    _tiles[row].fillPalette(palette);
}

void TileMatrix::fillPaletteCol(uint16_t col, uint8_t palette){
    if (col >= _width) {throw std::invalid_argument("[TileMatrix::fillPaletteCol]: col is out of bounds");}
    for (uint16_t i = 0; i < _height; i++) {_tiles[i].setPalette(col, palette);}
}

void TileMatrix::fillPaletteRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t palette){
    if (x >= _width) {throw std::invalid_argument("[TileMatrix::fillPaletteRect]: x is out of bounds");}
    if (y >= _height) {throw std::invalid_argument("[TileMatrix::fillPaletteRect]: y is out of bounds");}
    if (width+x > _width) {throw std::invalid_argument("[TileMatrix::fillPaletteRect]: width+x is out of bounds");}
    if (height+y > _height) {throw std::invalid_argument("[TileMatrix::fillPaletteRect]: height+y is out of bounds");}
    for (uint16_t i = y; i < _height; i++) {
        _tiles[i].fillPalette(x, width, palette);
    }
}

#pragma endregion
#pragma region copying

void TileMatrix::copyRow(uint16_t row, uint32_t src[]){
    if (row >= _height) {throw std::invalid_argument("[TileMatrix::copyRow]: row is out of bounds");}
    for (uint16_t i = 0; i < _width; i++){_tiles[row]._tiles[i] = src[i];}
}

void TileMatrix::copyCol(uint16_t col, uint32_t src[]){
    if (col >= _width) {throw std::invalid_argument("[TileMatrix::copyCol]: col is out of bounds");}
    for (uint16_t i = 0; i < _height; i++){_tiles[i]._tiles[col] = src[i];}
}

void TileMatrix::copyRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t src[]){
    if (x >= _width) {throw std::invalid_argument("[TileMatrix::copyRect]: x is out of bounds");}
    if (y >= _height) {throw std::invalid_argument("[TileMatrix::copyRect]: y is out of bounds");}
    if (width+x > _width) {throw std::invalid_argument("[TileMatrix::copyRect]: width+x is out of bounds");}
    if (height+y > _height) {throw std::invalid_argument("[TileMatrix::copyRect]: height+y is out of bounds");}
    uint32_t ptr = 0;
    for (uint16_t i = y; i < height+y; i++){
        for (uint16_t j = x; j < width+x; j++){
            _tiles[i]._tiles[j] = src[ptr++];
        }
    }
}

void TileMatrix::copyRect(uint16_t out_x, uint16_t out_y, uint16_t width, uint16_t height, TileMatrix *src, uint16_t in_x, uint16_t in_y){

    #pragma region errorHandling
    if (in_x >= src->getWidth()) {throw std::invalid_argument("[TileMatrix::copyRect]: x is out of bounds (source)");}
    if (in_y >= src->getHeight()) {throw std::invalid_argument("[TileMatrix::copyRect]: y is out of bounds (source)");}
    if (width+in_x > src->getWidth()) {throw std::invalid_argument("[TileMatrix::copyRect]: width+x is out of bounds (source)");}
    if (height+in_y > src->getHeight()) {throw std::invalid_argument("[TileMatrix::copyRect]: height+y is out of bounds (source)");}

    if (out_x >= _width) {throw std::invalid_argument("[TileMatrix::copyRect]: x is out of bounds (destination)");}
    if (out_y >= _height) {throw std::invalid_argument("[TileMatrix::copyRect]: y is out of bounds (destination)");}
    if (width+out_x > _width) {throw std::invalid_argument("[TileMatrix::copyRect]: width+x is out of bounds (destination)");}
    if (height+out_y > _height) {throw std::invalid_argument("[TileMatrix::copyRect]: height+y is out of bounds (destination)");}
    #pragma endregion

    for (uint16_t i = 0; i < height; i++){
        for (uint16_t j = 0; j < width; j++){
            _tiles[out_y+i]._tiles[out_x+j] = src->_tiles[in_y+i]._tiles[in_x+j];
        }
    }
}

#pragma endregion
#pragma region rendering

void TileMatrix::render(uint16_t x, uint16_t y, sf::RenderWindow *window, sf::Texture texture){
    sf::Vector2f texturePos {0, 0};
    uint8_t flip_palette;
    sf::Color color;
    for (uint16_t i = 0; i < _height && i*8+y < window->getSize().y; i++){
        for (uint16_t j = 0; j < _width && j*8+x < window->getSize().x; j++){
            texturePos.y = (_tiles[i]._tiles[j]) << 3;
            flip_palette = _tiles[i]._flip_palette[j];
            color = sf::Color(
                flip_palette&REDMASK?255:0,
                flip_palette&GRNMASK?255:0,
                flip_palette&BLUMASK?255:0);
            sf::Vertex vertices[4] = {
                sf::Vertex(sf::Vector2f(j*8, i*8), color, texturePos+sf::Vector2f(flip_palette&HFLIP?8:0,flip_palette&VFLIP?8:0)),
                sf::Vertex(sf::Vector2f(j*8+8, i*8), color, texturePos+sf::Vector2f(flip_palette&HFLIP?0:8,flip_palette&VFLIP?8:0)),
                sf::Vertex(sf::Vector2f(j*8+8, i*8+8), color, texturePos+sf::Vector2f(flip_palette&HFLIP?0:8,flip_palette&VFLIP?0:8)),
                sf::Vertex(sf::Vector2f(j*8, i*8+8), color, texturePos+sf::Vector2f(flip_palette&HFLIP?8:0,flip_palette&VFLIP?0:8))
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
    target.create(_width*8, _height*8);
    for (uint16_t i = 0; i < _height; i++){
        uint16_t y = _height - i - 1;
        for (uint16_t j = 0; j < _width; j++){
            texturePos.y = (_tiles[i]._tiles[j]) << 3;
            flip_palette = _tiles[i]._flip_palette[j];
            color = sf::Color(
                flip_palette&REDMASK?255:0,
                flip_palette&GRNMASK?255:0,
                flip_palette&BLUMASK?255:0);
            sf::Vertex vertices[4] = {
                sf::Vertex(sf::Vector2f(j*8, y*8+8), color, texturePos+sf::Vector2f(flip_palette&HFLIP?8:0,flip_palette&VFLIP?8:0)),
                sf::Vertex(sf::Vector2f(j*8+8, y*8+8), color, texturePos+sf::Vector2f(flip_palette&HFLIP?0:8,flip_palette&VFLIP?8:0)),
                sf::Vertex(sf::Vector2f(j*8+8, y*8), color, texturePos+sf::Vector2f(flip_palette&HFLIP?0:8,flip_palette&VFLIP?0:8)),
                sf::Vertex(sf::Vector2f(j*8, y*8), color, texturePos+sf::Vector2f(flip_palette&HFLIP?8:0,flip_palette&VFLIP?0:8))
            };
            target.draw(vertices, 4, sf::TriangleFan, sf::RenderStates(&texture));
        }
    }
    return target.getTexture();
}

#pragma endregion