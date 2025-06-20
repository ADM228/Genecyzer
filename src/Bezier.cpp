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
    vertices = sf::VertexArray(sf::PrimitiveType::LineStrip);

    #ifdef BEZIER_DEBUG
        lines = sf::VertexArray(sf::PrimitiveType::LineStrip);
        center = sf::VertexArray();
        lines.append({points[0], sf::Color::Red});
        lines.append({points[1], sf::Color::Red});
        lines.append({points[2], sf::Color::Red});
        lines.append({points[3], sf::Color::Red});
    #endif

    precision *= (
        (points[0] - points[1]).length() + 
        (points[1] - points[2]).length() + 
        (points[2] - points[3]).length());

    precision = std::min(precision, 512.f);
    int limit = static_cast<int>(precision+1);
    precision = 1.f/precision;

    // pol0 is just points[0]
    sf::Vector2f pol1 = points[0] *-3.f + points[1] * 3.f;
    sf::Vector2f pol2 = points[0] * 3.f + points[1] *-6.f + points[2] * 3.f;
    sf::Vector2f pol3 = points[0] *-1.f + points[1] * 3.f + points[2] *-3.f + points[3];

    float t, t2, t3;
    for (int f = 0; f <= limit; f++) {
        t = precision*f;
        t2 = std::min(t*t, 1.f);
        t3 = std::min(t2*t, 1.f);
        t = std::min(t, 1.f);
        vertices.append(sf::Vertex{points[0] + t * pol1 + t2 * pol2 + t3 * pol3, color});
    }
}

void CubicBezier::calculate(std::array<sf::Vector2f, 4>& points, float precision, float lineWidth, bool thin) {
    if (lineWidth <= 0) return;
    if (thin) { calculateThin(points, precision); return; }
    if (precision <= 0) precision = 1.f/1024;
    lineWidth /= 2;
    vertices = sf::VertexArray(sf::PrimitiveType::TriangleStrip);

    #ifdef BEZIER_DEBUG
        lines = sf::VertexArray(sf::PrimitiveType::LineStrip);
        center = sf::VertexArray();
        lines.append({points[0], sf::Color::Red});
        lines.append({points[1], sf::Color::Red});
        lines.append({points[2], sf::Color::Red});
        lines.append({points[3], sf::Color::Red});
    #endif

    precision *= (
        (points[0] - points[1]).length() + 
        (points[1] - points[2]).length() + 
        (points[2] - points[3]).length());

    precision = std::min(precision, 512.f);
    int limit = static_cast<int>(precision+1);
    precision = 1.f/precision;

    sf::Vector2f prevPoint = points[0], velocity;

    // pol0 is just points[0]
    sf::Vector2f pol1 = points[0] *-3.f + points[1] * 3.f;
    sf::Vector2f pol2 = points[0] * 3.f + points[1] *-6.f + points[2] * 3.f;
    sf::Vector2f pol3 = points[0] *-1.f + points[1] * 3.f + points[2] *-3.f + points[3];

    float t, t2, t3;
    for (int f = 0; f <= limit; f++) {
        t = precision*f;
        t2 = std::min(t*t, 1.f);
        t3 = std::min(t2*t, 1.f);
        t = std::min(t, 1.f);

        auto point = points[0] + t * pol1 + t2 * pol2 + t3 * pol3;    // Center point
        velocity = point - prevPoint;
        if (velocity.length() != 0)
            velocity = velocity.normalized().perpendicular() * lineWidth;
        vertices.append(sf::Vertex{prevPoint+velocity, color});
        vertices.append(sf::Vertex{prevPoint-velocity, color});
        prevPoint = point;
        #ifdef BEZIER_DEBUG
        center.append(sf::Vertex{point, sf::Color::Red});
        #endif
    }
    vertices.append(sf::Vertex{prevPoint+velocity, color});
    vertices.append(sf::Vertex{prevPoint-velocity, color});
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