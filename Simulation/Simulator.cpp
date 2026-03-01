#include "Simulator.h"
#include "Universe.h"
#include <iostream>

Simulator::Simulator()
{
}

Simulator::~Simulator()
{
}

void Simulator::Test()
{
    const int iStepSize = 60 * 60 * 24;  // One day in seconds

    // Create a simple solar system: Sun, Earth, Moon, Moon2
    Universe oOriginalUniverse(4);

    {
        // Sun - at origin with large mass
        oOriginalUniverse.m_masses[0].m_MasseKG = 2e30;  // Sun mass

        // Earth - at 146 million km from Sun
        oOriginalUniverse.m_masses[1].m_MasseKG = 5.98e24;  // Earth mass
        oOriginalUniverse.m_masses[1].m_X = 146000000.0;     // 146 million km
        oOriginalUniverse.m_masses[1].m_VitesseY = 29720.0;  // ~29.7 km/s orbital velocity

        // Moon - orbiting Earth
        oOriginalUniverse.m_masses[2].m_MasseKG = 7.342e22;  // Moon mass
        oOriginalUniverse.m_masses[2].m_X = oOriginalUniverse.m_masses[1].m_X + 384400.0;  // 384,400 km from Earth
        oOriginalUniverse.m_masses[2].m_VitesseY = oOriginalUniverse.m_masses[1].m_VitesseY + 1023.0;  // Additional orbital velocity

        // Moon2 - another moon at different distance
        oOriginalUniverse.m_masses[3].m_MasseKG = 7.342e22;
        oOriginalUniverse.m_masses[3].m_X = oOriginalUniverse.m_masses[1].m_X + 3844000.0;  // 3,844,000 km from Earth
        oOriginalUniverse.m_masses[3].m_VitesseY = oOriginalUniverse.m_masses[1].m_VitesseY + 1023.0;
    }

    std::cout << "Initial universe state:\n";
    for (size_t i = 0; i < oOriginalUniverse.m_masses.size(); ++i)
    {
        std::cout << "  Mass " << i << ": "
                  << "Mass=" << oOriginalUniverse.m_masses[i].m_MasseKG
                  << "kg, Position=(" << oOriginalUniverse.m_masses[i].m_X
                  << ", " << oOriginalUniverse.m_masses[i].m_Y << ", "
                  << oOriginalUniverse.m_masses[i].m_Z << ") km"
                  << ", Velocity=(" << oOriginalUniverse.m_masses[i].m_VitesseX
                  << ", " << oOriginalUniverse.m_masses[i].m_VitesseY
                  << ", " << oOriginalUniverse.m_masses[i].m_VitesseZ << ") m/s\n";
    }

    // Generate one simulation step
    Universe* poCurrentUniverse = oOriginalUniverse.GenerateSimulationStep(iStepSize);

    std::cout << "\nAfter one day:\n";
    for (size_t i = 0; i < poCurrentUniverse->m_masses.size(); ++i)
    {
        std::cout << "  Mass " << i << ": "
                  << "Mass=" << poCurrentUniverse->m_masses[i].m_MasseKG
                  << "kg, Position=(" << poCurrentUniverse->m_masses[i].m_X
                  << ", " << poCurrentUniverse->m_masses[i].m_Y << ", "
                  << poCurrentUniverse->m_masses[i].m_Z << ") km"
                  << ", Velocity=(" << poCurrentUniverse->m_masses[i].m_VitesseX
                  << ", " << poCurrentUniverse->m_masses[i].m_VitesseY
                  << ", " << poCurrentUniverse->m_masses[i].m_VitesseZ << ") m/s\n";
    }

    // Clean up
    delete poCurrentUniverse;

    std::cout << "\nSimulation test complete.\n";
}