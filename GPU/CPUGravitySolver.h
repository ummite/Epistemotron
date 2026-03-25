#pragma once

#include "IGravitySolver.h"

/**
 * @brief CPU implementation of gravity solver (reference implementation)
 *
 * This is a straightforward O(n²) implementation that works on any system.
 * It serves as:
 * - Fallback when GPU is not available
 * - Reference for validating GPU results
 * - Efficient enough for small simulations (< 1000 bodies)
 *
 * Future optimizations:
 * - Use SIMD (SSE/AVX) for vectorized computations
 * - Multi-threading with OpenMP
 * - Newton's 3rd law to halve computations
 */
class CPUGravitySolver : public IGravitySolver
{
public:
    CPUGravitySolver();
    ~CPUGravitySolver() override = default;

    // IGravitySolver interface
    void ComputeAccelerations(
        const Universe& universe,
        CArray<double>& out_accX,
        CArray<double>& out_accY,
        CArray<double>& out_accZ
    ) override;

    const char* GetName() const override { return "CPU"; }
    bool IsAvailable() const override { return true; }  // Always available
    bool Initialize() override;
    void Shutdown() override;
    bool IsInitialized() const override { return m_initialized; }
    Statistics GetStatistics() const override { return m_stats; }

private:
    bool m_initialized{ false };
    Statistics m_stats{};

    // Softening parameter to prevent singularities
    static constexpr double SOFTENING_PARAM_SQUARED = 1e6;  // 1 km² in m²
};
