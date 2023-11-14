#include <SFML/System.hpp>
#include <memory>

#include "Bezier.cpp"

#ifndef __MODULAR_SYNTH_INCLUDED__
#define __MODULAR_SYNTH_INCLUDED__

class ModSynthBezier : public CubicBezier {
    public:
        void updatePosition(std::array<sf::Vector2f, 2> & position);
        void updatePosition(sf::Vector2f & position, bool end);
        void calculate(float precision, float lineWidth, bool thin);

    private:
        std::array<sf::Vector2f, 2> position;
};

class ModSynthElement : public sf::Drawable {
    public:
        ModSynthElement ();

        void connectOutputBezier(ModSynthBezier & bezier);

    private:
        virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const override; 

        virtual void calculate();

        std::vector<std::weak_ptr<ModSynthBezier>> outputs;
};



void ModSynthBezier::updatePosition(std::array<sf::Vector2f, 2> & position) {
    this->position = position;
}

void ModSynthBezier::updatePosition(sf::Vector2f & position, bool end) {
    this->position[end] = position;
}

void ModSynthBezier::calculate(float precision, float lineWidth, bool thin) {
    if (lineWidth <= 0) return;
    // Get 4 points from the 2 provided
    // Four types of connections:
    if (position[0] == position[1]) {
        // Type 1: Directly on the exact same point
        #ifdef BEZIER_DEBUG
        center = sf::VertexArray();
        lines = sf::VertexArray();
        #endif
        vertices = sf::VertexArray(sf::Points);
        vertices.append(position[0]);
    } else if (position[0].x == position[1].x || position[0].y == position[1].y) {
        // Type 2: Straight line
        #ifdef BEZIER_DEBUG
        center = sf::VertexArray();
        lines = sf::VertexArray(sf::Lines);
        #endif
        if (thin){
            vertices = sf::VertexArray(sf::Lines);

            vertices.append(sf::Vertex(position[0], color));
            vertices.append(sf::Vertex(position[1], color));
        } else {
            vertices = sf::VertexArray(sf::TriangleStrip);

            sf::Vector2f velocity = position[1] - position[0];
            velocity /= lengthOfLine(position[0], position[1]);
            velocity *= (lineWidth / 2);
            velocity = perpendiculate(velocity);

            vertices.append(sf::Vertex(position[0]+velocity, color));
            vertices.append(sf::Vertex(position[0]-velocity, color));
            vertices.append(sf::Vertex(position[1]+velocity, color));
            vertices.append(sf::Vertex(position[1]-velocity, color));
            #ifdef BEZIER_DEBUG
            center.append(sf::Vertex(position[0], sf::Color::Red));
            center.append(sf::Vertex(position[1], sf::Color::Red));
            lines.append(sf::Vertex(position[0], sf::Color::Red));
            lines.append(sf::Vertex(position[1], sf::Color::Red));
            #endif
        }
        
    } else {
        // Types 3 and 4 consist of 3 lines around which a bezier curve is built

        std::array<sf::Vector2f, 4> points;
        points[0] = position[0];
        points[3] = position[1];

        if (position[0].x > position[1].x) {
            // Type 3: 2 horizontal, 1 vertical line

            //  +--E                  .-E
            //   \      Creates a     |
            //    \     bezier:       |
            //  S--+                S-'

            // (also works upside down)

            points[1] = sf::Vector2f(position[1].x, position[0].y);
            points[2] = sf::Vector2f(position[0].x, position[1].y);
        } else {
            // Type 4: 1 horizontal, 2 vertical lines

            //  +  S                   S
            //  |\ |    Creates a    __|
            //  | \|    bezier:     |
            //  E  +                E

            // (also works upside down)

            points[1] = sf::Vector2f(position[0].x, position[1].y);
            points[2] = sf::Vector2f(position[1].x, position[0].y);
        }

        CubicBezier::calculate(points, precision, lineWidth, thin);
    }
}

void ModSynthElement::connectOutputBezier(ModSynthBezier & bezier) {
    outputs.push_back(std::make_shared<ModSynthBezier>(bezier));
}


#endif  // __MODULAR_SYNTH_INCLUDED__