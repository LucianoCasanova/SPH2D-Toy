#pragma once
#include "configuration.hpp"
#include <random>

class Simulation;

struct Particle
{
	private:
		sf::Vector2f r = { conf::window_size_f.x/2, conf::window_size_f.y/2 };
		sf::Vector2f v = { 0.f, 0.f } ;
		sf::Vector2f a = { 0.f, conf::g };
		float const m = conf::m_particle;
		float rho = 1;
		float P = 1;

	private:
		void handleWallCollisions(sf::Time deltaTime, const float x_border);
	
	public:
		sf::CircleShape shape;

	public:
		Particle();
		Particle(const sf::Vector2f& pos);
		Particle(const sf::Vector2f& pos, const sf::Vector2f& vel);
		Particle& operator=(const Particle& other);
		void updateParticle(sf::Time deltaTime, const sf::Vector2f& f_interaction, const sf::Vector2f& f_external, const float x_border);
		void setDensityAndPressure(float new_rho);
		sf::Vector2f getPosition() const;
		sf::Vector2f getVelocity() const;
		float getVelocityMagnitude() const;
		float getDensity() const;
		float getPressure() const;
};

Particle::Particle()
{
	shape.setRadius(conf::h);
	shape.setOrigin(conf::h, conf::h);
	shape.setPosition(r);
	shape.setFillColor(sf::Color::Blue);
}
Particle::Particle(const sf::Vector2f& pos)
	: r(pos)
{
	shape.setRadius(conf::h);
	shape.setOrigin(conf::h, conf::h);
	shape.setPosition(r);
	shape.setFillColor(sf::Color::Blue); // Check why the following doesn't work
	//shape.setFillColor(conf::particle_color); 
}
Particle::Particle(const sf::Vector2f& pos,const sf::Vector2f& vel)
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

void Particle::updateParticle(sf::Time deltaTime, const sf::Vector2f& f_interaction, const sf::Vector2f& f_external, const float x_border)
{
	a = 1.f / m * (f_interaction + f_external);

	// Explicit Euler Method
	r += v * deltaTime.asSeconds();
	v += a * deltaTime.asSeconds();

	handleWallCollisions(deltaTime, x_border);
}

// Inellastic discrete

void Particle::handleWallCollisions(sf::Time deltaTime, const float x_border)
{

	if (r.x < conf::h && v.x < 0 || r.x > x_border - conf::h && v.x > 0)
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

//std::vector<Particle> createParticles(uint32_t count)
//{
//	std::vector<Particle> particles;
//	particles.reserve(count);
//
//	//Random numbers generator
//	std::random_device rd;
//	std::mt19937 gen(rd());
//	std::uniform_real_distribution<float> dis(0.0f, 1.0f);
//
//	// Create randomly distributed particles on the screen
//	for (uint32_t i{ count }; i--;)
//	{
//		float const rx = conf::h + dis(gen) * (conf::window_size_f.x - 2.f * conf::h);
//		float const ry = conf::h + dis(gen) * (conf::window_size_f.y - 2.f * conf::h);
//
//		float const vx = conf::v_lineal_max * ( dis(gen) * 2.f - 1.f );
//		float const vy = conf::v_lineal_max * ( dis(gen) * 2.f - 1.f );
//
//		Particle particle({ rx, ry }, { vx, vy });
//
//		particles.push_back({ particle });
//	}
//	return particles;
//}
std::vector<Particle> createParticles(uint32_t count)
{
	std::vector<Particle> particles;
	particles.reserve(count);

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dis(0.0f, 1.0f);

	float spacing = conf::spacing;
	int x_balls = conf::x_initialBox / spacing - 1;
	int y_balls = conf::n_particles / x_balls + 1;
	int x_balls_last = conf::n_particles % x_balls;

	for (uint32_t i=0; i < y_balls; i++)
	{
		if (i != y_balls - 1)
		{
			for (uint32_t j = 0; j < x_balls; j++)
			{
				float const rx = spacing * (1 + j);
				float const ry = conf::window_size.y - spacing * (1 + i);
				float const vx = conf::v_lineal_max * (dis(gen) * 2.f - 1.f);
				float const vy = conf::v_lineal_max/10.f * (dis(gen) * 2.f - 1.f);
				Particle particle({ rx, ry }, {vx, vy});
				particles.push_back({ particle });
			}
		}
		else
		{
			for (uint32_t j = 0; j < x_balls_last; j++)
			{
				float const rx = spacing * (1 + j);
				float const ry = conf::window_size.y - spacing * (1 + i);
				float const vx = conf::v_lineal_max * (dis(gen) * 2.f - 1.f);
				float const vy = conf::v_lineal_max / 10.f * (dis(gen) * 2.f - 1.f);
				Particle particle({ rx, ry }, { vx, vy });
				particles.push_back({ particle });
			}
		}
	}
	return particles;
}

sf::Vector2f Particle::getPosition() const
{
	return r;
}

sf::Vector2f Particle::getVelocity() const
{
	return v;
}

float Particle::getVelocityMagnitude() const
{
	return std::sqrt(v.x * v.x + v.y * v.y);
}

float Particle::getDensity() const
{
	return rho;
}

float Particle::getPressure() const
{
	return P;
}

void Particle::setDensityAndPressure(float new_rho)
{
	float S = 14.f / 30.f * 3.14159 * conf::h * conf::h;

	rho = new_rho +  2.f/( S * 3.f); // Le agrego la autodensidad, falta multiplicar por la masa
	P = conf::rho_0 * conf::v_max * conf::v_max / conf::gamma * (std::pow(rho / conf::rho_0, conf::gamma) - 1);
}