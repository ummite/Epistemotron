#include "Mass.h"
#include "Universe.h"
#include <cmath>
#include <sstream>

Mass::Mass()
    : m_X(0.0),
      m_Y(0.0),
      m_Z(0.0),
      m_VitesseX(0.0),
      m_VitesseY(0.0),
      m_VitesseZ(0.0),
      m_MasseKG(0.0)
{
}

Mass::~Mass()
{
}

void Mass::Randomize()
{
    // Random position within +/- 73 million km (same as Science/Mass.cpp)
    m_X = (static_cast<double>(rand()) / RAND_MAX - 0.5) * 146000000.0;
    m_Y = (static_cast<double>(rand()) / RAND_MAX - 0.5) * 146000000.0;
    m_Z = (static_cast<double>(rand()) / RAND_MAX - 0.5) * 146000000.0;

    // Random velocity between -29720 and +29720 m/s (same as Science/Mass.cpp)
    m_VitesseX = (static_cast<double>(rand()) / RAND_MAX - 0.5) * 29720.0;
    m_VitesseY = (static_cast<double>(rand()) / RAND_MAX - 0.5) * 29720.0;
    m_VitesseZ = (static_cast<double>(rand()) / RAND_MAX - 0.5) * 29720.0;

    // Earth mass as default
    m_MasseKG = 5.98e24;
}

// Position in KM = (Velocity in M/S * number of seconds) / 1000
// Divided by 1000 because we convert meters to kilometers. StepSize in seconds.
void Mass::EffectuerPasChangementPosition(int p_iStepSize)
{
    m_X += (m_VitesseX * p_iStepSize) / 1000.0;
    m_Y += (m_VitesseY * p_iStepSize) / 1000.0;
    m_Z += (m_VitesseZ * p_iStepSize) / 1000.0;
}

// Update velocity based on gravitational forces from all other masses
void Mass::EffectuerPasChangementVitesse(const Universe& p_roUniverse, int p_iStepSize)
{
    double dblImpactSpeedX = 0.0;
    double dblImpactSpeedY = 0.0;
    double dblImpactSpeedZ = 0.0;

    for (size_t i = 0; i < p_roUniverse.m_masses.size(); i++)
    {
        const Mass& roMass = p_roUniverse.m_masses[i];

        // Skip self-interaction - a mass should not exert gravitational force on itself
        if (&roMass == this)
        {
            continue;
        }

        const double deltaX = roMass.m_X - m_X;
        const double deltaY = roMass.m_Y - m_Y;
        const double deltaZ = roMass.m_Z - m_Z;

        // Compute distance-squared directly (in km^2) to avoid unnecessary sqrt
        const double dblDistanceSquaredKm = deltaX * deltaX + deltaY * deltaY + deltaZ * deltaZ;

        // Only compute sqrt when needed for the distance check
        const double dblDistance = std::sqrt(dblDistanceSquaredKm);

        if (dblDistance < DIST_MIN)
        {
            continue;  // Skip if too close to avoid numerical instability
        }

        // F = G * m1 * m2 / d^2 (distance in meters, so multiply km by 1000)
        // a = F / m = G * m_other / d^2
        // Using Plummer softening to prevent numerical instability at close range
        // Convert km^2 to m^2 by multiplying by 1000^2 = 1,000,000
        const double dblDistanceSquaredMeters = dblDistanceSquaredKm * 1000000.0 + SOFTENING_PARAM;
        const double dblAcceleration = G * roMass.m_MasseKG / dblDistanceSquaredMeters;

        // Direction cosines (normalized vector toward other mass)
        const double dirX = deltaX / dblDistance;
        const double dirY = deltaY / dblDistance;
        const double dirZ = deltaZ / dblDistance;

        // Velocity change = acceleration * direction * time step
        dblImpactSpeedX += dirX * dblAcceleration * p_iStepSize;
        dblImpactSpeedY += dirY * dblAcceleration * p_iStepSize;
        dblImpactSpeedZ += dirZ * dblAcceleration * p_iStepSize;
    }

    m_VitesseX += dblImpactSpeedX;
    m_VitesseY += dblImpactSpeedY;
    m_VitesseZ += dblImpactSpeedZ;
}

// Calculate distance in KM using 3D Euclidean distance
double Mass::Distance(const Mass& p_roMass) const
{
    const double dx = p_roMass.m_X - m_X;
    const double dy = p_roMass.m_Y - m_Y;
    const double dz = p_roMass.m_Z - m_Z;

    return std::sqrt(dx * dx + dy * dy + dz * dz);
}

std::string Mass::Trace(const Universe& p_roUniverse) const
{
    std::ostringstream oss;
    oss << "Iteration: " << p_roUniverse.m_iIteration
        << ", X: " << m_X << " Y: " << m_Y
        << " Speed X: " << m_VitesseX << " Y: " << m_VitesseY;
    return oss.str();
}