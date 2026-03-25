// CUDAGravitySolver.cpp - CUDA implementation of gravity solver
//
// NOTE: This is a PARTIAL implementation. Full testing requires:
// 1. NVIDIA GPU with compute capability >= 3.0
// 2. CUDA Toolkit installed
// 3. Project configured with CUDA support
//
// To enable: Add /DUSE_CUDA to preprocessor definitions and link with cudart

#include "pch.h"
#include "CUDAGravitySolver.h"
#include "../Science/Environment.h"
#include <chrono>

#ifdef USE_CUDA
#include <cuda_runtime.h>

// CUDA gravity kernel - each thread computes acceleration for one body
__global__ void GravityKernel(
    const double* positions,  // [n * 3] - x, y, z in km
    const double* masses,     // [n] - mass in kg
    double* accX,             // [n] - output acceleration x in m/s²
    double* accY,             // [n] - output acceleration y in m/s²
    double* accZ,             // [n] - output acceleration z in m/s²
    int n,
    double G,
    double softeningSquared
)
{
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if (i >= n) return;

    double ax = 0.0, ay = 0.0, az = 0.0;

    const double xi = positions[i * 3 + 0];
    const double yi = positions[i * 3 + 1];
    const double zi = positions[i * 3 + 2];

    // Compute force from all other bodies
    for (int j = 0; j < n; j++)
    {
        if (i == j) continue;

        const double xj = positions[j * 3 + 0];
        const double yj = positions[j * 3 + 1];
        const double zj = positions[j * 3 + 2];

        // Displacement in km
        const double dx = xj - xi;
        const double dy = yj - yi;
        const double dz = zj - zi;

        // Distance squared in m²
        const double distSqM = (dx * dx + dy * dy + dz * dz) * 1e6;

        // Apply softening
        const double distSqSoftened = distSqM + softeningSquared;
        const double distM = sqrt(distSqSoftened);

        // Acceleration magnitude
        const double aMag = G * masses[j] / distSqSoftened;

        // Direction (convert km to m)
        const double dirX = (dx * 1000.0) / distM;
        const double dirY = (dy * 1000.0) / distM;
        const double dirZ = (dz * 1000.0) / distM;

        ax += aMag * dirX;
        ay += aMag * dirY;
        az += aMag * dirZ;
    }

    accX[i] = ax;
    accY[i] = ay;
    accZ[i] = az;
}

#endif  // USE_CUDA

CUDAGravitySolver::CUDAGravitySolver()
    : m_stats{ 0.0, 0.0, 0, 0 }
{
#ifdef USE_CUDA
    m_stream = nullptr;
    m_d_positions = nullptr;
    m_d_masses = nullptr;
    m_d_accelX = nullptr;
    m_d_accelY = nullptr;
    m_d_accelZ = nullptr;
#endif
}

CUDAGravitySolver::~CUDAGravitySolver()
{
    Shutdown();
}

bool CUDAGravitySolver::IsAvailable() const
{
#ifdef USE_CUDA
    int deviceCount = 0;
    cudaError_t err = cudaGetDeviceCount(&deviceCount);
    return (err == cudaSuccess && deviceCount > 0);
#else
    return false;  // Not compiled with CUDA support
#endif
}

bool CUDAGravitySolver::Initialize()
{
    if (m_initialized) return true;

#ifdef USE_CUDA
    // Get device count
    int deviceCount = 0;
    cudaError_t err = cudaGetDeviceCount(&deviceCount);
    if (err != cudaSuccess || deviceCount == 0)
    {
        TRACE(_T("CUDA: No devices found\n"));
        return false;
    }

    // Use first GPU
    m_deviceId = 0;
    err = cudaSetDevice(m_deviceId);
    if (err != cudaSuccess)
    {
        TRACE(_T("CUDA: Failed to set device\n"));
        return false;
    }

    // Get device name
    cudaDeviceProp prop;
    err = cudaGetDeviceProperties(&prop, m_deviceId);
    if (err == cudaSuccess)
    {
        strncpy_s(m_deviceName, prop.name, _TRUNCATE);
    }

    // Create stream for async operations
    err = cudaStreamCreate(&m_stream);
    if (err != cudaSuccess)
    {
        TRACE(_T("CUDA: Failed to create stream\n"));
        return false;
    }

    m_initialized = true;
    m_available = true;
    TRACE(_T("CUDA: Initialized on %s\n"), m_deviceName);
    return true;
#else
    TRACE(_T("CUDA: Not compiled with CUDA support\n"));
    return false;
#endif
}

void CUDAGravitySolver::Shutdown()
{
    if (!m_initialized) return;

#ifdef USE_CUDA
    FreeDeviceMemory();

    if (m_stream != nullptr)
    {
        cudaStreamDestroy(m_stream);
        m_stream = nullptr;
    }

    cudaSetDevice(0);  // Reset to default device
#endif

    m_initialized = false;
}

bool CUDAGravitySolver::AllocateDeviceMemory(int n)
{
#ifdef USE_CUDA
    if (n <= m_allocatedSize) return true;  // Already have enough

    FreeDeviceMemory();

    size_t posSize = n * 3 * sizeof(double);
    size_t massSize = n * sizeof(double);
    size_t accSize = n * sizeof(double);

    cudaError_t err;

    err = cudaMalloc(&m_d_positions, posSize);
    if (err != cudaSuccess) return false;

    err = cudaMalloc(&m_d_masses, massSize);
    if (err != cudaSuccess) return false;

    err = cudaMalloc(&m_d_accelX, accSize);
    if (err != cudaSuccess) return false;

    err = cudaMalloc(&m_d_accelY, accSize);
    if (err != cudaSuccess) return false;

    err = cudaMalloc(&m_d_accelZ, accSize);
    if (err != cudaSuccess) return false;

    m_allocatedSize = n;
    return true;
#else
    return false;
#endif
}

void CUDAGravitySolver::FreeDeviceMemory()
{
#ifdef USE_CUDA
    if (m_d_positions) { cudaFree(m_d_positions); m_d_positions = nullptr; }
    if (m_d_masses) { cudaFree(m_d_masses); m_d_masses = nullptr; }
    if (m_d_accelX) { cudaFree(m_d_accelX); m_d_accelX = nullptr; }
    if (m_d_accelY) { cudaFree(m_d_accelY); m_d_accelY = nullptr; }
    if (m_d_accelZ) { cudaFree(m_d_accelZ); m_d_accelZ = nullptr; }
    m_allocatedSize = 0;
#endif
}

void CUDAGravitySolver::ComputeAccelerations(
    const Universe& universe,
    CArray<double>& out_accX,
    CArray<double>& out_accY,
    CArray<double>& out_accZ
)
{
    auto startTime = std::chrono::high_resolution_clock::now();

    const int n = universe.GetMassCount();

    if (n == 0) return;

    // Ensure output arrays are sized correctly
    if (out_accX.GetSize() != n)
    {
        out_accX.SetSize(n);
        out_accY.SetSize(n);
        out_accZ.SetSize(n);
    }

#ifndef USE_CUDA
    // Fallback to CPU if not compiled with CUDA
    TRACE(_T("CUDA: Not available, would use CPU fallback\n"));
    return;
#endif

    // Allocate memory if needed
    if (!AllocateDeviceMemory(n))
    {
        TRACE(_T("CUDA: Failed to allocate memory\n"));
        return;
    }

    // Prepare host data
    std::vector<double> h_positions(n * 3);
    std::vector<double> h_masses(n);

    for (int i = 0; i < n; i++)
    {
        const Mass& m = universe.GetAt(i);
        h_positions[i * 3 + 0] = m.m_X;
        h_positions[i * 3 + 1] = m.m_Y;
        h_positions[i * 3 + 2] = m.m_Z;
        h_masses[i] = m.m_MasseKG;
    }

    // Copy to device
    cudaError_t err;
    err = cudaMemcpy(m_d_positions, h_positions.data(), n * 3 * sizeof(double), cudaMemcpyHostToDevice);
    if (err != cudaSuccess) return;

    err = cudaMemcpy(m_d_masses, h_masses.data(), n * sizeof(double), cudaMemcpyHostToDevice);
    if (err != cudaSuccess) return;

    // Launch kernel
    const int threadsPerBlock = 256;
    const int blocksPerGrid = (n + threadsPerBlock - 1) / threadsPerBlock;

    const double softeningSquared = 1e6;  // 1 km² in m²

    GravityKernel<<<blocksPerGrid, threadsPerBlock, 0, m_stream>>>(
        m_d_positions,
        m_d_masses,
        m_d_accelX,
        m_d_accelY,
        m_d_accelZ,
        n,
        G,
        softeningSquared
    );

    // Copy results back
    err = cudaMemcpy(out_accX.GetData(), m_d_accelX, n * sizeof(double), cudaMemcpyDeviceToHost);
    if (err != cudaSuccess) return;

    err = cudaMemcpy(out_accY.GetData(), m_d_accelY, n * sizeof(double), cudaMemcpyDeviceToHost);
    if (err != cudaSuccess) return;

    err = cudaMemcpy(out_accZ.GetData(), m_d_accelZ, n * sizeof(double), cudaMemcpyDeviceToHost);
    if (err != cudaSuccess) return;

    // Synchronize to get accurate timing
    cudaStreamSynchronize(m_stream);

    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);

    // Update statistics
    m_stats.lastComputeTimeMs = duration.count() / 1000.0;
    m_stats.totalComputeTimeMs += m_stats.lastComputeTimeMs;
    m_stats.computeCount++;
    m_stats.bodyCount = n;
}
