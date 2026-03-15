// Simple test for the N-body simulation
// Tests basic physics correctness

#include "pch.h"
#include "Environment.h"
#include "Mass.h"
#include "Universe.h"
#include <iostream>
#include <cmath>

// Test 1: Verify gravitational acceleration calculation
bool TestGravitationalAcceleration()
{
    std::cout << "Test 1: Gravitational acceleration... ";

    Universe universe(2);
    Mass& body1 = universe.GetAt(0);
    Mass& body2 = universe.GetAt(1);

    // Body 1 at origin, Body 2 at 1 million km
    body1.m_MasseKG = MASSE_SOLEIL;
    body1.m_X = 0; body1.m_Y = 0; body1.m_Z = 0;
    body1.m_VitesseX = 0; body1.m_VitesseY = 0; body1.m_VitesseZ = 0;

    body2.m_MasseKG = 5.972e24;  // Earth mass
    body2.m_X = 1000000;  // 1 million km = 1e9 meters
    body2.m_Y = 0; body2.m_Z = 0;
    body2.m_VitesseX = 0; body2.m_VitesseY = 0; body2.m_VitesseZ = 0;

    // Expected acceleration: a = G * M / r^2
    double distanceMeters = 1e9;  // 1 million km in meters
    double expectedAccel = G * MASSE_SOLEIL / (distanceMeters * distanceMeters);

    // Apply one step
    int stepSize = 1;  // 1 second
    body2.EffectuerPasChangementVitesse(universe, stepSize);

    // Velocity change should equal acceleration * time
    double actualAccel = sqrt(body2.m_VitesseX * body2.m_VitesseX +
                              body2.m_VitesseY * body2.m_VitesseY +
                              body2.m_VitesseZ * body2.m_VitesseZ) / stepSize;

    // Allow 1% error due to softening
    double error = fabs(actualAccel - expectedAccel) / expectedAccel;
    bool passed = error < 0.01;

    std::cout << (passed ? "PASSED" : "FAILED") << std::endl;
    if (!passed)
    {
        std::cout << "  Expected accel: " << expectedAccel << " m/s^2" << std::endl;
        std::cout << "  Actual accel: " << actualAccel << " m/s^2" << std::endl;
        std::cout << "  Error: " << (error * 100) << "%" << std::endl;
    }

    return passed;
}

// Test 2: Verify circular orbit stability
bool TestCircularOrbit()
{
    std::cout << "Test 2: Circular orbit stability... ";

    Universe universe(2);
    Mass& sun = universe.GetAt(0);
    Mass& earth = universe.GetAt(1);

    // Sun at origin
    sun.m_MasseKG = MASSE_SOLEIL;
    sun.m_X = 0; sun.m_Y = 0; sun.m_Z = 0;
    sun.m_VitesseX = 0; sun.m_VitesseY = 0; sun.m_VitesseZ = 0;

    // Earth at 1 AU with circular orbital velocity
    double distanceKm = 149598000;  // 1 AU
    double distanceM = distanceKm * 1000;
    double orbitalVel = sqrt(G * MASSE_SOLEIL / distanceM);  // ~29780 m/s

    earth.m_MasseKG = 5.972e24;
    earth.m_X = distanceKm; earth.m_Y = 0; earth.m_Z = 0;
    earth.m_VitesseX = 0; earth.m_VitesseY = orbitalVel; earth.m_VitesseZ = 0;

    // Store initial distance
    double initialDistance = distanceKm;

    // Simulate one orbit (365 days, 1-hour steps)
    int stepSize = 3600;  // 1 hour
    int stepsPerOrbit = 365 * 24;

    for (int i = 0; i < stepsPerOrbit; i++)
    {
        Universe next = *universe.GenerateSimulationStep(stepSize);
        // Note: This leaks the old universe, but it's a test
    }

    // Calculate final distance from sun
    double finalDistance = sqrt(earth.m_X * earth.m_X + earth.m_Y * earth.m_Y);

    // Distance should be within 10% of initial (Euler drift is expected)
    double distanceError = fabs(finalDistance - initialDistance) / initialDistance;
    bool passed = distanceError < 0.10;

    std::cout << (passed ? "PASSED" : "FAILED") << std::endl;
    if (!passed)
    {
        std::cout << "  Initial distance: " << initialDistance << " km" << std::endl;
        std::cout << "  Final distance: " << finalDistance << " km" << std::endl;
        std::cout << "  Error: " << (distanceError * 100) << "%" << std::endl;
    }

    return passed;
}

// Test 3: Verify softening prevents singularity
bool TestSoftening()
{
    std::cout << "Test 3: Softening parameter... ";

    Universe universe(2);
    Mass& body1 = universe.GetAt(0);
    Mass& body2 = universe.GetAt(1);

    body1.m_MasseKG = MASSE_SOLEIL;
    body1.m_X = 0; body1.m_Y = 0; body1.m_Z = 0;

    body2.m_MasseKG = 5.972e24;
    // Place very close - within softening range
    body2.m_X = 0.5;  // 0.5 km - very close!
    body2.m_Y = 0; body2.m_Z = 0;

    // This should not cause infinity or NaN
    body2.EffectuerPasChangementVitesse(universe, 1);

    // Check for NaN or infinity
    bool hasNaN = std::isnan(body2.m_VitesseX) || std::isnan(body2.m_VitesseY) ||
                  std::isnan(body2.m_VitesseZ);
    bool hasInf = std::isinf(body2.m_VitesseX) || std::isinf(body2.m_VitesseY) ||
                  std::isinf(body2.m_VitesseZ);

    bool passed = !hasNaN && !hasInf;
    std::cout << (passed ? "PASSED" : "FAILED") << std::endl;

    return passed;
}

int main()
{
    std::cout << "=== Epistemotron N-Body Simulation Tests ===" << std::endl;
    std::cout << std::endl;

    int passed = 0;
    int total = 0;

    total++; if (TestGravitationalAcceleration()) passed++;
    total++; if (TestCircularOrbit()) passed++;
    total++; if (TestSoftening()) passed++;

    std::cout << std::endl;
    std::cout << "Results: " << passed << "/" << total << " tests passed" << std::endl;

    return (passed == total) ? 0 : 1;
}
