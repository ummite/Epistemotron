#pragma once

#include "Mass.h"
#include <vector>

/**
 * @brief Represents a universe of interacting masses
 *
 * The Universe class manages a collection of Mass objects and simulates
 * their gravitational interactions over time.
 */
class Universe
{
public:
    Universe();
    Universe(int p_iMasses);
    Universe(const Universe& src);  // Copy constructor
    virtual ~Universe();

    // Collection of masses in this universe
    std::vector<Mass> m_masses;

    // Current iteration number (simulation step count)
    int m_iIteration;

    /**
     * @brief Randomize all masses in the universe
     */
    void Randomize();

    /**
     * @brief Generate the next simulation step
     * @param p_iStepSize Time step in seconds
     * @return New Universe with updated positions and velocities
     */
    Universe* GenerateSimulationStep(int p_iStepSize);

    /**
     * @brief Simulate from another universe state
     * @param p_roUniverse Source universe state
     * @param p_iStepSize Time step in seconds
     */
    void SimulateFrom(const Universe& p_roUniverse, int p_iStepSize);
};