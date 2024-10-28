#pragma once

namespace conf
{
	// Window configuration
	sf::Vector2u const window_size = { 1920, 1080 };
	sf::Vector2f const window_size_f = static_cast<sf::Vector2f>(window_size);
	uint32_t const max_framerate = 60;
	float const dt = 1.0f / static_cast<float>(max_framerate);

	// Particle configuration
	float const m_particle = 10.f;
	float const h = 10.f;
	sf::Color particle_color = sf::Color::Blue;
	uint32_t const n_particles = 670;
	float const v_lineal_max = 100.f;

	// Physical parameters
	float const g = 9.8;
	float const H = window_size_f.y;
	float const v_max = std::sqrt(2.f * g * H) + std::sqrt(2.f)*v_lineal_max;
	float const tau = 0.2 * h / v_max; // 10th of a diameter 
	float const beta = 10.f; // Drag coefficient
	float const alpha = 0.7; // Inelastic coefficient
	float const k = 5000.f; //Stiffness coefficient

	// Hash Grid parameteres
	uint32_t const cellSize = 250;
	//sf::Vector2u const numberOfCells = { window_size.x / cellSize, window_size.y / cellSize };
	uint32_t const hashMapSize = 100000; //At least larger than numberOfCells.x * numberOfCells.y, in video PixelPhysics uses this
	uint32_t const prime_x = 6614058611;
	uint32_t const prime_y = 7528850467;
}

float distance(const sf::Vector2f& v1, const sf::Vector2f& v2) {
	sf::Vector2f diff = v1 - v2;
	return std::sqrt(diff.x * diff.x + diff.y * diff.y);
}