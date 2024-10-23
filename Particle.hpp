#pragma once
#include "configuration.hpp"
#include <random>

struct Particle
{
	private:
		sf::Vector2f r = { conf::window_size_f.x/2, conf::window_size_f.y/2 };
		sf::Vector2f v = { 0.f, -100.f } ;
		sf::Vector2f a = { 0.f, conf::g };
		float const m = conf::m_particle;

	private:
		void handleWallCollisions(sf::Time deltaTime);
	
	public:
		sf::CircleShape shape;

	public:
		Particle();
		void updateParticle(sf::Time deltaTime);
		sf::Vector2f getPosition();
		void setPosition(sf::Vector2f pos);
		sf::Vector2f getVelocity();
		void setVelocity(sf::Vector2f vel);
		float getVelocityMagnitude();
};

Particle::Particle()
{
	shape.setRadius(conf::h);
	shape.setPosition(r);
	shape.setFillColor(sf::Color::Blue); // Check why the following doesn't work
	//shape.setFillColor(conf::particle_color); 
}

void Particle::updateParticle(sf::Time deltaTime)
{
	// No friction
	 //sf::Vector2f f_air = { 0, 0 };

	// With friction
	sf::Vector2f f_air = -1.f * conf::beta * v;

	sf::Vector2f f_grav = { 0.f, m*conf::g };

	a = 1.f / m * (f_air + f_grav);

	// Explicit Euler Method

	handleWallCollisions(deltaTime);
}

void Particle::handleWallCollisions(sf::Time deltaTime) //Optimizar
{
	sf::Vector2f next_v = v + a * deltaTime.asSeconds();
	sf::Vector2f next_r = r + v * deltaTime.asSeconds();

	bool horizontal = r.x > 0 && r.x < conf::window_size_f.x;
	bool vertical = r.y > 0 && r.y < conf::window_size_f.y;

	if (horizontal && vertical)
	{
		v = next_v;
		r = next_r;
	}
	else 
	{
		if (!vertical)
		{
			next_v.y *= -1.f;
			next_r.y = (next_r.y < 0) ? -1.f * next_r.y : 2.f * conf::window_size_f.y - next_r.y;
		}
		if (!horizontal)
		{
			next_v.x *= -1.f;
			next_r.x = (next_r.x < 0) ? -1.f * next_r.x : 2.f * conf::window_size_f.x - next_r.x;
		}
		v = conf::alpha * next_v;
		r = next_r;
	}
}

std::vector<Particle> createParticles(uint32_t count)
{
	std::vector<Particle> particles;
	particles.reserve(count);

	//Random numbers generator
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dis(0.0f, 1.0f);

	// Create randomly distributed particles on the screen
	for (uint32_t i{ count }; i--;)
	{
		float const rx = dis(gen) * conf::window_size_f.x;
		float const ry = dis(gen) * conf::window_size_f.y;

		float const vx = dis(gen) * 200.f - 100.f;
		float const vy = dis(gen) * 200.f - 100.f;

		Particle particle;
		particle.setPosition({ rx,ry });
		particle.setVelocity({ vx,vy });

		particles.push_back({ particle });
	}
	return particles;
}

sf::Vector2f Particle::getPosition()
{
	return r;
}

sf::Vector2f Particle::getVelocity()
{
	return v;
}

float Particle::getVelocityMagnitude()
{
	return std::sqrt(v.x * v.x + v.y * v.y);
}

void Particle::setPosition(sf::Vector2f pos)
{
	r = pos;
}

void Particle::setVelocity(sf::Vector2f vel)
{
	v = vel;
}