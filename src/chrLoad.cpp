#include <fstream>
#include <string>
#include <cstdio>
#include <iomanip>
#include <locale>
#include <algorithm>

#include "Tile.hpp"
#include <SFML/Graphics.hpp>

#ifndef __CHRLOAD_INCLUDED__
#define __CHRLOAD_INCLUDED__

// utility wrapper to adapt locale-bound facets for wstring/wbuffer convert
template<class Facet>
struct deletable_facet : Facet
{
    template<class... Args>
    deletable_facet(Args&&... args) : Facet(std::forward<Args>(args)...) {}
    ~deletable_facet() {}
};

std::u32string To_UTF32(const std::string &s)
{
    std::wstring_convert<deletable_facet<std::codecvt<char32_t, char, std::mbstate_t>>, char32_t> conv;
    return conv.from_bytes(s);
}

sf::Texture renderText(sf::Texture font, std::string string){
    std::u32string text = To_UTF32(string);
    uint8_t bank;
    TileMatrix matrix(text.length(), 1);

    for (uint32_t i = 0; i < text.length(); i++){
        //bank = std::find(text[i]&0xFFFFFF80);
        
        
    }
    return matrix.renderToTexture(font);
}

sf::VertexArray createTile(sf::Vector2i position, uint32_t tile){
    sf::VertexArray output (sf::TriangleFan);
    output.append(sf::Vertex(sf::Vector2f(position.x, position.y), sf::Vector2f(0, tile*8)));
    output.append(sf::Vertex(sf::Vector2f(position.x+8, position.y), sf::Vector2f(8, tile*8)));
    output.append(sf::Vertex(sf::Vector2f(position.x+8, position.y+8), sf::Vector2f(8, tile*8+8)));
    output.append(sf::Vertex(sf::Vector2f(position.x, position.y+8), sf::Vector2f(0, tile*8+8)));

    return output;
}

#endif
