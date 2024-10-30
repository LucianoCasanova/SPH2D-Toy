#include <iostream>
#include "Simulation.hpp"

int main()
{
    Simulation simulation;
    std::cout << "deltaT in Simulation: " << conf::tau * 1000000 << std::endl;
    simulation.run();
}
