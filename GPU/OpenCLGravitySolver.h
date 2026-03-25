#pragma once

#include "IGravitySolver.h"

// OpenCL includes - only included if compiling with OpenCL support
#ifdef USE_OPENCL
#include <CL/cl.h>
#endif

/**
 * @brief OpenCL implementation of gravity solver for cross-platform GPU support
 *
 * This implementation uses OpenCL to parallelize the O(n²) gravitational
 * force computation. OpenCL supports:
 * - NVIDIA GPUs
 * - AMD GPUs
 * - Intel GPUs
 * - Some CPUs and FPGAs
 *
 * Performance characteristics:
 * - ~50-500x faster than CPU for n > 10000 bodies
 * - More portable than CUDA but slightly lower performance
 * - Requires OpenCL driver support
 *
 * @note This file is a stub - full implementation requires OpenCL
 *       SDK and testing on target hardware. Marked for future implementation.
 */
class OpenCLGravitySolver : public IGravitySolver
{
public:
    OpenCLGravitySolver();
    ~OpenCLGravitySolver() override;

    // IGravitySolver interface
    void ComputeAccelerations(
        const Universe& universe,
        CArray<double>& out_accX,
        CArray<double>& out_accY,
        CArray<double>& out_accZ
    ) override;

    const char* GetName() const override { return "OpenCL"; }
    bool IsAvailable() const override;
    bool Initialize() override;
    void Shutdown() override;
    bool IsInitialized() const override { return m_initialized; }
    Statistics GetStatistics() const override { return m_stats; }

    /**
     * @brief Get the detected device name
     */
    const char* GetDeviceName() const { return m_deviceName; }

    /**
     * @brief Get the device type (GPU, CPU, etc.)
     */
    const char* GetDeviceType() const { return m_deviceType; }

private:
    bool m_initialized{ false };
    bool m_available{ false };
    Statistics m_stats{};
    char m_deviceName[256]{ "" };
    char m_deviceType[64]{ "" };

#ifdef USE_OPENCL
    // OpenCL context
    cl_platform_id m_platform{ nullptr };
    cl_device_id m_device{ nullptr };
    cl_context m_context{ nullptr };
    cl_command_queue m_queue{ nullptr };
    cl_program m_program{ nullptr };
    cl_kernel m_kernel{ nullptr };

    // Device memory
    cl_mem m_bufPositions{ nullptr };
    cl_mem m_bufMasses{ nullptr };
    cl_mem m_bufAccelX{ nullptr };
    cl_mem m_bufAccelY{ nullptr };
    cl_mem m_bufAccelZ{ nullptr };

    int m_allocatedSize{ 0 };

    bool AllocateDeviceMemory(int n);
    void FreeDeviceMemory();
    bool CompileProgram();
#endif
};
