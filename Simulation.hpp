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
    int count = 0;
};

Simulation::Simulation() : 
    mWindow(sf::VideoMode(conf::window_size.x, conf::window_size.y), "SPH2d-Toy", sf::Style::Fullscreen), particle()
{
    std::cout << "TimePerFrame: " << TimePerFrame.asSeconds() << std::endl;
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
            update(sf::seconds(conf::tau));
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

    particle.shape.setPosition(particle.getPosition());
    mWindow.draw(particle.shape);

    if (count % 1000 == 0)
    {
        float speed = particle.getVelocityMagnitude();
        float energy = conf::m_particle * (0.5 * speed * speed - conf::g * particle.getPosition().y);
        std::cout << energy << std::endl;
        count = 0;
    }
    count++;
    

    mWindow.display();
}
