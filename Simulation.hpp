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
    std::vector<Particle> particles;
    int count = 0;
};

Simulation::Simulation() : 
    mWindow(sf::VideoMode(conf::window_size.x, conf::window_size.y), "SPH2d-Toy", sf::Style::Fullscreen), particles()
{
    //std::cout << "TimePerFrame: " << TimePerFrame.asSeconds() << std::endl;
    particles = createParticles(conf::n_particles);
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
        //std::cout << timeSinceLastUpdate.asMilliseconds() << std::endl;
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
    // Creo que la idea va a ser calcular el vector de fuerzas de collision para cada particula y pasarlo como argumento para que se haga el update particle
    
    std::vector<sf::Vector2f> f_collisions(conf::n_particles, sf::Vector2f(0.0f, 0.0f));

    for (uint32_t i{ conf::n_particles }; i--; )
    {
        for (uint32_t j = i; j--; )
        {
            float d_ij = distance(particles[j].getPosition(), particles[i].getPosition());
            
            if (d_ij < 2.f * conf::h)
            {
                sf::Vector2f u_ij = (particles[j].getPosition() - particles[i].getPosition()) / d_ij;
                sf::Vector2f f_collision_i = -1.f *conf::k * (2.f * conf::h - d_ij) * u_ij;
                
                f_collisions[i] += f_collision_i;
                f_collisions[j] -= f_collision_i;
            }
        }
    }

    for (uint32_t i{ conf::n_particles }; i--; )
    {
        particles[i].updateParticle(deltaTime, f_collisions[i]);
    }
}

void Simulation::render()
{
    mWindow.clear();

    for (uint32_t i{ conf::n_particles }; i--; )
    {
        particles[i].shape.setPosition(particles[i].getPosition());
        mWindow.draw(particles[i].shape);
    }
 

    if (count % 300 == 0)
    {
        float energy = 0.f;
        for (uint32_t i{ conf::n_particles }; i--;)
        {
            float speed = particles[i].getVelocityMagnitude();
            energy += conf::m_particle * (0.5 * speed * speed - conf::g * (particles[i].getPosition().y - conf::window_size_f.y));
        }
        std::cout << energy << std::endl;
        count = 0;
    }
    count++;
    

    mWindow.display();
}

