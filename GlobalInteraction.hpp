#pragma once
#include "configuration.hpp"
#include "Particle.hpp"
#include "hashGrid.hpp"
#include "WKernel.hpp"

template <typename Detector, typename Action, typename Output>
struct GlobalInteraction
{
    Detector detector;
    Action action;

    std::vector<Output> handleInteraction(const std::vector<Particle>& particles)
    {
        return detector.handleInteraction(particles, action);
    }
};

template <typename Output>
struct Action
{
    virtual void doAction(const std::vector<Particle>& particles, std::vector<Output>& output_v, const uint32_t idx_i, const uint32_t idx_j) const = 0;
};

struct DensityCalculator : public Action<float>
{
    void doAction(const std::vector<Particle>& particles, std::vector<float>& output_v, const uint32_t idx_i, const uint32_t idx_j) const override
    {
        calculate(particles, output_v, idx_i, idx_j);
    }

    void calculate(const std::vector<Particle>& particles, std::vector<float>& densities, const uint32_t idx_i, const uint32_t idx_j) const
    {
        float d_ij = distance(particles[idx_j].getPosition(), particles[idx_i].getPosition());

        if (d_ij < 2.f * conf::h)
        {
            WKernel kernel;
            float W_ij = kernel.W(d_ij);

            densities[idx_i] += conf::m_particle * W_ij;
            densities[idx_j] += conf::m_particle * W_ij;
        }
    }
};

struct Model : public Action<sf::Vector2f>
{
    void doAction(const std::vector<Particle>& particles, std::vector<sf::Vector2f>& output_v, const uint32_t idx_i, const uint32_t idx_j) const override
    {
        solve(particles, output_v, idx_i, idx_j);
    }
    
    virtual void solve(const std::vector<Particle>& particles, std::vector<sf::Vector2f>& f_collisions, const uint32_t idx_i, const uint32_t idx_j) const = 0;
};

struct SpringLike : public Model
{
    void solve(const std::vector<Particle>& particles, std::vector<sf::Vector2f>& f_collisions, const uint32_t idx_i, const uint32_t idx_j) const override
    {
        float d_ij = distance(particles[idx_j].getPosition(), particles[idx_i].getPosition());

        if (d_ij < 2.f * conf::h)
        {
            sf::Vector2f u_ij = (particles[idx_j].getPosition() - particles[idx_i].getPosition()) / d_ij;
            sf::Vector2f f_collision_i = -1.f * conf::k * (2.f * conf::h - d_ij) * u_ij;

            f_collisions[idx_i] += f_collision_i;
            f_collisions[idx_j] -= f_collision_i;
        }
    }
};

struct SPH : public Model
{
    void solve(const std::vector<Particle>& particles, std::vector<sf::Vector2f>& f_collisions, const uint32_t idx_i, const uint32_t idx_j) const override
    {
        float d_ij = distance(particles[idx_j].getPosition(), particles[idx_i].getPosition());

        if (d_ij < 2.f * conf::h)
        {
            /*sf::Clock clockSPH;
            clockSPH.restart();*/
            WKernel kernel;
            float dW_ij = kernel.dW(d_ij);

            sf::Vector2f u_ij = (particles[idx_j].getPosition() - particles[idx_i].getPosition()) / d_ij;

            float P_i = particles[idx_i].getPressure();
            float rho_i = particles[idx_i].getDensity();
            float P_j = particles[idx_j].getPressure();
            float rho_j = particles[idx_j].getDensity();

            float pressureTerm = P_i / (rho_i * rho_i) + P_j / (rho_j * rho_j);

            sf::Vector2f f_pressure = -1.f * conf::m_particle * conf::m_particle * pressureTerm * dW_ij * u_ij;

            sf::Vector2f v_i = particles[idx_i].getVelocity();
            sf::Vector2f v_j = particles[idx_j].getVelocity();
            float dot_product = (v_i - v_j).x * u_ij.x + (v_i - v_j).y * u_ij.y;
            sf::Vector2f f_viscosity = conf::m_particle * conf::m_particle * conf::alpha_v * conf::h * conf::v_max * (rho_i + rho_j) / 2.f * dot_product * dW_ij * u_ij;

            //std::cout << std::sqrt(f_pressure.x* f_pressure.x+ f_pressure.y * f_pressure.y) << std::endl;
            //std::cout << pressureTerm << std::endl;

            f_collisions[idx_i] += f_pressure + f_viscosity;
            f_collisions[idx_j] -= f_pressure - f_viscosity;
            /*std::cout << clockSPH.restart().asMicroseconds() << std::endl;*/
        }
    }
};

template <typename Output>
struct Detector
{
    virtual std::vector<Output> handleInteraction(const std::vector<Particle>& particles, const Action<Output>& action) = 0;

    std::vector<Output> output_v{ conf::n_particles, Output{} }; //Chequear como inicializan los Output, parece que bien??
};

template<>
struct Detector<float>
{
    virtual std::vector<float> handleInteraction(const std::vector<Particle>& particles, const Action<float>& action) = 0;
    std::vector<float> output_v;
    Detector() : output_v(conf::n_particles, 0.0f) {}
};

template <typename Output>
struct NaiveDetector : public Detector<Output>
{
    std::vector<Output> handleInteraction(const std::vector<Particle>& particles, const Action<Output>& action)
    {

        for (uint32_t i{ conf::n_particles }; i--; )
        {
            for (uint32_t j = i; j--; )
            {
                action.doAction(particles, output_v, i, j);
            }
        }
        return output_v;
    }
};

template <typename Output>
struct GridDetector : public Detector<Output>
{
    private:
            
        HashGrid hashGrid;

    public:

        std::vector<Output> handleInteraction(const std::vector<Particle>& particles, const Action<Output>& action)
        {
            hashGrid.clearGrid();
            hashGrid.mapParticlesToCell(particles);

            std::vector<uint32_t> hashes = hashGrid.getListOfHash();

            for (auto hash : hashes)
            {
                std::vector<uint32_t> cellIdxs = hashGrid.getContentOfCell(hash);

                sf::Vector2f probeParticlePos = particles[cellIdxs[0]].getPosition();
                std::vector<uint32_t> neighborsHashes = getNeighborsHash(hash, probeParticlePos);

                for (uint32_t i = (uint32_t)cellIdxs.size(); i--; )
                {
                    // Same cell interactions

                    for (uint32_t j = i; j--; )
                    {
                        //std::cout << output_v.size() << std::endl;
                        /*for (uint32_t k = output_v.size(), k--, k = 0)
                        {
                            std::cout
                        }*/
                        action.doAction(particles, output_v, cellIdxs[i], cellIdxs[j]);
                    }

                    // Neighbor cells interactions

                    for (auto neighborHash : neighborsHashes)
                    {
                        std::vector<uint32_t> neighborCellIdxs = hashGrid.getContentOfCell(neighborHash);

                        for (uint32_t j = (uint32_t)neighborCellIdxs.size(); j--;)
                        {
                            action.doAction(particles, output_v, cellIdxs[i], neighborCellIdxs[j]);
                        }
                    }
                }
            }
            return output_v;
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


