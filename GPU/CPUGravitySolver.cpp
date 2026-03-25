// CPUGravitySolver.cpp - CPU implementation of gravity solver

#include "pch.h"
#include "CPUGravitySolver.h"
#include "../Science/Environment.h"
#include <chrono>

CPUGravitySolver::CPUGravitySolver()
    : m_stats{ 0.0, 0.0, 0, 0 }
{
}

bool CPUGravitySolver::Initialize()
{
    // CPU solver doesn't need special initialization
    m_initialized = true;
    return true;
}

void CPUGravitySolver::Shutdown()
{
    m_initialized = false;
}

void CPUGravitySolver::ComputeAccelerations(
    const Universe& universe,
    CArray<double>& out_accX,
    CArray<double>& out_accY,
    CArray<double>& out_accZ
)
{
    auto startTime = std::chrono::high_resolution_clock::now();

    const int n = universe.GetMassCount();

    // Resize output arrays if needed
    if (out_accX.GetSize() != n)
    {
        out_accX.SetSize(n);
        out_accY.SetSize(n);
        out_accZ.SetSize(n);
    }

    // Initialize accelerations to zero
    for (int i = 0; i < n; i++)
    {
        out_accX[i] = 0.0;
        out_accY[i] = 0.0;
        out_accZ[i] = 0.0;
    }

    // O(n²) computation of gravitational accelerations
    // For each pair (i, j), compute the force and add to both bodies
    for (int i = 0; i < n; i++)
    {
        const Mass& massI = universe.GetAt(i);

        for (int j = i + 1; j < n; j++)
        {
            const Mass& massJ = universe.GetAt(j);

            // Compute displacement vector (j - i) in km
            const double dx = massJ.m_X - massI.m_X;
            const double dy = massJ.m_Y - massI.m_Y;
            const double dz = massJ.m_Z - massI.m_Z;

            // Compute distance squared in km², then convert to m²
            const double distSqKm = dx * dx + dy * dy + dz * dz;
            const double distSqM = distSqKm * 1e6;  // km² to m²

            // Apply softening to prevent singularities
            const double distSqSoftened = distSqM + SOFTENING_PARAM_SQUARED;

            // Distance for normalization (in meters)
            const double distM = std::sqrt(distSqSoftened);

            // Gravitational acceleration magnitude: G * m / r²
            // Force on i due to j: F = G * m[i] * m[j] / r²
            // Acceleration on i: a[i] = F / m[i] = G * m[j] / r²
            const double accelMagnitude = G * massJ.m_MasseKG / distSqSoftened;

            // Direction unit vector (j - i) / |j - i|
            // Need to convert dx, dy, dz from km to m for consistency
            const double dxM = dx * 1000.0;
            const double dyM = dy * 1000.0;
            const double dzM = dz * 1000.0;

            const double dirX = dxM / distM;
            const double dirY = dyM / distM;
            const double dirZ = dzM / distM;

            // Acceleration components (in m/s²)
            const double ax = accelMagnitude * dirX;
            const double ay = accelMagnitude * dirY;
            const double az = accelMagnitude * dirZ;

            // Apply to body i (attracted toward j)
            out_accX[i] += ax;
            out_accY[i] += ay;
            out_accZ[i] += az;

            // Apply to body j (attracted toward i, opposite direction)
            // This uses Newton's 3rd law to halve the computations
            out_accX[j] -= accelMagnitude * massI.m_MasseKG / massJ.m_MasseKG * dirX;
            out_accY[j] -= accelMagnitude * massI.m_MasseKG / massJ.m_MasseKG * dirY;
            out_accZ[j] -= accelMagnitude * massI.m_MasseKG / massJ.m_MasseKG * dirZ;
        }
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);

    // Update statistics
    m_stats.lastComputeTimeMs = duration.count() / 1000.0;
    m_stats.totalComputeTimeMs += m_stats.lastComputeTimeMs;
    m_stats.computeCount++;
    m_stats.bodyCount = n;
}
