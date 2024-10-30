#pragma once
#include <SFML/Graphics.hpp>
#include "configuration.hpp"
#include "Particle.hpp"
#include "hashGrid.hpp"
#include "GlobalInteraction.hpp"
#include "WKernel.hpp"
#include <sstream>

class Simulation
{
public:
    Simulation();
    void run();

private:
    void processEvents();
    void update(sf::Time);
    void render();
    void highlightNeighborSearch();
    void calculateTotalEnergy();

private:
    sf::RenderWindow mWindow;
    sf::Time TimePerFrame = sf::seconds(conf::dt);
    std::vector<Particle> particles;
    WKernel kernel;
    HashGrid hashGrid;
    sf::Text text;
    std::ostringstream oss;

    float tUpdate = 0.f;
    float tRender = 0.f;
};

Simulation::Simulation() : 
    mWindow(sf::VideoMode(conf::window_size.x, conf::window_size.y), "SPH2d-Toy", sf::Style::Fullscreen), particles()
{
    std::cout << "TimePerFrame: " << TimePerFrame.asMicroseconds() << std::endl;
    particles = createParticles(conf::n_particles);

    static sf::Font font;
    static bool fontLoaded = false;
    if (!fontLoaded) {
        if (!font.loadFromFile("arial.ttf")) {
            std::cerr << "Error: No se pudo cargar la fuente." << std::endl;
            return;
        }
        fontLoaded = true;
    }
    text.setFont(font);
    text.setCharacterSize(30);
    text.setFillColor(sf::Color::White);
}

void Simulation::run()
{
    mWindow.setMouseCursorVisible(true);
    sf::Clock clock;
    sf::Clock clockUpdate;
    sf::Clock clockRender;
    sf::Time timeSinceLastUpdate = sf::Time::Zero;

    while (mWindow.isOpen())
    {
        processEvents();
        timeSinceLastUpdate += clock.restart();
        while (timeSinceLastUpdate > TimePerFrame)
        {
            timeSinceLastUpdate -= TimePerFrame;
            processEvents();
            clockUpdate.restart();
            update(sf::seconds(conf::tau));

            tUpdate = clockUpdate.restart().asMicroseconds();
        }
        clockRender.restart();
        render();
        tRender = clockRender.restart().asMicroseconds();
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
    // Change CollisionHandler here

    GlobalInteraction<GridDetector<float>, DensityCalculator, float> densityCalculator;

    std::vector<float> densities = densityCalculator.handleInteraction(particles);

    for (uint32_t i{ conf::n_particles }; i--; )
    {
        particles[i].setDensityAndPressure(densities[i]);
    }

    GlobalInteraction<GridDetector<sf::Vector2f>, SPH, sf::Vector2f> collisionHandler;

    /*sf::Clock clockSPH;
    clockSPH.restart();*/

    std::vector<sf::Vector2f> f_collisions = collisionHandler.handleInteraction(particles);

    //std::cout << clockSPH.restart().asMicroseconds() << std::endl;

    sf::Vector2f f_grav = { 0.f, conf::m_particle * conf::g };

    for (uint32_t i{ conf::n_particles }; i--; )
    {
        sf::Vector2f f_air = -1.f * conf::beta * particles[i].getVelocity();
        //sf::Vector2f f_air{ 0.f, 0.f };

        particles[i].updateParticle(deltaTime, f_collisions[i], f_air + f_grav);
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

    highlightNeighborSearch();

    calculateTotalEnergy();

    oss << "Tiempo de update: " << tUpdate << " ms" << std::endl;
    oss << "Tiempo de render: " << tRender << " ms" << std::endl;

    text.setString(oss.str());
    mWindow.draw(text);
    oss.str("");

    mWindow.display();
}

void Simulation::highlightNeighborSearch()
{
    hashGrid.clearGrid();
    hashGrid.mapParticlesToCell(particles);

    sf::Vector2f mousePos = (sf::Vector2f) sf::Mouse::getPosition(mWindow);

    uint32_t mouseHash = hashGrid.getHashFromPos(mousePos);

    std::vector<uint32_t> idxs = hashGrid.getContentOfCell(mouseHash);

    std::vector<Particle> cellParticles(idxs.size());

    for ( int i=0; i<idxs.size(); i++ )
    {
        cellParticles[i] = particles[idxs[i]];
    }

    for (auto& particle : cellParticles)
    {
        particle.shape.setPosition(particle.getPosition());
        particle.shape.setFillColor(sf::Color::Magenta);
        mWindow.draw(particle.shape);
    }

    sf::RectangleShape cellBorder;

    float x_border = mousePos.x - (int) mousePos.x % conf::cellSize;
    float y_border = mousePos.y - (int) mousePos.y % conf::cellSize;

    cellBorder.setPosition(x_border, y_border);

    cellBorder.setOutlineColor(sf::Color::Magenta);
    cellBorder.setOutlineThickness(3.f);
    
    cellBorder.setSize({ conf::cellSize, conf::cellSize });
    cellBorder.setFillColor(sf::Color::Transparent);
    mWindow.draw(cellBorder);

    //oss << "Mouse Hash: " << mouseHash << std::endl;
}

void Simulation::calculateTotalEnergy()
{
    float energy = 0.f;

    for (uint32_t i{ conf::n_particles }; i--;)
    {
        float speed = particles[i].getVelocityMagnitude();
        energy += conf::m_particle * (0.5 * speed * speed - conf::g * (particles[i].getPosition().y - conf::window_size_f.y));
    }

    oss << "Total Energy: " << energy << std::endl;
}