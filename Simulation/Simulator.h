#pragma once

class Universe;

/**
 * @brief Simulator class for running physics simulations
 *
 * The Simulator provides high-level simulation orchestration.
 * It can run demonstration simulations and manage simulation workflows.
 */
class Simulator
{
public:
    Simulator();
    virtual ~Simulator();

    /**
     * @brief Run a demonstration simulation
     *
     * Creates a simple solar system (Sun, Earth, Moon, Moon2)
     * and runs a simulation step to demonstrate the physics.
     */
    void Test();
};