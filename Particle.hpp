#pragma once
#include "configuration.hpp"
#include <random>

struct Particle
{
	private:
		sf::Vector2f r = { conf::window_size_f.x/2, conf::window_size_f.y/2 };
		sf::Vector2f v = { 0.f, 0.f } ;
		sf::Vector2f a = { 0.f, conf::g };
		float const m = conf::m_particle;

	private:
		void handleWallCollisions(sf::Time deltaTime);
	
	public:
		sf::CircleShape shape;

	public:
		Particle();
		Particle(sf::Vector2f pos, sf::Vector2f vel);
		Particle& operator=(const Particle& other);
		void updateParticle(sf::Time deltaTime, sf::Vector2f& f_collision);
		sf::Vector2f getPosition();
		sf::Vector2f getVelocity();
		float getVelocityMagnitude();
};

Particle::Particle()
{
	shape.setRadius(conf::h);
	shape.setOrigin(conf::h, conf::h);
	shape.setPosition(r);
	shape.setFillColor(sf::Color::Blue);
}

Particle::Particle(sf::Vector2f pos, sf::Vector2f vel)
        : r(pos), v(vel)
{
	shape.setRadius(conf::h);
	shape.setOrigin(conf::h , conf::h);
	shape.setPosition(r);
	shape.setFillColor(sf::Color::Blue); // Check why the following doesn't work
	//shape.setFillColor(conf::particle_color); 
}

Particle& Particle::operator=(const Particle& other)
{
	if (this != &other) {
		r = other.r;
	}
	return *this;
}

void Particle::updateParticle(sf::Time deltaTime, sf::Vector2f& f_collision)
{

	sf::Vector2f f_air = -1.f * conf::beta * v;
	sf::Vector2f f_grav = { 0.f, m*conf::g };

	a = 1.f / m * (f_air + f_grav + f_collision);

	// Explicit Euler Method

	handleWallCollisions(deltaTime);
}

// Inellastic discrete

void Particle::handleWallCollisions(sf::Time deltaTime)
{
	r += v * deltaTime.asSeconds();
	v += a * deltaTime.asSeconds();

	if (r.x < conf::h && v.x < 0 || r.x > conf::window_size_f.x - conf::h && v.x > 0)
	{
		v.x *= -1.f * conf::alpha;
		v.y *= conf::alpha;
	}
	if (r.y < conf::h && v.y < 0 || r.y > conf::window_size_f.y - conf::h && v.y > 0)
	{
		v.y *= -1.f * conf::alpha;
		v.x *= conf::alpha;
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
		float const rx = conf::h + dis(gen) * (conf::window_size_f.x - 2.f * conf::h);
		float const ry = conf::h + dis(gen) * (conf::window_size_f.y - 2.f * conf::h);

		float const vx = conf::v_lineal_max * ( dis(gen) * 2.f - 1.f );
		float const vy = conf::v_lineal_max * ( dis(gen) * 2.f - 1.f );

		Particle particle({ rx, ry }, { vx, vy });

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