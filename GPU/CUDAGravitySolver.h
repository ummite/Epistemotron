#pragma once

#include "IGravitySolver.h"

// CUDA includes - only included if compiling with CUDA support
#ifdef USE_CUDA
#include <cuda.h>
#include <cuda_runtime.h>
#endif

/**
 * @brief CUDA implementation of gravity solver for NVIDIA GPUs
 *
 * This implementation uses CUDA to parallelize the O(n²) gravitational
 * force computation across GPU threads. Each thread block computes
 * accelerations for a subset of bodies.
 *
 * Performance characteristics:
 * - ~100-1000x faster than CPU for n > 10000 bodies
 * - Memory bandwidth limited for very large n
 * - Requires NVIDIA GPU with compute capability >= 3.0
 *
 * Algorithm:
 * - Each thread computes forces from all other bodies on one body
 * - Uses shared memory for frequently accessed data
 * - Implements softening to prevent numerical instabilities
 *
 * @note This file is a stub - full implementation requires CUDA toolkit
 *       and GPU for testing. Marked for future implementation.
 */
class CUDAGravitySolver : public IGravitySolver
{
public:
    CUDAGravitySolver();
    ~CUDAGravitySolver() override;

    // IGravitySolver interface
    void ComputeAccelerations(
        const Universe& universe,
        CArray<double>& out_accX,
        CArray<double>& out_accY,
        CArray<double>& out_accZ
    ) override;

    const char* GetName() const override { return "CUDA"; }
    bool IsAvailable() const override;
    bool Initialize() override;
    void Shutdown() override;
    bool IsInitialized() const override { return m_initialized; }
    Statistics GetStatistics() const override { return m_stats; }

    /**
     * @brief Get the detected GPU device name
     */
    const char* GetDeviceName() const { return m_deviceName; }

    /**
     * @brief Get the GPU device ID being used
     */
    int GetDeviceId() const { return m_deviceId; }

private:
    bool m_initialized{ false };
    bool m_available{ false };
    Statistics m_stats{};
    char m_deviceName[256]{ "" };
    int m_deviceId{ -1 };

#ifdef USE_CUDA
    // CUDA device context
    cudaStream_t m_stream{ nullptr };

    // Device memory pointers
    double* m_d_positions{ nullptr };  // [n * 3] - x, y, z in km
    double* m_d_masses{ nullptr };     // [n] - mass in kg
    double* m_d_accelX{ nullptr };     // [n] - output acceleration x in m/s²
    double* m_d_accelY{ nullptr };     // [n] - output acceleration y in m/s²
    double* m_d_accelZ{ nullptr };     // [n] - output acceleration z in m/s²

    // Memory allocation
    int m_allocatedSize{ 0 };

    /**
     * @brief Allocate or resize device memory for n bodies
     */
    bool AllocateDeviceMemory(int n);

    /**
     * @brief Free all device memory
     */
    void FreeDeviceMemory();

    /**
     * @brief CUDA kernel for gravity computation
     * Each thread computes acceleration for one body
     */
    friend void __global__ GravityKernel(
        const double* positions,  // [n * 3]
        const double* masses,     // [n]
        double* accX,             // [n]
        double* accY,             // [n]
        double* accZ,             // [n]
        int n,
        double G,
        double softeningSquared
    );
#endif
};
