#pragma once

#include <vector>

class Universe;

/// @brief Represents a single position point in an orbit trail (coordinates in km)
struct TrailPoint
{
    double x;  ///< X coordinate in kilometers
    double y;  ///< Y coordinate in kilometers
    double z;  ///< Z coordinate in kilometers

    /// Default constructor - initializes all coordinates to zero
    TrailPoint() : x(0.0), y(0.0), z(0.0) {}

    /// Parameterized constructor
    /// @param px X coordinate in kilometers
    /// @param py Y coordinate in kilometers
    /// @param pz Z coordinate in kilometers
    TrailPoint(double px, double py, double pz) : x(px), y(py), z(pz) {}
};

/// @brief Represents a celestial body with mass, position, and velocity
///
/// The Mass class models a physical body in an N-body gravitational simulation.
/// Position is stored in kilometers, velocity in meters/second, and mass in kilograms.
/// Supports orbit trail recording for visualization purposes.
class Mass
{
public:
    // ============================================================================
    // Rule of Five - explicitly managing copy/move semantics
    // ============================================================================

    /// Default constructor - initializes all fields to zero
    Mass();

    /// Destructor
    ~Mass() = default;

    /// Copy constructor - performs deep copy of trail data
    Mass(const Mass& other) = default;

    /// Copy assignment operator - performs deep copy of trail data
    Mass& operator=(const Mass& other) = default;

    /// Move constructor - transfers ownership of trail data
    Mass(Mass&& other) noexcept = default;

    /// Move assignment operator - transfers ownership of trail data
    Mass& operator=(Mass&& other) noexcept = default;

    // ============================================================================
    // Physical Properties (Public Members)
    // ============================================================================

    // Position coordinates in kilometers (km)
    double m_X;  ///< X position in kilometers
    double m_Y;  ///< Y position in kilometers
    double m_Z;  ///< Z position in kilometers

    // Velocity components in meters per second (m/s)
    double m_VitesseX;  ///< X velocity component in m/s
    double m_VitesseY;  ///< Y velocity component in m/s
    double m_VitesseZ;  ///< Z velocity component in m/s

    double m_MasseKG;  ///< Mass in kilograms (kg)

    // ============================================================================
    // Constants
    // ============================================================================

    /// Maximum number of points to retain in orbit trail
    static constexpr int MAX_TRAIL_LENGTH = 500;

    // ============================================================================
    // Orbit Trail
    // ============================================================================

    /// Orbit trail storage - records historical positions for visualization
    std::vector<TrailPoint> m_trail;

    // ============================================================================
    // Initialization
    // ============================================================================

    /// Randomize this body's properties for testing/demo purposes
    ///
    /// Sets random position within +/-146 million km, random velocity
    /// within +/-29720 m/s, and mass to Earth's mass (5.98e24 kg).
    void Randomize();

    // ============================================================================
    // Physics Calculations
    // ============================================================================

    /// Calculate Euclidean distance to another mass
    /// @param other The other Mass object
    /// @return Distance in kilometers
    double Distance(const Mass& other) const;

    /// Update position based on current velocity (kinematics step)
    /// @param stepSize Time step in seconds
    void EffectuerPasChangementPosition(int stepSize);

    /// Update velocity based on gravitational interactions with all other masses
    /// @param universe Reference to the Universe containing all masses
    /// @param stepSize Time step in seconds
    void EffectuerPasChangementVitesse(const Universe& universe, int stepSize);

    // ============================================================================
    // Debug/Logging
    // ============================================================================

    /// Generate a debug string representation of this mass's state
    /// @param universe Reference to the Universe (used for iteration count)
    /// @return CString containing position and velocity information
    CString Trace(const Universe& universe) const;

    // ============================================================================
    // Energy and Momentum Calculations
    // ============================================================================

    /// Calculate kinetic energy: KE = 0.5 * m * v^2
    /// @return Kinetic energy in joules (J)
    double GetKineticEnergy() const;

    /// Calculate linear momentum: p = m * v
    /// @param out_px Output: x-component of linear momentum (kg*m/s)
    /// @param out_py Output: y-component of linear momentum (kg*m/s)
    /// @param out_pz Output: z-component of linear momentum (kg*m/s)
    void GetLinearMomentum(double& out_px, double& out_py, double& out_pz) const;

    /// Calculate angular momentum about origin: L = r x p
    /// @param out_Lx Output: x-component of angular momentum (kg*m^2/s)
    /// @param out_Ly Output: y-component of angular momentum (kg*m^2/s)
    /// @param out_Lz Output: z-component of angular momentum (kg*m^2/s)
    void GetAngularMomentum(double& out_Lx, double& out_Ly, double& out_Lz) const;

    // ============================================================================
    // Trail Management
    // ============================================================================

    /// Record current position to the orbit trail
    void RecordTrailPosition();

    /// Clear all recorded trail points
    void ClearTrail();

    /// Limit trail to a maximum number of points, removing oldest first
    /// @param maxLength Maximum number of points to retain (default: MAX_TRAIL_LENGTH)
    void LimitTrailLength(int maxLength = MAX_TRAIL_LENGTH);
};
