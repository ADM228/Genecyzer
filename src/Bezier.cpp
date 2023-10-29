
#include "SFML/Graphics/Color.hpp"
#include "SFML/Graphics/PrimitiveType.hpp"
#include "SFML/Graphics/RenderStates.hpp"
#include "SFML/Graphics/RenderTarget.hpp"
#include "SFML/Graphics/VertexArray.hpp"
#include "SFML/System/Vector2.hpp"
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <array>
#include <cmath>
#include <pthread.h>

#ifndef __BEZIER_INCLUDED__
#define __BEZIER_INCLUDED__

// class Bezier : public sf::Drawable
// {
//     public:
//         void calculate
// }

sf::Vector2f operator*(sf::Vector2f a, float b) {
    return sf::Vector2f(a.x*b, a.y*b);
}

sf::Vector2f operator*(sf::Vector2f a, int b) {
    return sf::Vector2f(a.x*b, a.y*b);
}

sf::Vector2f operator/(sf::Vector2f a, float b) {
    return sf::Vector2f(a.x/b, a.y/b);
}

sf::Vector2f operator/(sf::Vector2f a, double b) {
    return sf::Vector2f(a.x/b, a.y/b);
}

sf::Vector2f operator/(sf::Vector2f a, int b) {
    return sf::Vector2f(a.x/b, a.y/b);
}

class CubicBezier : public sf::Drawable
{
    public:
        void calculate (std::array<sf::Vector2f, 4>&, float precision);
        void calculate (std::array<sf::Vector2f, 4>&, float precision, float lineWidth);
        void setColor(sf::Color color) { this->color = color; };
    private:
        virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

        sf::Color color;
        sf::VertexArray vertices;
        #ifdef BEZIER_DEBUG
        sf::VertexArray lines;
        sf::VertexArray center;
        #endif
};

void CubicBezier::calculate(std::array<sf::Vector2f, 4>& points, float precision) {
    vertices = sf::VertexArray(sf::LineStrip);
    #ifdef BEZIER_DEBUG
    lines = sf::VertexArray(sf::LineStrip);
    center = sf::VertexArray();
    lines.append(sf::Vertex(points[0], sf::Color::Red));
    lines.append(sf::Vertex(points[1], sf::Color::Red));
    lines.append(sf::Vertex(points[2], sf::Color::Red));
    lines.append(sf::Vertex(points[3], sf::Color::Red));
    #endif

    #define lengthOfLine(begin, end) std::sqrt(std::pow(end.x-begin.x, 2)+std::pow(end.y-begin.y, 2))

    precision *= (lengthOfLine(points[0], points[1]) + lengthOfLine(points[2], points[1]) + lengthOfLine(points[3], points[2]));
    precision = 1/precision;

    // pol0 is just points[0]
    sf::Vector2f pol1 = points[0] *-3 + points[1] * 3;
    sf::Vector2f pol2 = points[0] * 3 + points[1] *-6 + points[2] * 3;
    sf::Vector2f pol3 = points[0] *-1 + points[1] * 3 + points[2] *-3 + points[3];

    float limit = 1 + precision;

    for (float t = 0; t < limit; t += precision) {
        if (t > 1) {t = 1;}
        vertices.append(sf::Vertex(points[0] + t * pol1 + t*t * pol2 + t*t*t * pol3, color));
    }
}

void CubicBezier::calculate(std::array<sf::Vector2f, 4>& points, float precision, float lineWidth) {
    lineWidth /= 2;
    vertices = sf::VertexArray(sf::TriangleStrip);
    #ifdef BEZIER_DEBUG
    lines = sf::VertexArray(sf::LineStrip);
    center = sf::VertexArray();
    lines.append(sf::Vertex(points[0], sf::Color::Red));
    lines.append(sf::Vertex(points[1], sf::Color::Red));
    lines.append(sf::Vertex(points[2], sf::Color::Red));
    lines.append(sf::Vertex(points[3], sf::Color::Red));
    #endif

    #define lengthOfLine(begin, end) std::sqrt(std::pow(end.x-begin.x, 2)+std::pow(end.y-begin.y, 2))
    #define lengthOfVelocity(velocity) std::sqrt(std::pow(velocity.x, 2)+std::pow(velocity.y, 2))

    precision *= (lengthOfLine(points[0], points[1]) + lengthOfLine(points[1], points[2]) + lengthOfLine(points[2], points[3]));
    precision = 1/precision;

    sf::Vector2f prevPoint = points[0], velocity;

    // pol0 is just points[0]
    sf::Vector2f pol1 = points[0] *-3 + points[1] * 3;
    sf::Vector2f pol2 = points[0] * 3 + points[1] *-6 + points[2] * 3;
    sf::Vector2f pol3 = points[0] *-1 + points[1] * 3 + points[2] *-3 + points[3];

    float limit = 1+precision;

    for (float t = 0; t < limit; t += precision) {
        if (t > 1) {t = 1;}
        auto point = points[0] + t * pol1 + t*t * pol2 + t*t*t * pol3;    // Center point
        velocity = point - prevPoint;
        velocity = velocity / lengthOfVelocity(velocity);
        vertices.append(sf::Vertex(sf::Vector2f(prevPoint.x+velocity.y*lineWidth, prevPoint.y-velocity.x*lineWidth), color));
        vertices.append(sf::Vertex(sf::Vector2f(prevPoint.x-velocity.y*lineWidth, prevPoint.y+velocity.x*lineWidth), color));
        prevPoint = point;
        #ifdef BEZIER_DEBUG
        center.append(sf::Vertex(point, sf::Color::Red));
        #endif
    }
    vertices.append(sf::Vertex(sf::Vector2f(prevPoint.x+velocity.y*lineWidth, prevPoint.y-velocity.x*lineWidth), color));
    vertices.append(sf::Vertex(sf::Vector2f(prevPoint.x-velocity.y*lineWidth, prevPoint.y+velocity.x*lineWidth), color));
}

void CubicBezier::draw (sf::RenderTarget &target, sf::RenderStates states) const {
    #ifdef BEZIER_DEBUG
    target.draw(lines, states);
    #endif
    target.draw(vertices, states);
    #ifdef BEZIER_DEBUG
    if (center.getVertexCount() > 0) target.draw(center);
    #endif
}

#endif  // __BEZIER_INCLUDED__