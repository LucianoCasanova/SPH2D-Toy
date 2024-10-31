#include <iostream>
#include "Simulation.hpp"
#include "configuration.hpp"

sf::Clock Simulation::globalClock;
float Simulation::x_border = conf::x_initialBox;

//const sf::Clock& Simulation::getClock() {
//    return globalClock; // Retorna una referencia constante al reloj
//}

int main()
{
    Simulation simulation;
    std::cout << "deltaT in Simulation: " << conf::tau * 1000000 << std::endl;
    simulation.run();
}
