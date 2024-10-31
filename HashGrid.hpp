#pragma once
#include "configuration.hpp"
#include <unordered_map>

struct HashGrid
{
	private:
		std::unordered_map< uint32_t, std::vector<uint32_t>> hashMap;

	public:
		HashGrid();
		void clearGrid();
		void mapParticlesToCell(const std::vector<Particle>& particles);
		std::vector<uint32_t> getContentOfCell(uint32_t hash);
		uint32_t getHashFromPos(const sf::Vector2f& pos);
		//std::vector<uint32_t> getNeighborsHash(uint32_t hash, const sf::Vector2f& probeParticlePos);
		std::vector<uint32_t> getListOfHash();
};

HashGrid::HashGrid()
{
	hashMap.reserve(conf::hashMapSize);
}

uint32_t HashGrid::getHashFromPos(const sf::Vector2f& pos)
{
	uint32_t x = pos.x / conf::cellSize;
	uint32_t y = pos.y / conf::cellSize;

	//uint32_t hash = (uint64_t) std::pow(x * conf::prime_x, y * conf::prime_y) % conf::hashMapSize;
	//uint32_t hash = (x * conf::prime_x + y * conf::prime_y) % conf::hashMapSize;

	uint32_t hash = x + conf::n_collumns * y;

	return hash;
}

void HashGrid::mapParticlesToCell(const std::vector<Particle>& particles)
{
	for (uint32_t i{ conf::n_particles }; i--; )
	{
		uint32_t hash = getHashFromPos(particles[i].getPosition());
		
		hashMap[hash].push_back(i); // [] operator creates a new empty entry if key hash did not exist
	}
}

std::vector<uint32_t> HashGrid::getContentOfCell(uint32_t hash)
{
	auto it = hashMap.find(hash);

	if (it != hashMap.end())
		return it->second;

	return {};
}

std::vector<uint32_t> HashGrid::getListOfHash() {
	
	std::vector<uint32_t> hashes;

	for (const auto& pair : hashMap)
	{
		hashes.push_back(pair.first);
	}

	return hashes;
}

void HashGrid::clearGrid()
{
	hashMap.clear();
}