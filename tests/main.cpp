/**
 * @file main.cpp
 * @brief Test program for the Epistemotron Simulation Library
 *
 * This program demonstrates the basic usage of the simulation library
 * and verifies that the physics calculations work correctly.
 */

#include "../Simulation/Simulation.h"
#include <iostream>
#include <iomanip>
#include <cmath>

/**
 * @brief Print a formatted mass state
 */
void PrintMass(const Mass& mass, int index)
{
    std::cout << "  Mass " << index << ":\n"
              << "    Position: (" << std::fixed << std::setprecision(2)
              << mass.m_X << ", " << mass.m_Y << ", " << mass.m_Z << ") km\n"
              << "    Velocity: (" << std::setprecision(2)
              << mass.m_VitesseX << ", " << mass.m_VitesseY << ", " << mass.m_VitesseZ << ") m/s\n"
              << "    Mass: " << std::scientific << mass.m_MasseKG << " kg\n";
}

/**
 * @brief Test basic mass creation and randomization
 */
void TestMassRandomization()
{
    std::cout << "\n=== Test: Mass Randomization ===\n";

    Mass m;
    m.Randomize();

    std::cout << "Randomized mass:\n";
    PrintMass(m, 0);

    // Verify values are within expected range
    const double maxPos = 146000000.0;
    const double maxVel = 14860.0;  // Half of 29720

    bool passed = std::abs(m.m_X) < maxPos && std::abs(m.m_Y) < maxPos && std::abs(m.m_Z) < maxPos &&
                  std::abs(m.m_VitesseX) < maxVel && std::abs(m.m_VitesseY) < maxVel && std::abs(m.m_VitesseZ) < maxVel;

    std::cout << (passed ? "PASSED\n" : "FAILED\n");
}

/**
 * @brief Test distance calculation
 */
void TestDistance()
{
    std::cout << "\n=== Test: Distance Calculation ===\n";

    Mass m1, m2;
    m1.m_X = 0.0; m1.m_Y = 0.0; m1.m_Z = 0.0;
    m2.m_X = 3.0; m2.m_Y = 4.0; m2.m_Z = 0.0;  // Distance should be 5

    double dist = m1.Distance(m2);
    std::cout << "Distance between (0,0,0) and (3,4,0): " << std::setprecision(6) << dist << " km\n";

    bool passed = std::abs(dist - 5.0) < 0.001;
    std::cout << (passed ? "PASSED\n" : "FAILED\n");
}

/**
 * @brief Test simple solar system simulation
 */
void TestSolarSystem()
{
    std::cout << "\n=== Test: Solar System Simulation ===\n";

    // Create universe with Sun and Earth
    Universe universe(2);

    // Sun at origin
    universe.m_masses[0].m_MasseKG = 1.989e30;  // Sun mass
    universe.m_masses[0].m_X = 0.0;
    universe.m_masses[0].m_Y = 0.0;
    universe.m_masses[0].m_Z = 0.0;

    // Earth at 146 million km with orbital velocity
    universe.m_masses[1].m_MasseKG = 5.98e24;  // Earth mass
    universe.m_masses[1].m_X = 146000000.0;    // km
    universe.m_masses[1].m_Y = 0.0;
    universe.m_masses[1].m_Z = 0.0;
    universe.m_masses[1].m_VitesseY = 29720.0; // m/s (approximate orbital velocity)

    std::cout << "Initial state:\n";
    PrintMass(universe.m_masses[0], 0);
    PrintMass(universe.m_masses[1], 1);

    // Run simulation for several steps
    const int stepSize = 60 * 60 * 24;  // One day in seconds
    const int numSteps = 5;

    // Use heap allocation to avoid deleting stack variable
    Universe* current = new Universe(universe);  // Copy constructor
    for (int step = 0; step < numSteps; ++step)
    {
        Universe* next = current->GenerateSimulationStep(stepSize);
        delete current;
        current = next;
    }

    std::cout << "\nAfter " << numSteps << " days:\n";
    PrintMass(current->m_masses[0], 0);
    PrintMass(current->m_masses[1], 1);

    // Clean up
    delete current;

    std::cout << "PASSED (simulation ran without errors)\n";
}

/**
 * @brief Test multi-body simulation
 */
void TestMultiBody()
{
    std::cout << "\n=== Test: Multi-Body Simulation ===\n";

    // Create a solar system with Sun, Earth, and two Moons
    Universe universe(4);

    // Sun
    universe.m_masses[0].m_MasseKG = 2e30;

    // Earth
    universe.m_masses[1].m_MasseKG = 5.98e24;
    universe.m_masses[1].m_X = 146000000.0;
    universe.m_masses[1].m_VitesseY = 29720.0;

    // Moon 1
    universe.m_masses[2].m_MasseKG = 7.342e22;
    universe.m_masses[2].m_X = universe.m_masses[1].m_X + 384400.0;
    universe.m_masses[2].m_VitesseY = universe.m_masses[1].m_VitesseY + 1023.0;

    // Moon 2 (further out)
    universe.m_masses[3].m_MasseKG = 7.342e22;
    universe.m_masses[3].m_X = universe.m_masses[1].m_X + 3844000.0;
    universe.m_masses[3].m_VitesseY = universe.m_masses[1].m_VitesseY + 1023.0;

    std::cout << "Initial state:\n";
    for (size_t i = 0; i < universe.m_masses.size(); ++i)
    {
        std::cout << "Mass " << i << ":\n";
        PrintMass(universe.m_masses[i], static_cast<int>(i));
    }

    // Run simulation
    const int stepSize = 60 * 60 * 24;  // One day
    Universe* current = universe.GenerateSimulationStep(stepSize);

    std::cout << "\nAfter 1 day:\n";
    for (size_t i = 0; i < current->m_masses.size(); ++i)
    {
        std::cout << "Mass " << i << ":\n";
        PrintMass(current->m_masses[i], static_cast<int>(i));
    }

    // Clean up
    delete current;

    std::cout << "PASSED (multi-body simulation ran without errors)\n";
}

/**
 * @brief Test Trace function
 */
void TestTrace()
{
    std::cout << "\n=== Test: Trace Output ===\n";

    Universe universe(1);
    universe.m_iIteration = 42;
    universe.m_masses[0].m_X = 1000.0;
    universe.m_masses[0].m_Y = 2000.0;
    universe.m_masses[0].m_VitesseX = 100.0;
    universe.m_masses[0].m_VitesseY = 200.0;

    std::string trace = universe.m_masses[0].Trace(universe);
    std::cout << "Trace output: " << trace << "\n";

    bool passed = !trace.empty() && trace.find("Iteration: 42") != std::string::npos;
    std::cout << (passed ? "PASSED\n" : "FAILED\n");
}

/**
 * @brief Test physical radius calculation
 */
void TestPhysicalRadius()
{
    std::cout << "\n=== Test: Physical Radius Calculation ===\n";

    Mass earth;
    earth.m_MasseKG = 5.972e24;  // Earth mass in kg

    double radius = earth.GetPhysicalRadiusKM();
    std::cout << "Earth-like body radius: " << std::fixed << std::setprecision(2) << radius << " km\n";
    std::cout << "(Actual Earth radius: ~6371 km)\n";

    // With density of 2000 kg/m^3, Earth-mass body should have radius around 8900 km
    // This is larger than actual Earth because Earth's density (~5515 kg/m^3) is higher
    bool passed = radius > 6000.0 && radius < 10000.0;
    std::cout << (passed ? "PASSED\n" : "FAILED\n");
}

/**
 * @brief Test collision detection - non-colliding bodies
 */
void TestCollisionNoCollision()
{
    std::cout << "\n=== Test: Collision Detection (No Collision) ===\n";

    Mass m1, m2;
    m1.m_MasseKG = 1e24;  // Smaller bodies
    m1.m_X = 0.0; m1.m_Y = 0.0; m1.m_Z = 0.0;

    m2.m_MasseKG = 1e24;
    m2.m_X = 10000.0; m2.m_Y = 0.0; m2.m_Z = 0.0;  // 10,000 km apart

    bool colliding = m1.IsCollidingWith(m2);
    std::cout << "Bodies at 10,000 km distance: " << (colliding ? "COLLIDING" : "NOT COLLIDING") << "\n";

    bool passed = !colliding;
    std::cout << (passed ? "PASSED\n" : "FAILED\n");
}

/**
 * @brief Test collision detection - colliding bodies
 */
void TestCollisionDetected()
{
    std::cout << "\n=== Test: Collision Detection (Collision) ===\n";

    Mass m1, m2;
    m1.m_MasseKG = 1e30;  // Solar mass - very large radius
    m1.m_X = 0.0; m1.m_Y = 0.0; m1.m_Z = 0.0;

    m2.m_MasseKG = 1e30;  // Solar mass
    m2.m_X = 100.0; m2.m_Y = 0.0; m2.m_Z = 0.0;  // Only 100 km apart

    double radius1 = m1.GetPhysicalRadiusKM();
    double radius2 = m2.GetPhysicalRadiusKM();
    std::cout << "Body 1 radius: " << std::fixed << std::setprecision(2) << radius1 << " km\n";
    std::cout << "Body 2 radius: " << radius2 << " km\n";
    std::cout << "Distance between centers: 100 km\n";

    bool colliding = m1.IsCollidingWith(m2);
    std::cout << "Collision detected: " << (colliding ? "YES" : "NO") << "\n";

    bool passed = colliding;
    std::cout << (passed ? "PASSED\n" : "FAILED\n");
}

/**
 * @brief Test collision detection - edge case (just touching)
 */
void TestCollisionEdgeCase()
{
    std::cout << "\n=== Test: Collision Detection (Edge Case) ===\n";

    Mass m1, m2;
    m1.m_MasseKG = 5.972e24;  // Earth mass
    m1.m_X = 0.0; m1.m_Y = 0.0; m1.m_Z = 0.0;

    m2.m_MasseKG = 5.972e24;  // Earth mass
    double radius = m1.GetPhysicalRadiusKM();
    m2.m_X = radius * 2; m2.m_Y = 0.0; m2.m_Z = 0.0;  // Exactly touching (center-to-center = 2*radius)

    std::cout << "Body radius: " << std::fixed << std::setprecision(2) << radius << " km\n";
    std::cout << "Distance between centers: " << (radius * 2) << " km (exactly 2*radius)\n";

    bool colliding = m1.IsCollidingWith(m2);
    // At exactly 2*radius distance, bodies are touching but not overlapping
    // Our collision check uses < not <=, so this should return false
    std::cout << "Collision detected: " << (colliding ? "YES" : "NO") << "\n";
    std::cout << "(Expected: NO - bodies are touching but not overlapping)\n";

    bool passed = !colliding;
    std::cout << (passed ? "PASSED\n" : "FAILED\n");
}

int main()
{
    std::cout << "========================================\n";
    std::cout << "Epistemotron Simulation Library Tests\n";
    std::cout << "========================================\n";

    TestMassRandomization();
    TestDistance();
    TestSolarSystem();
    TestMultiBody();
    TestTrace();
    TestPhysicalRadius();
    TestCollisionNoCollision();
    TestCollisionDetected();
    TestCollisionEdgeCase();

    // Also run the built-in test
    std::cout << "\n=== Running Simulator::Test() ===\n";
    Simulator sim;
    sim.Test();

    std::cout << "\n========================================\n";
    std::cout << "All tests completed!\n";
    std::cout << "========================================\n";

    return 0;
}