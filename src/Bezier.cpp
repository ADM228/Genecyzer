#include "Utils.cpp"
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <array>
#include <cmath>

#ifndef __BEZIER_INCLUDED__
#define __BEZIER_INCLUDED__

// class Bezier : public sf::Drawable
// {
//     public:
//         void calculate
// }

class CubicBezier : public sf::Drawable
{
    public:
        void calculate (std::array<sf::Vector2f, 4>&, float precision, float lineWidth, bool thin);
        void setColor(sf::Color color) { this->color = color; };
    protected:
        sf::Color color;
        sf::VertexArray vertices;

        #ifdef BEZIER_DEBUG
        sf::VertexArray lines;
        sf::VertexArray center;
        #endif
    private:
        void calculateThin (std::array<sf::Vector2f, 4>&, float precision);
        virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const override;
};

void CubicBezier::calculateThin(std::array<sf::Vector2f, 4>& points, float precision) {
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
    precision = std::min(precision, 512.f);
    int limit = static_cast<int>(precision+1);
    precision = 1.f/precision;
    float precision2 = precision*precision;
    float precision3 = precision2*precision;

    // pol0 is just points[0]
    sf::Vector2f pol1 = points[0] *-3 + points[1] * 3;
    sf::Vector2f pol2 = points[0] * 3 + points[1] *-6 + points[2] * 3;
    sf::Vector2f pol3 = points[0] *-1 + points[1] * 3 + points[2] *-3 + points[3];

    for (int f = 0; f <= limit; f++) {
        float t = std::min(precision*f, 1.f), t2 = std::min(precision2*(f*f), 1.f), t3 = std::min(precision3*(f*f*f), 1.f);
        vertices.append(sf::Vertex(points[0] + t * pol1 + t2 * pol2 + t3 * pol3, color));
    }
}

void CubicBezier::calculate(std::array<sf::Vector2f, 4>& points, float precision, float lineWidth, bool thin) {
    if (lineWidth <= 0) return;
    if (thin) { calculateThin(points, precision); return; }
    if (precision <= 0) precision = 1.f/1024;
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
    precision = std::min(precision, 512.f);
    int limit = static_cast<int>(precision+1);
    precision = 1.f/precision;
    float precision2 = precision*precision;
    float precision3 = precision2*precision;

    sf::Vector2f prevPoint = points[0], velocity;

    // pol0 is just points[0]
    sf::Vector2f pol1 = points[0] *-3 + points[1] * 3;
    sf::Vector2f pol2 = points[0] * 3 + points[1] *-6 + points[2] * 3;
    sf::Vector2f pol3 = points[0] *-1 + points[1] * 3 + points[2] *-3 + points[3];

    for (int f = 0; f <= limit; f++) {
        float t = std::min(precision*f, 1.f), t2 = std::min(precision2*(f*f), 1.f), t3 = std::min(precision3*(f*f*f), 1.f);
        auto point = points[0] + t * pol1 + t2 * pol2 + t3 * pol3;    // Center point
        velocity = point - prevPoint;
        velocity /= lengthOfVelocity(velocity);
        velocity *= lineWidth;
        velocity = perpendiculate(velocity);
        vertices.append(sf::Vertex(prevPoint+velocity, color));
        vertices.append(sf::Vertex(prevPoint-velocity, color));
        prevPoint = point;
        #ifdef BEZIER_DEBUG
        center.append(sf::Vertex(point, sf::Color::Red));
        #endif
    }
    vertices.append(sf::Vertex(prevPoint+velocity, color));
    vertices.append(sf::Vertex(prevPoint-velocity, color));
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