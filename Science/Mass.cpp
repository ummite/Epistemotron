#include "pch.h"

#include "Mass.h"
#include "math.h"
#include "Environment.h"
#include "Universe.h"

namespace
{
    // ============================================================================
    // Randomization Constants
    // ============================================================================

    /// Maximum random position offset in km (approximately 1 AU)
    constexpr double RANDOM_POSITION_MAX_KM = 146000000.0;

    /// Maximum random velocity in m/s (approximately Earth's orbital speed)
    constexpr double RANDOM_VELOCITY_MAX_MS = 29720.0;

    /// Earth's mass in kg (used as default mass for randomization)
    constexpr double EARTH_MASS_KG = 5.98e24;

    // ============================================================================
    // Unit Conversion Constants
    // ============================================================================

    /// Meters per kilometer
    constexpr double METERS_PER_KM = 1000.0;

    /// Square meters per square kilometer
    constexpr double SQ_METERS_PER_SQ_KM = 1000000.0;
}

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

void Mass::Randomize()
{
    // Random position within +/- RANDOM_POSITION_MAX_KM
    // Each axis gets an independent random value
    m_X = (rand() / static_cast<double>(RAND_MAX) - 0.5) * RANDOM_POSITION_MAX_KM;
    m_Y = (rand() / static_cast<double>(RAND_MAX) - 0.5) * RANDOM_POSITION_MAX_KM;
    m_Z = (rand() / static_cast<double>(RAND_MAX) - 0.5) * RANDOM_POSITION_MAX_KM;

    // Random velocity between +/- RANDOM_VELOCITY_MAX_MS
    // Each velocity component gets an independent random value
    m_VitesseX = (rand() / static_cast<double>(RAND_MAX) - 0.5) * RANDOM_VELOCITY_MAX_MS;
    m_VitesseY = (rand() / static_cast<double>(RAND_MAX) - 0.5) * RANDOM_VELOCITY_MAX_MS;
    m_VitesseZ = (rand() / static_cast<double>(RAND_MAX) - 0.5) * RANDOM_VELOCITY_MAX_MS;

    // Set mass to Earth's mass as default
    m_MasseKG = EARTH_MASS_KG;
}

/// Update position based on velocity (kinematics step)
/// New Position (km) = Position (km) + Velocity (m/s) * StepSize (s) / 1000
void Mass::EffectuerPasChangementPosition(int stepSize)
{
    m_X += (m_VitesseX * stepSize) / METERS_PER_KM;
    m_Y += (m_VitesseY * stepSize) / METERS_PER_KM;
    m_Z += (m_VitesseZ * stepSize) / METERS_PER_KM;
}

// Update velocity based on gravitational interactions with all other masses
void Mass::EffectuerPasChangementVitesse(const Universe& universe, int stepSize)
{
    double velocityChangeX = 0.0;
    double velocityChangeY = 0.0;
    double velocityChangeZ = 0.0;

    for (int i = 0; i < universe.GetMassCount(); i++)
    {
        const Mass& otherMass = universe.GetAt(i);

        const double deltaX = otherMass.m_X - m_X;
        const double deltaY = otherMass.m_Y - m_Y;
        const double deltaZ = otherMass.m_Z - m_Z;

        // Compute distance-squared directly (in km^2) to avoid unnecessary sqrt
        const double distanceSquaredKm = deltaX * deltaX + deltaY * deltaY + deltaZ * deltaZ;

        // Only compute sqrt when needed for the distance check
        const double distance = sqrt(distanceSquaredKm);

        if (distance < DIST_MIN)
        {
            continue;  // Skip if too close (e.g., same mass)
        }

        // F = G*m1*m2/d^2 (distance in meters, so multiply km by 1000)
        // a = F/m = G*m_other/d^2
        // Using Plummer softening: replace d^2 with (d^2 + epsilon^2) to prevent
        // numerical instability when bodies get very close
        // Convert km^2 to m^2 by multiplying by SQ_METERS_PER_SQ_KM
        const double distanceSquaredMeters = distanceSquaredKm * SQ_METERS_PER_SQ_KM + SOFTENING_PARAM;
        const double acceleration = G * otherMass.m_MasseKG / distanceSquaredMeters;

        // Direction cosines (normalized vector toward other mass)
        const double dirX = deltaX / distance;
        const double dirY = deltaY / distance;
        const double dirZ = deltaZ / distance;

        // Velocity change = acceleration * direction * time step
        velocityChangeX += dirX * acceleration * stepSize;
        velocityChangeY += dirY * acceleration * stepSize;
        velocityChangeZ += dirZ * acceleration * stepSize;
    }

    m_VitesseX += velocityChangeX;
    m_VitesseY += velocityChangeY;
    m_VitesseZ += velocityChangeZ;
}

/// Calculate Euclidean distance to another mass
double Mass::Distance(const Mass& other) const
{
    const double dx = other.m_X - m_X;
    const double dy = other.m_Y - m_Y;
    const double dz = other.m_Z - m_Z;

    return sqrt(dx * dx + dy * dy + dz * dz);  // Returns distance in km
}


CString Mass::Trace(const Universe& universe) const
{
    CString strTrace;
    strTrace.Format(_T("Iteration: %d, x:%f y:%f  Velocity: X:%f Y:%f\n"),
                    universe.m_iIteration, m_X, m_Y, m_VitesseX, m_VitesseY);
    return strTrace;
}

// Calculate kinetic energy: KE = 0.5 * m * v^2
double Mass::GetKineticEnergy() const
{
    constexpr double HALF = 0.5;
    const double vSquared = m_VitesseX * m_VitesseX +
                            m_VitesseY * m_VitesseY +
                            m_VitesseZ * m_VitesseZ;
    return HALF * m_MasseKG * vSquared;  // Result in joules
}

// Calculate linear momentum: p = m * v (kg*m/s)
void Mass::GetLinearMomentum(double& out_px, double& out_py, double& out_pz) const
{
	out_px = m_MasseKG * m_VitesseX;
	out_py = m_MasseKG * m_VitesseY;
	out_pz = m_MasseKG * m_VitesseZ;
}

// Calculate angular momentum about origin: L = r x p (kg*m^2/s)
void Mass::GetAngularMomentum(double& out_Lx, double& out_Ly, double& out_Lz) const
{
    // Convert position from km to meters
    const double x = m_X * METERS_PER_KM;
    const double y = m_Y * METERS_PER_KM;
    const double z = m_Z * METERS_PER_KM;

    // Linear momentum components
    const double px = m_MasseKG * m_VitesseX;
    const double py = m_MasseKG * m_VitesseY;
    const double pz = m_MasseKG * m_VitesseZ;

    // Cross product: L = r x p
    out_Lx = y * pz - z * py;
    out_Ly = z * px - x * pz;
    out_Lz = x * py - y * px;
}

// Record current position to the orbit trail
void Mass::RecordTrailPosition()
{
	m_trail.push_back(TrailPoint(m_X, m_Y, m_Z));
}

// Clear all trail points
void Mass::ClearTrail()
{
	m_trail.clear();
}

// Limit trail length by removing oldest points
void Mass::LimitTrailLength(int maxLength)
{
	if (static_cast<int>(m_trail.size()) <= maxLength)
	{
		return;
	}

	// Remove oldest points (from the beginning) to keep only the newest
	size_t pointsToRemove = m_trail.size() - static_cast<size_t>(maxLength);
	m_trail.erase(m_trail.begin(), m_trail.begin() + pointsToRemove);
}