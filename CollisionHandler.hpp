#pragma once
#include "configuration.hpp"
#include "Particle.hpp"
#include "hashGrid.hpp"

struct CollisionHandler
{
    public:

        virtual std::vector<sf::Vector2f> handleCollisions(std::vector<Particle>& particles) = 0;

    public:
        
        std::vector<sf::Vector2f> f_collisions{ conf::n_particles, sf::Vector2f(0.0f, 0.0f) };
};

struct NaiveCollisionHandler : public CollisionHandler
{
	std::vector<sf::Vector2f> handleCollisions(std::vector<Particle>& particles)
	{

        for (uint32_t i{ conf::n_particles }; i--; )
        {
            for (uint32_t j = i; j--; )
            {
                float d_ij = distance(particles[j].getPosition(), particles[i].getPosition());

                if (d_ij < 2.f * conf::h)
                {
                    sf::Vector2f u_ij = (particles[j].getPosition() - particles[i].getPosition()) / d_ij;
                    sf::Vector2f f_collision_i = -1.f * conf::k * (2.f * conf::h - d_ij) * u_ij;

                    f_collisions[i] += f_collision_i;
                    f_collisions[j] -= f_collision_i;
                }
            }
        }
        return f_collisions;
	}
};

struct GridCollisionHandler : public CollisionHandler
{
    private:
        
        HashGrid hashGrid;

    public:

        std::vector<sf::Vector2f> handleCollisions(std::vector<Particle>& particles)
        {

            hashGrid.clearGrid();
            hashGrid.mapParticlesToCell(particles);

            std::vector<uint32_t> hashes = hashGrid.getListOfHash();

            for (auto hash : hashes)
            {
                std::vector<uint32_t> cellIdxs = hashGrid.getContentOfCell(hash);

                sf::Vector2f probeParticlePos = particles[cellIdxs[0]].getPosition();
                std::vector<uint32_t> neighborsHashes = getNeighborsHash(hash, probeParticlePos);

                for (uint32_t i = (uint32_t) cellIdxs.size(); i--; )
                {
                    // Same cell collisions

                    for (uint32_t j = i; j--; )
                    {
                        float d_ij = distance(particles[cellIdxs[j]].getPosition(), particles[cellIdxs[i]].getPosition());

                        if (d_ij < 2.f * conf::h)
                        {
                            sf::Vector2f u_ij = (particles[cellIdxs[j]].getPosition() - particles[cellIdxs[i]].getPosition()) / d_ij;
                            sf::Vector2f f_collision_i = -1.f * conf::k * (2.f * conf::h - d_ij) * u_ij;

                            f_collisions[cellIdxs[i]] += f_collision_i;
                            f_collisions[cellIdxs[j]] -= f_collision_i;
                        }
                    }

                    // Neighbor cells collisions

                    for (auto neighborHash : neighborsHashes)
                    {
                        std::vector<uint32_t> neighborCellIdxs = hashGrid.getContentOfCell(neighborHash);

                        for (uint32_t j = (uint32_t) neighborCellIdxs.size(); j--;)
                        {
                            float d_ij = distance(particles[neighborCellIdxs[j]].getPosition(), particles[cellIdxs[i]].getPosition());

                            if (d_ij < 2.f * conf::h)
                            {
                                sf::Vector2f u_ij = (particles[neighborCellIdxs[j]].getPosition() - particles[cellIdxs[i]].getPosition()) / d_ij;
                                sf::Vector2f f_collision_i = -1.f * conf::k * (2.f * conf::h - d_ij) * u_ij;

                                f_collisions[cellIdxs[i]] += f_collision_i;
                                f_collisions[neighborCellIdxs[j]] -= f_collision_i;
                            }
                        }
                    }
                }
            }
            return f_collisions;
        }

    std::vector<uint32_t> getNeighborsHash(uint32_t hash, sf::Vector2f probeParticlePos) // Only taking below and right cells to avoid repetition
    {
        std::vector<uint32_t> neighborsHash;

        bool rightOnGrid = probeParticlePos.x + conf::cellSize < conf::cellSize * conf::n_collumns;
        bool leftOnGrid = probeParticlePos.x - conf::cellSize > 0;
        bool downOnGrid = probeParticlePos.y + conf::cellSize < conf::cellSize * conf::n_rows;

        if (rightOnGrid) {
            neighborsHash.push_back(hashGrid.getHashFromPos({ probeParticlePos.x + conf::cellSize , probeParticlePos.y }));
        }
        if (rightOnGrid && downOnGrid) {
            neighborsHash.push_back(hashGrid.getHashFromPos({ probeParticlePos.x + conf::cellSize , probeParticlePos.y + conf::cellSize }));
        }
        if (downOnGrid) {
            neighborsHash.push_back(hashGrid.getHashFromPos({ probeParticlePos.x, probeParticlePos.y + conf::cellSize }));
        }
        if (leftOnGrid && downOnGrid) {
            neighborsHash.push_back(hashGrid.getHashFromPos({ probeParticlePos.x - conf::cellSize , probeParticlePos.y + conf::cellSize }));
        }

        return neighborsHash;
    }
};

