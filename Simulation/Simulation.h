#pragma once

/**
 * @mainpage Epistemotron Simulation Library
 *
 * A standalone C++ library for N-body gravitational simulation.
 *
 * ## Overview
 *
 * This library simulates gravitational interactions between multiple
 * masses in 3D space. It implements Newtonian gravity and can be
 * used for orbital mechanics simulations, planetary system modeling,
 * or any application requiring N-body physics.
 *
 * ## Components
 *
 * - @ref Mass - A physical body with position, velocity, and mass
 * - @ref Universe - A collection of masses that interact gravitationally
 * - @ref Simulator - High-level simulation orchestration
 *
 * ## Quick Start
 *
 * @code
 * #include "Simulation/Simulation.h"
 *
 * int main() {
 *     // Create a universe with 4 masses
 *     Universe universe(4);
 *
 *     // Setup: Sun, Earth, Moon, etc.
 *     universe.m_masses[0].m_MasseKG = 2e30;  // Sun
 *     universe.m_masses[1].m_MasseKG = 5.98e24;  // Earth
 *     universe.m_masses[1].m_X = 146000000.0;  // 146 million km
 *     universe.m_masses[1].m_VitesseY = 29720.0;  // Orbital velocity
 *
 *     // Run simulation
 *     Universe* next = universe.GenerateSimulationStep(86400);  // 1 day
 *
 *     delete next;
 *     return 0;
 * }
 * @endcode
 *
 * ## Build Requirements
 *
 * - C++17 compatible compiler
 * - CMake 3.16 or later (for CMake build system)
 *
 * ## License
 *
 * This library is provided as-is for educational and research purposes.
 */

#include "Environment.h"
#include "Mass.h"
#include "Universe.h"
#include "Simulator.h"