#pragma once
#include "configuration.hpp"
#include <unordered_map>

struct HashGrid
{
	private:
		std::unordered_map< uint32_t, std::vector<Particle>> hashMap;

	public:
		HashGrid();
		void clearGrid();
		void mapParticlesToCell(std::vector<Particle>& particles);
		std::vector<Particle> getContentOfCell(uint32_t hash);
		uint32_t getHashFromPos(sf::Vector2f pos);
};

HashGrid::HashGrid()
{
	hashMap.reserve(conf::hashMapSize);
}

uint32_t HashGrid::getHashFromPos(sf::Vector2f pos)
{
	uint32_t x = pos.x / conf::cellSize;
	uint32_t y = pos.y / conf::cellSize;

	//uint32_t hash = (uint64_t) std::pow(x * conf::prime_x, y * conf::prime_y) % conf::hashMapSize;
	uint32_t hash = (x * conf::prime_x + y * conf::prime_y) % conf::hashMapSize;

	return hash;
}

void HashGrid::mapParticlesToCell(std::vector<Particle>& particles)
{
	for (uint32_t i{ conf::n_particles }; i--; )
	{
		uint32_t hash = getHashFromPos(particles[i].getPosition());
		
		hashMap[hash].push_back(particles[i]);
	}
}

std::vector<Particle> HashGrid::getContentOfCell(uint32_t hash)
{
	auto it = hashMap.find(hash);

	if (it != hashMap.end())
		return it->second;

	return {};
}

void HashGrid::clearGrid()
{
	hashMap.clear();
}