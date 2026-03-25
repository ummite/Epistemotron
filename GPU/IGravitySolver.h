#pragma once

// IGravitySolver.h - Abstract interface for gravity computation
// This allows swapping between CPU and GPU implementations transparently

#include "../Science/Mass.h"
#include "../Science/Universe.h"

// Forward declaration
class Universe;

/**
 * @brief Abstract interface for gravitational force computation
 *
 * This interface allows the simulation to use different backends:
 * - CPU implementation (reference, always available)
 * - CUDA implementation (NVIDIA GPUs, high performance)
 * - OpenCL implementation (cross-platform GPU support)
 *
 * The solver computes gravitational accelerations for all bodies
 * in the universe. The actual position/velocity updates are
 * handled by the integrator (Euler, Velocity Verlet, etc.)
 */
class IGravitySolver
{
public:
    virtual ~IGravitySolver() = default;

    /**
     * @brief Compute gravitational accelerations for all bodies
     *
     * For each body i, computes:
     *   acc[i] = sum over j!=i of G * m[j] * (r[j] - r[i]) / |r[j] - r[i]|^3
     *
     * @param universe The universe containing all bodies
     * @param out_accX Output array for X accelerations (m/s²)
     * @param out_accY Output array for Y accelerations (m/s²)
     * @param out_accZ Output array for Z accelerations (m/s²)
     *
     * @note Output arrays must be pre-allocated to universe.GetMassCount()
     */
    virtual void ComputeAccelerations(
        const Universe& universe,
        CArray<double>& out_accX,
        CArray<double>& out_accY,
        CArray<double>& out_accZ
    ) = 0;

    /**
     * @brief Get the name of this solver implementation
     */
    virtual const char* GetName() const = 0;

    /**
     * @brief Check if this solver is available on the current system
     */
    virtual bool IsAvailable() const = 0;

    /**
     * @brief Initialize the solver (e.g., allocate GPU memory)
     * @return true if initialization succeeded
     */
    virtual bool Initialize() = 0;

    /**
     * @brief Shutdown the solver and release resources
     */
    virtual void Shutdown() = 0;

    /**
     * @brief Check if the solver is currently initialized
     */
    virtual bool IsInitialized() const = 0;

    /**
     * @brief Get performance statistics
     */
    virtual struct Statistics
    {
        double lastComputeTimeMs;     // Time for last ComputeAccelerations call
        double totalComputeTimeMs;    // Cumulative time
        int computeCount;             // Number of ComputeAccelerations calls
        int bodyCount;                // Number of bodies in last computation
    } GetStatistics() const = 0;
};

/**
 * @brief Factory for creating gravity solver instances
 */
class GravitySolverFactory
{
public:
    /**
     * @brief Create the best available solver for this system
     * @return Pointer to solver, or nullptr if none available
     */
    static IGravitySolver* CreateBestSolver();

    /**
     * @brief Create a specific solver by name
     * @param name "CPU", "CUDA", or "OpenCL"
     * @return Pointer to solver, or nullptr if not available
     */
    static IGravitySolver* CreateSolver(const char* name);

    /**
     * @brief Get list of available solvers
     */
    static void GetAvailableSolvers(CArray<CString>& out_names);
};
