#include <fstream>
#include <string>
#include <cstdio>
#include <iomanip>
#include <locale>
#include <algorithm>

#include "ChrFont.hpp"
#include "Tile.hpp"
#include <SFML/Graphics.hpp>

#ifndef __CHRLOAD_INCLUDED__
#define __CHRLOAD_INCLUDED__

sf::VertexArray createTile(sf::Vector2i position, uint32_t tile){
    sf::VertexArray output (sf::TriangleFan);
    output.append(sf::Vertex(sf::Vector2f(position.x, position.y), sf::Vector2f(0, tile*8)));
    output.append(sf::Vertex(sf::Vector2f(position.x+8, position.y), sf::Vector2f(8, tile*8)));
    output.append(sf::Vertex(sf::Vector2f(position.x+8, position.y+8), sf::Vector2f(8, tile*8+8)));
    output.append(sf::Vertex(sf::Vector2f(position.x, position.y+8), sf::Vector2f(0, tile*8+8)));

    return output;
}

#endif
