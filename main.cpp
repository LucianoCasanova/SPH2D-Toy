#include <iostream>
#include "Simulation.hpp"

int main()
{
    Simulation simulation;
    std::cout << "deltaT in Simulation: " << conf::tau << std::endl;
    simulation.run();
}
