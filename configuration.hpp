#pragma once

namespace conf
{
	// Window configuration
	sf::Vector2u const window_size = { 1920, 1080 };
	sf::Vector2f const window_size_f = static_cast<sf::Vector2f>(window_size);
	uint32_t const max_framerate = 60;
	float const dt = 1.0f / static_cast<float>(max_framerate);

	// Physical parameters
	float const g = 9.8;
	float const H = window_size_f.x;
	float const v_max = std::sqrt(2.f * g * H);

	// Particle configuration
	float const m_particle = 10.f;
	float const radius = 30.f;
	sf::Color particle_color = sf::Color::Blue;

}
