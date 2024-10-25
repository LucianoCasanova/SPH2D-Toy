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
	float const H = window_size_f.x;
	float const v_max = std::sqrt(2.f * g * H) + std::sqrt(2.f)*v_lineal_max;
	float const tau = 0.2 * h / v_max; //10th of a diameter? 0.2 * 
	float const beta = 7.f; // Drag coefficient
	float const alpha = 0.5; // Inelastic coefficient
	float const k = 5000.f; //Stiffness coefficient
}

float distance(const sf::Vector2f& v1, const sf::Vector2f& v2) {
	sf::Vector2f diff = v1 - v2;
	return std::sqrt(diff.x * diff.x + diff.y * diff.y);
}