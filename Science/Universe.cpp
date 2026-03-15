#include "pch.h"

#include "Universe.h"
#include "Mass.h"
#include "Environment.h"
#include <vector>
#include <cmath>    // For M_PI, sqrt

// ============================================================================
// Scenario Configuration Constants
// ============================================================================

// Solar System scenario constants
namespace SolarSystemConstants
{
	constexpr double SunMass = 1.989e30;           // kg
	constexpr double EarthMass = 5.972e24;         // kg
	constexpr double MoonMass = 7.342e22;          // kg
	constexpr double MarsMass = 6.39e23;           // kg
	constexpr double EarthOrbitRadiusKM = 149598000;  // km (1 AU)
	constexpr double EarthOrbitalSpeed = 29780;    // m/s
	constexpr double MoonOrbitRadiusKM = 384400;   // km
	constexpr double MoonOrbitalSpeed = 1023;      // m/s (relative to Earth)
	constexpr double MarsOrbitRadiusKM = 227940000; // km (1.52 AU)
	constexpr double MarsOrbitalSpeed = 24070;     // m/s
	constexpr int BodyCount = 4;
}

// Binary Star scenario constants
namespace BinaryStarConstants
{
	constexpr double Star1MassRatio = 1.5;         // solar masses
	constexpr double Star2MassRatio = 1.0;         // solar masses
	constexpr double SeparationKM = 100000000.0;   // km (~0.67 AU)
	constexpr int BodyCount = 2;
}

// Three-Body Figure-8 scenario constants
namespace ThreeBodyConstants
{
	// Famous Chenciner-Montgomery figure-8 solution (1993/2000)
	// See: https://en.wikipedia.org/wiki/Three-body_problem#Figure-eight_solution
	constexpr double Mass = 1.0e30;                // kg (1 solar mass each)
	constexpr double PosScaleKM = 100000000.0;     // km - scale factor for positions

	// Normalized position coordinates
	constexpr double X1 = 0.97000436;
	constexpr double Y1 = -0.24308753;
	// x2 = -x1, y2 = -y1 (symmetric)
	constexpr double X3 = 0.0;
	constexpr double Y3 = 0.0;

	// Normalized velocity components
	constexpr double VX1 = 0.4662036850;
	constexpr double VY1 = 0.4323657300;
	// vx2 = vx1, vy2 = vy1 (symmetric)
	// vx3 = -2*vx1, vy3 = -2*vy1

	constexpr int BodyCount = 3;
}

// Galaxy scenario constants
namespace GalaxyConstants
{
	constexpr double CentralBlackHoleMassRatio = 4.0e6;  // solar masses (like Sgr A*)
	constexpr int OrbitingBodyCount = 50;
	constexpr double MinOrbitRadiusKM = 10000000000.0;   // km (~67 AU)
	constexpr double MaxOrbitRadiusKM = 100000000000.0;  // km (~670 AU)
	constexpr double MinStarMassRatio = 0.5;             // solar masses
	constexpr double MaxStarMassRatio = 2.0;             // solar masses
	constexpr double MaxInclinationRad = 0.1;            // ~6 degrees
}

// PPM Export constants
namespace PPMExportConstants
{
	constexpr int MassStep = 16;
	constexpr double ZoomFactor = std::sqrt(2.0);  // sqrt(2) for aspect ratio
	constexpr int PixelThreshold = 15;
	constexpr const wchar_t* DefaultOutputPath = L"c:\\temp\\t.bmp";
}


Universe::Universe()
	: m_iIteration(0)
{
}

Universe::Universe(int m_iMasses) :
	m_iIteration(0)
{
	m_arrMasses.SetSize(m_iMasses);
}

Universe::~Universe()
{
}

// Helper function to initialize a Mass with orbital parameters
static void InitializeMass(Mass& mass, double p_massKG, double p_x, double p_y, double p_z,
                          double p_vx, double p_vy, double p_vz)
{
	mass.m_MasseKG = p_massKG;
	mass.m_X = p_x;
	mass.m_Y = p_y;
	mass.m_Z = p_z;
	mass.m_VitesseX = p_vx;
	mass.m_VitesseY = p_vy;
	mass.m_VitesseZ = p_vz;
	mass.ClearTrail();
}

void Universe::LoadScenario(ScenarioType p_scenarioType)
{
	m_iIteration = 0;

	switch (p_scenarioType)
	{
	case ScenarioType::SolarSystem:
		LoadSolarSystem();
		break;
	case ScenarioType::BinaryStar:
		LoadBinaryStar();
		break;
	case ScenarioType::ThreeBody:
		LoadThreeBody();
		break;
	case ScenarioType::Galaxy:
		LoadGalaxy();
		break;
	}
}

// Solar System: Sun, Earth, Moon, Mars
void Universe::LoadSolarSystem()
{
	m_arrMasses.SetSize(SolarSystemConstants::BodyCount);

	// Sun at center
	InitializeMass(m_arrMasses[0], SolarSystemConstants::SunMass, 0, 0, 0, 0, 0, 0);

	// Earth at 1 AU
	InitializeMass(m_arrMasses[1], SolarSystemConstants::EarthMass,
		SolarSystemConstants::EarthOrbitRadiusKM, 0, 0,
		0, SolarSystemConstants::EarthOrbitalSpeed, 0);

	// Moon orbiting Earth
	InitializeMass(m_arrMasses[2], SolarSystemConstants::MoonMass,
		SolarSystemConstants::EarthOrbitRadiusKM + SolarSystemConstants::MoonOrbitRadiusKM, 0, 0,
		0, SolarSystemConstants::EarthOrbitalSpeed + SolarSystemConstants::MoonOrbitalSpeed, 0);

	// Mars at 1.52 AU
	InitializeMass(m_arrMasses[3], SolarSystemConstants::MarsMass,
		SolarSystemConstants::MarsOrbitRadiusKM, 0, 0,
		0, SolarSystemConstants::MarsOrbitalSpeed, 0);
}

// Binary Star System: Two stars orbiting their common center of mass
void Universe::LoadBinaryStar()
{
	m_arrMasses.SetSize(BinaryStarConstants::BodyCount);

	// Star 1: 1.5 solar masses, Star 2: 1.0 solar masses
	// Separation: 100 million km (about 0.67 AU)
	// Orbital period: ~2 years

	const double mass1 = BinaryStarConstants::Star1MassRatio * MASSE_SOLEIL;
	const double mass2 = BinaryStarConstants::Star2MassRatio * MASSE_SOLEIL;
	const double totalMass = mass1 + mass2;
	const double separationKM = BinaryStarConstants::SeparationKM;

	// Calculate barycenter distances
	const double r1 = separationKM * mass2 / totalMass;  // Distance from barycenter to star 1
	const double r2 = separationKM * mass1 / totalMass;  // Distance from barycenter to star 2

	// Orbital velocities for circular binary orbit
	// v1 = sqrt(G * m2² / (M_total * a)), v2 = sqrt(G * m1² / (M_total * a))
	// where a is the separation distance
	constexpr double KM_TO_METER = 1000.0;
	const double v1 = std::sqrt(G * mass2 * mass2 / (totalMass * separationKM * KM_TO_METER));
	const double v2 = std::sqrt(G * mass1 * mass1 / (totalMass * separationKM * KM_TO_METER));

	// Star 1 on the left, moving up
	InitializeMass(m_arrMasses[0], mass1, -r1, 0, 0, 0, v1, 0);

	// Star 2 on the right, moving down
	InitializeMass(m_arrMasses[1], mass2, r2, 0, 0, 0, -v2, 0);
}

// Three-Body Problem: Three equal masses in a stable figure-8 orbit
// Using the famous "figure-8" solution discovered by Moore (1993) and Chenciner & Montgomery (2000)
// Reference: https://en.wikipedia.org/wiki/Three-body_problem#Figure-eight_solution
void Universe::LoadThreeBody()
{
	m_arrMasses.SetSize(ThreeBodyConstants::BodyCount);

	const double mass = ThreeBodyConstants::Mass;

	// Figure-8 orbit initial conditions (normalized units, then scaled)
	// These are the famous Chenciner-Montgomery initial conditions
	const double x1 = ThreeBodyConstants::X1;
	const double y1 = ThreeBodyConstants::Y1;
	const double x2 = -x1;  // Symmetric
	const double y2 = -y1;  // Symmetric
	const double x3 = ThreeBodyConstants::X3;
	const double y3 = ThreeBodyConstants::Y3;

	// Velocities (symmetric)
	const double vx1 = ThreeBodyConstants::VX1;
	const double vy1 = ThreeBodyConstants::VY1;
	const double vx2 = vx1;
	const double vy2 = vy1;
	const double vx3 = -2.0 * vx1;
	const double vy3 = -2.0 * vy1;

	// Scale factor to convert to realistic units
	const double posScaleKM = ThreeBodyConstants::PosScaleKM;  // km

	// For equal masses and G, the velocity scale is: v_scale = sqrt(G * M / L)
	// where L is the length scale in meters
	constexpr double KM_TO_METER = 1000.0;
	const double velScale = std::sqrt(G * mass / (posScaleKM * KM_TO_METER));  // m/s

	// Body 1
	InitializeMass(m_arrMasses[0], mass,
		x1 * posScaleKM, y1 * posScaleKM, 0.0,
		vx1 * velScale, vy1 * velScale, 0.0);

	// Body 2
	InitializeMass(m_arrMasses[1], mass,
		x2 * posScaleKM, y2 * posScaleKM, 0.0,
		vx2 * velScale, vy2 * velScale, 0.0);

	// Body 3
	InitializeMass(m_arrMasses[2], mass,
		x3 * posScaleKM, y3 * posScaleKM, 0.0,
		vx3 * velScale, vy3 * velScale, 0.0);
}

// Galaxy-like: Many small bodies orbiting a central massive body (black hole)
void Universe::LoadGalaxy()
{
	const int nOrbitingBodies = GalaxyConstants::OrbitingBodyCount;
	m_arrMasses.SetSize(nOrbitingBodies + 1);

	// Central supermassive black hole (4 million solar masses, like Sgr A*)
	const double centralMass = GalaxyConstants::CentralBlackHoleMassRatio * MASSE_SOLEIL;
	InitializeMass(m_arrMasses[0], centralMass, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);

	// Orbiting stars with Keplerian velocities
	// Distributed in a disk-like configuration
	constexpr double KM_TO_METER = 1000.0;
	constexpr double PI = 3.14159265358979323846;
	constexpr double TWO_PI = 2.0 * PI;

	for (int i = 0; i < nOrbitingBodies; i++)
	{
		// Star mass: 0.5 to 2.0 solar masses (varying by index)
		const double starMassRatio = GalaxyConstants::MinStarMassRatio +
			((i % 10) * 0.15);
		const double starMass = starMassRatio * MASSE_SOLEIL;

		// Radius: logarithmically spaced from min to max orbit radius
		const double radius = GalaxyConstants::MinOrbitRadiusKM *
			std::pow(GalaxyConstants::MaxOrbitRadiusKM / GalaxyConstants::MinOrbitRadiusKM,
			         static_cast<double>(i) / nOrbitingBodies);

		// Angle in the disk (spread around 360 degrees with small offset)
		const double angle = TWO_PI * i / nOrbitingBodies + (i % 3 - 1) * 0.1;

		// Slight inclination for 3D effect (within ~6 degrees)
		const double inclination = (i % 5 - 2) * 0.05;  // radians

		// Position
		const double cosAngle = std::cos(angle);
		const double sinAngle = std::sin(angle);
		const double cosInclination = std::cos(inclination);
		const double sinInclination = std::sin(inclination);

		const double x = radius * cosAngle * cosInclination;
		const double y = radius * sinAngle * cosInclination;
		const double z = radius * sinInclination;

		// Keplerian orbital velocity: v = sqrt(G * M / r)
		// Convert radius to meters for calculation
		const double orbitalSpeed = std::sqrt(G * centralMass / (radius * KM_TO_METER));

		// Velocity perpendicular to position (tangential)
		const double vx = -orbitalSpeed * sinAngle * cosInclination;
		const double vy =  orbitalSpeed * cosAngle * cosInclination;
		const double vz =  orbitalSpeed * sinInclination;

		InitializeMass(m_arrMasses[i + 1], starMass, x, y, z, vx, vy, vz);
	}
}

Universe* Universe::GenerateSimulationStep(int p_iStepSize)
{
	Universe* poUniverse = new Universe(m_arrMasses.GetSize());
	poUniverse->m_iIteration = m_iIteration + 1;
	poUniverse->m_arrMasses.Copy(m_arrMasses);
	poUniverse->SimulateFrom(*this, p_iStepSize);
	return poUniverse;
}

void Universe::Randomize()
{
	for (int i = 0; i < m_arrMasses.GetSize(); i++)
	{
		// Use operator[] instead of GetAt() to get non-const reference
		m_arrMasses[i].Randomize();
	}
}

// Export universe visualization as PPM/BMP
void Universe::ExportPPM(int p_iWidth, int p_iHeight)
{
	if (m_arrMasses.GetSize() < 1)
	{
		return;  // Nothing to export
	}

	const Mass& roUniverseCenter = m_arrMasses.GetAt(0);

	// Find maximum distance from center for zoom calculation
	double dblDistanceMax = 1.0;
	for (int i = 1; i < m_arrMasses.GetSize(); i++)
	{
		double dblNewDistance = roUniverseCenter.Distance(m_arrMasses.GetAt(i));
		if (dblNewDistance > dblDistanceMax)
		{
			dblDistanceMax = dblNewDistance;
		}
	}

	const double zoomFactor = PPMExportConstants::ZoomFactor;

	const double dblXStep = (zoomFactor * 2.0 * dblDistanceMax) / static_cast<double>(p_iWidth);
	const double dblYStep = (zoomFactor * 2.0 * dblDistanceMax) / static_cast<double>(p_iHeight);

	// Use RAII containers to prevent memory leaks
	const size_t totalPixels = static_cast<size_t>(p_iWidth) * p_iHeight;
	std::vector<int> pixelMatrix(totalPixels, 0);
	std::vector<BYTE> buf(totalPixels * 3, 0);

	const int centerX = p_iWidth / 2;
	const int centerY = p_iHeight / 2;

	for (int i = 0; i < m_arrMasses.GetSize(); i++)
	{
		const Mass& roMass = m_arrMasses.GetAt(i);
		const int x = static_cast<int>((roMass.m_X - roUniverseCenter.m_X) / dblXStep) + centerX;
		const int y = static_cast<int>((roMass.m_Y - roUniverseCenter.m_Y) / dblYStep) + centerY;

		if (x >= 0 && x < p_iWidth && y >= 0 && y < p_iHeight)
		{
			const size_t idx = static_cast<size_t>(x) + static_cast<size_t>(y) * p_iWidth;
			pixelMatrix[idx] += PPMExportConstants::PixelThreshold;
		}
	}

	// Convert pixel matrix to RGB buffer
	for (int y = 0; y < p_iHeight; y++)
	{
		const size_t rowOffset = static_cast<size_t>(y) * p_iWidth;
		for (int x = 0; x < p_iWidth; x++)
		{
			const size_t pixelIdx = rowOffset + static_cast<size_t>(x);
			const int iValue = pixelMatrix[pixelIdx] > 0 ? 255 : 0;  // Binary: mass present or not

			const size_t bufIdx = pixelIdx * 3;
			buf[bufIdx] = static_cast<BYTE>(iValue);
			buf[bufIdx + 1] = static_cast<BYTE>(iValue);
			buf[bufIdx + 2] = static_cast<BYTE>(iValue);
		}
	}

	SaveBitmapToFile(buf.data(), p_iWidth, p_iHeight, 24, 0, PPMExportConstants::DefaultOutputPath);
}

void Universe::SimulateFrom(const Universe& p_roUniverse, int p_iStepSize)
{
	// Symplectic Euler integration with Newton's 3rd law optimization
	// This preserves energy better than standard Euler and reduces computation by 50%
	// For each mass, we use the OLD positions (from p_roUniverse) to compute forces,
	// then update velocities, then update positions using new velocities.

	const int nMassCount = m_arrMasses.GetSize();
	const double dt = static_cast<double>(p_iStepSize);

	// Extract positions and masses from source universe for efficient computation
	CArray<double> posX, posY, posZ, massArray;
	posX.SetSize(nMassCount);
	posY.SetSize(nMassCount);
	posZ.SetSize(nMassCount);
	massArray.SetSize(nMassCount);

	for (int i = 0; i < nMassCount; i++)
	{
		const Mass& m = p_roUniverse.GetAt(i);
		posX[i] = m.m_X;
		posY[i] = m.m_Y;
		posZ[i] = m.m_Z;
		massArray[i] = m.m_MasseKG;
	}

	// Compute accelerations using the optimized helper function
	CArray<double> accX, accY, accZ;
	accX.SetSize(nMassCount);
	accY.SetSize(nMassCount);
	accZ.SetSize(nMassCount);

	ComputeAccelerations(nMassCount, posX, posY, posZ, massArray, accX, accY, accZ);

	// Step 1: Update velocities using accumulated acceleration * dt
	for (int i = 0; i < nMassCount; i++)
	{
		Mass& roMass = m_arrMasses.GetAt(i);
		roMass.m_VitesseX += accX[i] * dt;
		roMass.m_VitesseY += accY[i] * dt;
		roMass.m_VitesseZ += accZ[i] * dt;
	}

	// Step 2: Update positions using the NEW velocities (symplectic order)
	// Step 3: Record trails - combined loop for efficiency
	for (int i = 0; i < nMassCount; i++)
	{
		Mass& roMass = m_arrMasses.GetAt(i);
		roMass.EffectuerPasChangementPosition(p_iStepSize);
		roMass.RecordTrailPosition();
		roMass.LimitTrailLength(Mass::MAX_TRAIL_LENGTH);
	}
}

// Helper function to compute accelerations for all bodies (used by Velocity Verlet)
// Optimized with Newton's 3rd law (F_AB = -F_BA) to compute each pair once
static void ComputeAccelerations(
	int nMassCount,
	const CArray<double>& posX, const CArray<double>& posY, const CArray<double>& posZ,
	const CArray<double>& mass,
	CArray<double>& accX, CArray<double>& accY, CArray<double>& accZ)
{
	// Zero accelerations
	for (int i = 0; i < nMassCount; i++)
	{
		accX[i] = 0.0;
		accY[i] = 0.0;
		accZ[i] = 0.0;
	}

	constexpr double KM_TO_METER = 1000.0;
	constexpr double KM_TO_METER_SQUARED = 1000000.0;

	// Iterate over unique pairs only (j > i) using Newton's 3rd law symmetry
	for (int i = 0; i < nMassCount; i++)
	{
		for (int j = i + 1; j < nMassCount; j++)
		{
			// Displacement vector from i to j
			const double deltaX = posX[j] - posX[i];
			const double deltaY = posY[j] - posY[i];
			const double deltaZ = posZ[j] - posZ[i];

			// Compute squared distance in km^2 first (avoid unnecessary sqrt)
			const double distanceSquaredKm = deltaX * deltaX + deltaY * deltaY + deltaZ * deltaZ;

			// Skip if too close (compare squared distances to avoid sqrt)
			if (distanceSquaredKm < static_cast<double>(DIST_MIN) * DIST_MIN)
				continue;

			// Convert to meters and apply Plummer softening
			const double distance = std::sqrt(distanceSquaredKm);
			const double distanceSquaredMeters = distanceSquaredKm * KM_TO_METER_SQUARED + SOFTENING_PARAM;

			// Direction cosines (unit vector from i to j)
			const double dirX = deltaX / distance;
			const double dirY = deltaY / distance;
			const double dirZ = deltaZ / distance;

			// Acceleration magnitudes: a = G * M_other / r^2
			const double accelI = G * mass[j] / distanceSquaredMeters;
			const double accelJ = G * mass[i] / distanceSquaredMeters;

			// Apply accelerations (Newton's 3rd law: equal and opposite)
			accX[i] += dirX * accelI;
			accY[i] += dirY * accelI;
			accZ[i] += dirZ * accelI;

			accX[j] -= dirX * accelJ;
			accY[j] -= dirY * accelJ;
			accZ[j] -= dirZ * accelJ;
		}
	}
}

// Velocity Verlet integration for superior energy conservation
// Algorithm:
// 1. r(t+dt) = r(t) + v(t)*dt + 0.5*a(t)*dt^2
// 2. Compute a(t+dt) from new positions
// 3. v(t+dt) = v(t) + 0.5*(a(t) + a(t+dt))*dt
//
// This is a time-symmetric, symplectic integrator that conserves energy
// much better than Euler methods, especially for orbital mechanics.
void Universe::SimulateFromVelocityVerlet(const Universe& p_roUniverse, int p_iStepSize)
{
	const int nMassCount = m_arrMasses.GetSize();
	const double dt = static_cast<double>(p_iStepSize);

	// Storage for positions, velocities, and accelerations
	// Using parallel arrays for cache efficiency in the inner loop
	CArray<double> posX, posY, posZ;
	CArray<double> velX, velY, velZ;
	CArray<double> accX, accY, accZ;
	CArray<double> massArray;

	posX.SetSize(nMassCount);
	posY.SetSize(nMassCount);
	posZ.SetSize(nMassCount);
	velX.SetSize(nMassCount);
	velY.SetSize(nMassCount);
	velZ.SetSize(nMassCount);
	accX.SetSize(nMassCount);
	accY.SetSize(nMassCount);
	accZ.SetSize(nMassCount);
	massArray.SetSize(nMassCount);

	// Copy initial state from source universe
	for (int i = 0; i < nMassCount; i++)
	{
		const Mass& m = p_roUniverse.GetAt(i);
		posX[i] = m.m_X;
		posY[i] = m.m_Y;
		posZ[i] = m.m_Z;
		velX[i] = m.m_VitesseX;
		velY[i] = m.m_VitesseY;
		velZ[i] = m.m_VitesseZ;
		massArray[i] = m.m_MasseKG;
	}

	constexpr double KM_TO_METER = 1000.0;
	constexpr double KM_TO_METER_SQUARED = 1000000.0;
	constexpr double HALF = 0.5;

	// Step 1: Compute accelerations at time t from current positions
	ComputeAccelerations(nMassCount, posX, posY, posZ, massArray, accX, accY, accZ);

	// Step 2: Update positions using r(t+dt) = r(t) + v(t)*dt + 0.5*a(t)*dt^2
	// Velocity is in m/s, position in km, so divide by 1000
	const double dtKm = dt / KM_TO_METER;  // dt converted to km scale
	const double halfDtSquaredKm = HALF * dt * dt / KM_TO_METER;  // 0.5*dt^2 in km scale

	for (int i = 0; i < nMassCount; i++)
	{
		posX[i] += velX[i] * dtKm + halfDtSquaredKm * accX[i];
		posY[i] += velY[i] * dtKm + halfDtSquaredKm * accY[i];
		posZ[i] += velZ[i] * dtKm + halfDtSquaredKm * accZ[i];
	}

	// Store old accelerations before recomputing
	CArray<double> oldAccX, oldAccY, oldAccZ;
	oldAccX.Copy(accX);
	oldAccY.Copy(accY);
	oldAccZ.Copy(accZ);

	// Step 3: Compute new accelerations at time t+dt from updated positions
	ComputeAccelerations(nMassCount, posX, posY, posZ, massArray, accX, accY, accZ);

	// Step 4: Update velocities using v(t+dt) = v(t) + 0.5*(a_old + a_new)*dt
	for (int i = 0; i < nMassCount; i++)
	{
		velX[i] += HALF * (oldAccX[i] + accX[i]) * dt;
		velY[i] += HALF * (oldAccY[i] + accY[i]) * dt;
		velZ[i] += HALF * (oldAccZ[i] + accZ[i]) * dt;
	}

	// Step 5: Copy final state to this universe
	for (int i = 0; i < nMassCount; i++)
	{
		Mass& m = m_arrMasses.GetAt(i);
		m.m_X = posX[i];
		m.m_Y = posY[i];
		m.m_Z = posZ[i];
		m.m_VitesseX = velX[i];
		m.m_VitesseY = velY[i];
		m.m_VitesseZ = velZ[i];
		m.m_MasseKG = massArray[i];

		// Record trail position after update
		m.RecordTrailPosition();
		m.LimitTrailLength(Mass::MAX_TRAIL_LENGTH);
	}

	m_iIteration = p_roUniverse.m_iIteration + 1;
}

// Calculate total kinetic energy: sum of 0.5 * m * v^2 for all masses
double Universe::GetTotalKineticEnergy() const
{
	double totalKE = 0.0;
	int n = m_arrMasses.GetSize();
	for (int i = 0; i < n; i++)
	{
		const Mass& mass = m_arrMasses.GetAt(i);
		totalKE += mass.GetKineticEnergy();
	}
	return totalKE;  // Joules
}

// Calculate total potential energy: sum of -G * m1 * m2 / r for all pairs
// Optimized to compute distance squared directly, avoiding unnecessary sqrt() calls
double Universe::GetTotalPotentialEnergy() const
{
	double totalPE = 0.0;
	const int n = m_arrMasses.GetSize();

	constexpr double KM_TO_METER = 1000.0;
	constexpr double KM_TO_METER_SQUARED = 1000000.0;  // (1000)^2

	// Iterate over unique pairs only (j > i) using Newton's 3rd law symmetry
	for (int i = 0; i < n; i++)
	{
		const Mass& massI = m_arrMasses.GetAt(i);

		for (int j = i + 1; j < n; j++)
		{
			const Mass& massJ = m_arrMasses.GetAt(j);

			// Compute displacement and squared distance directly in km^2
			const double deltaX = massJ.m_X - massI.m_X;
			const double deltaY = massJ.m_Y - massI.m_Y;
			const double deltaZ = massJ.m_Z - massI.m_Z;
			const double distanceSquaredKm = deltaX * deltaX + deltaY * deltaY + deltaZ * deltaZ;

			// Skip if too close (avoids self-interaction and numerical issues)
			// Compare squared distances to avoid sqrt()
			if (distanceSquaredKm < static_cast<double>(DIST_MIN) * DIST_MIN)
				continue;

			// Potential energy with Plummer softening
			// U = -G * m1 * m2 / sqrt(r^2 + epsilon^2)
			// Convert km^2 to m^2 by multiplying by 10^6
			const double distanceSquaredMeters = distanceSquaredKm * KM_TO_METER_SQUARED;
			const double effectiveDistance = std::sqrt(distanceSquaredMeters + SOFTENING_PARAM);

			totalPE += -G * massI.m_MasseKG * massJ.m_MasseKG / effectiveDistance;
		}
	}
	return totalPE;  // Joules
}

// Total energy = kinetic + potential (should be conserved in closed system)
double Universe::GetTotalEnergy() const
{
	return GetTotalKineticEnergy() + GetTotalPotentialEnergy();
}

// Sum of all individual linear momenta: P = sum(m_i * v_i)
void Universe::GetTotalLinearMomentum(double& out_px, double& out_py, double& out_pz) const
{
	out_px = 0.0;
	out_py = 0.0;
	out_pz = 0.0;

	int n = m_arrMasses.GetSize();
	for (int i = 0; i < n; i++)
	{
		const Mass& mass = m_arrMasses.GetAt(i);
		double px, py, pz;
		mass.GetLinearMomentum(px, py, pz);
		out_px += px;
		out_py += py;
		out_pz += pz;
	}
}

// Magnitude of total linear momentum: |P| = sqrt(px^2 + py^2 + pz^2)
double Universe::GetTotalLinearMomentumMagnitude() const
{
	double px, py, pz;
	GetTotalLinearMomentum(px, py, pz);
	return std::sqrt(px * px + py * py + pz * pz);  // kg*m/s
}

// Sum of all individual angular momenta about origin: L = sum(r_i x p_i)
void Universe::GetTotalAngularMomentum(double& out_Lx, double& out_Ly, double& out_Lz) const
{
	out_Lx = 0.0;
	out_Ly = 0.0;
	out_Lz = 0.0;

	int n = m_arrMasses.GetSize();
	for (int i = 0; i < n; i++)
	{
		const Mass& mass = m_arrMasses.GetAt(i);
		double Lx, Ly, Lz;
		mass.GetAngularMomentum(Lx, Ly, Lz);
		out_Lx += Lx;
		out_Ly += Ly;
		out_Lz += Lz;
	}
}

// Magnitude of total angular momentum: |L| = sqrt(Lx^2 + Ly^2 + Lz^2)
double Universe::GetTotalAngularMomentumMagnitude() const
{
	double Lx, Ly, Lz;
	GetTotalAngularMomentum(Lx, Ly, Lz);
	return std::sqrt(Lx * Lx + Ly * Ly + Lz * Lz);  // kg*m^2/s
}

void Universe::SaveBitmapToFile(BYTE* pBitmapBits,
	LONG lWidth,
	LONG lHeight,
	WORD wBitsPerPixel,
	const unsigned long& padding_size,
	LPCTSTR lpszFileName)
{
	// Some basic bitmap parameters  
	unsigned long headers_size = sizeof(BITMAPFILEHEADER) +
		sizeof(BITMAPINFOHEADER);

	unsigned long pixel_data_size = lHeight * ((lWidth * (wBitsPerPixel / 8)) + padding_size);

	BITMAPINFOHEADER bmpInfoHeader = { 0 };

	// Set the size  
	bmpInfoHeader.biSize = sizeof(BITMAPINFOHEADER);

	// Bit count  
	bmpInfoHeader.biBitCount = wBitsPerPixel;

	// Use all colors  
	bmpInfoHeader.biClrImportant = 0;

	// Use as many colors according to bits per pixel  
	bmpInfoHeader.biClrUsed = 0;

	// Store as un Compressed  
	bmpInfoHeader.biCompression = BI_RGB;

	// Set the height in pixels  
	bmpInfoHeader.biHeight = lHeight;

	// Width of the Image in pixels  
	bmpInfoHeader.biWidth = lWidth;

	// Default number of planes  
	bmpInfoHeader.biPlanes = 1;

	// Calculate the image size in bytes  
	bmpInfoHeader.biSizeImage = pixel_data_size;

	BITMAPFILEHEADER bfh = { 0 };

	// This value should be values of BM letters i.e 0x4D42  
	// 0x4D = M 0�42 = B storing in reverse order to match with endian  
	bfh.bfType = 0x4D42;
	//bfh.bfType = 'B'+('M' << 8); 

	// <<8 used to shift �M� to end  */  

	// Offset to the RGBQUAD
	bfh.bfOffBits = headers_size;

	// Total size of image including size of headers
	bfh.bfSize = headers_size + pixel_data_size;

	// Create the file in disk to write
	HANDLE hFile = CreateFile(lpszFileName,
		GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	// Return if error opening file
	if (!hFile) return;

	DWORD dwWritten = 0;

	// Write the File header
	WriteFile(hFile,
		&bfh,
		sizeof(bfh),
		&dwWritten,
		NULL);

	// Write the bitmap info header  
	WriteFile(hFile,
		&bmpInfoHeader,
		sizeof(bmpInfoHeader),
		&dwWritten,
		NULL);

	// Write the RGB Data  
	WriteFile(hFile,
		pBitmapBits,
		bmpInfoHeader.biSizeImage,
		&dwWritten,
		NULL);

	// Close the file handle
	CloseHandle(hFile);
}