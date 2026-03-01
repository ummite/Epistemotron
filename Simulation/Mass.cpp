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
    // Initialize with 0 velocity
    m_VitesseX = 0.0;
    m_VitesseY = 0.0;
    m_VitesseZ = 0.0;

    // Random position within a large sphere (about 146 million km radius)
    m_X = (static_cast<double>(rand()) / RAND_MAX - 0.5) * 2.0 * 146000000.0;
    m_Y = (static_cast<double>(rand()) / RAND_MAX - 0.5) * 2.0 * 146000000.0;
    m_Z = (static_cast<double>(rand()) / RAND_MAX - 0.5) * 2.0 * 146000000.0;

    // Default mass (Earth-like)
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

    // Calculate gravitational acceleration from each mass
    for (size_t i = 0; i < p_roUniverse.m_masses.size(); i++)
    {
        const Mass& roMass = p_roUniverse.m_masses[i];

        const double dblDistance = Distance(roMass);
        if (dblDistance < DIST_MIN)  // Skip if too close to avoid division by zero
        {
            continue;
        }

        // F = G * m1 * m2 / d^2 (mass in kg, distance in meters)
        // Since we store distance in km, multiply distance by 1000^2 = 1000000
        // F = MA, so A = F / M (acceleration = force / our mass)
        // A * T = velocity change

        const double dblForce = (G * roMass.m_MasseKG * m_MasseKG) / (dblDistance * dblDistance * 1000000.0);
        const double dblAcceleration = dblForce / m_MasseKG;

        const double deltaX = roMass.m_X - m_X;
        const double deltaY = roMass.m_Y - m_Y;
        const double deltaZ = roMass.m_Z - m_Z;

        // Component of acceleration along each axis * time step = velocity change
        const double variationVitesseX = (deltaX * deltaX / (dblDistance * dblDistance)) * dblAcceleration * p_iStepSize;
        const double variationVitesseY = (deltaY * deltaY / (dblDistance * dblDistance)) * dblAcceleration * p_iStepSize;
        const double variationVitesseZ = (deltaZ * deltaZ / (dblDistance * dblDistance)) * dblAcceleration * p_iStepSize;

        // Accelerate towards the other mass
        if (deltaX > 0)
        {
            dblImpactSpeedX += variationVitesseX;
        }
        else
        {
            dblImpactSpeedX -= variationVitesseX;
        }

        if (deltaY > 0)
        {
            dblImpactSpeedY += variationVitesseY;
        }
        else
        {
            dblImpactSpeedY -= variationVitesseY;
        }

        if (deltaZ > 0)
        {
            dblImpactSpeedZ += variationVitesseZ;
        }
        else
        {
            dblImpactSpeedZ -= variationVitesseZ;
        }
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