#pragma once
#include "configuration.hpp"

struct Particle
{
	sf::Vector2f r = { conf::window_size_f.x/2, conf::window_size_f.y/2 };
	sf::Vector2f v = { 0.f, 100.f } ;
	sf::Vector2f a = { 0.f, 0.f };
	float const m = conf::m_particle;
	sf::CircleShape shape;

	Particle();

	void updateParticle(sf::Time deltaTime);
};

Particle::Particle()
{
	shape.setRadius(conf::radius);
	shape.setPosition(r);
	shape.setFillColor(sf::Color::Blue); // Check why the following doesn't work
	//shape.setFillColor(conf::particle_color); 
}

void Particle::updateParticle(sf::Time deltaTime)
{
	r += v * deltaTime.asSeconds();
	shape.setPosition(r);
};