#pragma once

#include "Environment.h"
#include <string>

/**
 * @brief Represents a physical mass in the simulation
 *
 * A Mass object has position (in km), velocity (in m/s), and mass (in kg).
 * It can compute gravitational interactions with other masses.
 */
class Mass
{
public:
    Mass();
    virtual ~Mass();

    // Position in KM relative to origin
    double m_X;
    double m_Y;
    double m_Z;

    // Velocity in meters per second
    double m_VitesseX;
    double m_VitesseY;
    double m_VitesseZ;

    // Mass in KG
    double m_MasseKG;

    /**
     * @brief Randomize the mass position and velocity
     *
     * Sets position within a large sphere and random velocity
     * suitable for solar system-like simulations.
     */
    void Randomize();

    /**
     * @brief Calculate distance to another mass
     * @param p_roMass The other mass
     * @return Distance in KM
     */
    double Distance(const Mass& p_roMass) const;

    /**
     * @brief Update position based on current velocity
     * @param p_iStepSize Time step in seconds
     */
    void EffectuerPasChangementPosition(int p_iStepSize);

    /**
     * @brief Update velocity based on gravitational interactions
     * @param p_roUniverse The universe containing all masses
     * @param p_iStepSize Time step in seconds
     */
    void EffectuerPasChangementVitesse(const class Universe& p_roUniverse, int p_iStepSize);

    /**
     * @brief Generate a debug string with current state
     * @param p_roUniverse The universe (for iteration count)
     * @return Debug string with position and velocity info
     */
    std::string Trace(const class Universe& p_roUniverse) const;

    // ============================================================================
    // Collision Detection
    // ============================================================================

    /**
     * @brief Calculate the physical radius of this body based on its mass and density
     * @return Radius in kilometers
     */
    double GetPhysicalRadiusKM() const;

    /**
     * @brief Check if this body is colliding with another body
     * @param other The other Mass object to check against
     * @return true if the bodies are touching or overlapping
     */
    bool IsCollidingWith(const Mass& other) const;

    // ============================================================================
    // Constants
    // ============================================================================

    /// Average density for celestial bodies (kg/m^3)
    /// Used to calculate realistic radius from mass: r = (3*m / (4*pi*rho))^(1/3)
    static constexpr double DEFAULT_DENSITY_KG_M3 = 2000.0;
};