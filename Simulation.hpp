#pragma once
#include <SFML/Graphics.hpp>
#include "configuration.hpp"
#include "Particle.hpp"

class Simulation
{
public:
    Simulation();
    void run();

private:
    void processEvents();
    void update(sf::Time);
    void render();

private:
    sf::RenderWindow mWindow;
    sf::Time TimePerFrame = sf::seconds(conf::dt);
    Particle particle;
};

Simulation::Simulation() : 
    mWindow(sf::VideoMode(conf::window_size.x, conf::window_size.y), "SPH2d-Toy", sf::Style::Fullscreen), particle()
{
}

void Simulation::run()
{
    mWindow.setMouseCursorVisible(false);
    sf::Clock clock;
    sf::Time timeSinceLastUpdate = sf::Time::Zero;
    while (mWindow.isOpen())
    {
        processEvents();
        timeSinceLastUpdate += clock.restart();
        while (timeSinceLastUpdate > TimePerFrame)
        {
            timeSinceLastUpdate -= TimePerFrame;
            processEvents();
            update(TimePerFrame);
        }
        render();
    }
}

void Simulation::processEvents()
{
    sf::Event event;
    while (mWindow.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
            mWindow.close();
        if (event.type == sf::Event::KeyPressed)
            if(event.key.code == sf::Keyboard::Escape)
                mWindow.close();
    }
}

void Simulation::update(sf::Time deltaTime)
{
    particle.updateParticle(deltaTime);
}

void Simulation::render()
{
    mWindow.clear();
    mWindow.draw(particle.shape);
    mWindow.display();
}
