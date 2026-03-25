// OpenCLGravitySolver.cpp - OpenCL implementation of gravity solver
//
// NOTE: This is a PARTIAL implementation. Full testing requires:
// 1. OpenCL SDK installed
// 2. OpenCL-capable device (GPU recommended)
// 3. Project configured with OpenCL support
//
// To enable: Add /DUSE_OPENCL to preprocessor definitions and link with OpenCL

#include "pch.h"
#include "OpenCLGravitySolver.h"
#include "../Science/Environment.h"
#include <chrono>

// Embedded OpenCL kernel source
static const char* CL_KERNEL_SOURCE = R"(
__kernel void GravityKernel(
    const __global double* positions,  // [n * 3]
    const __global double* masses,     // [n]
    __global double* accX,             // [n]
    __global double* accY,             // [n]
    __global double* accZ,             // [n]
    int n,
    double G,
    double softeningSquared
)
{
    int i = get_global_id(0);
    if (i >= n) return;

    double ax = 0.0, ay = 0.0, az = 0.0;

    const double xi = positions[i * 3 + 0];
    const double yi = positions[i * 3 + 1];
    const double zi = positions[i * 3 + 2];

    for (int j = 0; j < n; j++)
    {
        if (i == j) continue;

        const double xj = positions[j * 3 + 0];
        const double yj = positions[j * 3 + 1];
        const double zj = positions[j * 3 + 2];

        const double dx = xj - xi;
        const double dy = yj - yi;
        const double dz = zj - zi;

        const double distSqM = (dx * dx + dy * dy + dz * dz) * 1e6;
        const double distSqSoftened = distSqM + softeningSquared;
        const double distM = sqrt(distSqSoftened);

        const double aMag = G * masses[j] / distSqSoftened;

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
)";

OpenCLGravitySolver::OpenCLGravitySolver()
    : m_stats{ 0.0, 0.0, 0, 0 }
{
#ifdef USE_OPENCL
    m_platform = nullptr;
    m_device = nullptr;
    m_context = nullptr;
    m_queue = nullptr;
    m_program = nullptr;
    m_kernel = nullptr;
#endif
}

OpenCLGravitySolver::~OpenCLGravitySolver()
{
    Shutdown();
}

bool OpenCLGravitySolver::IsAvailable() const
{
#ifdef USE_OPENCL
    cl_uint numPlatforms = 0;
    cl_int err = clGetPlatformIDs(0, nullptr, &numPlatforms);
    return (err == CL_SUCCESS && numPlatforms > 0);
#else
    return false;
#endif
}

bool OpenCLGravitySolver::Initialize()
{
    if (m_initialized) return true;

#ifdef USE_OPENCL
    // Find platform
    cl_uint numPlatforms = 0;
    cl_int err = clGetPlatformIDs(0, nullptr, &numPlatforms);
    if (err != CL_SUCCESS || numPlatforms == 0)
    {
        TRACE(_T("OpenCL: No platforms found\n"));
        return false;
    }

    cl_platform_id platforms[16];
    err = clGetPlatformIDs(16, platforms, &numPlatforms);
    if (err != CL_SUCCESS)
    {
        TRACE(_T("OpenCL: Failed to get platforms\n"));
        return false;
    }

    m_platform = platforms[0];  // Use first platform

    // Find GPU device
    cl_uint numDevices = 0;
    err = clGetDeviceIDs(m_platform, CL_DEVICE_TYPE_GPU, 0, nullptr, &numDevices);
    if (err != CL_SUCCESS || numDevices == 0)
    {
        // Try any device type
        err = clGetDeviceIDs(m_platform, CL_DEVICE_TYPE_ALL, 0, nullptr, &numDevices);
    }

    if (err != CL_SUCCESS || numDevices == 0)
    {
        TRACE(_T("OpenCL: No devices found\n"));
        return false;
    }

    cl_device_id devices[16];
    err = clGetDeviceIDs(m_platform, CL_DEVICE_TYPE_ALL, 16, devices, &numDevices);
    if (err != CL_SUCCESS)
    {
        TRACE(_T("OpenCL: Failed to get devices\n"));
        return false;
    }

    m_device = devices[0];

    // Get device info
    char deviceName[256];
    clGetDeviceInfo(m_device, CL_DEVICE_NAME, sizeof(deviceName), deviceName, nullptr);
    strncpy_s(m_deviceName, deviceName, _TRUNCATE);

    cl_device_type deviceType;
    clGetDeviceInfo(m_device, CL_DEVICE_TYPE, sizeof(deviceType), &deviceType, nullptr);
    if (deviceType & CL_DEVICE_TYPE_GPU)
        strncpy_s(m_deviceType, "GPU", _TRUNCATE);
    else if (deviceType & CL_DEVICE_TYPE_CPU)
        strncpy_s(m_deviceType, "CPU", _TRUNCATE);
    else
        strncpy_s(m_deviceType, "Other", _TRUNCATE);

    // Create context
    m_context = clCreateContext(nullptr, 1, &m_device, nullptr, nullptr, &err);
    if (err != CL_SUCCESS)
    {
        TRACE(_T("OpenCL: Failed to create context\n"));
        return false;
    }

    // Create command queue
    m_queue = clCreateCommandQueue(m_context, m_device, 0, &err);
    if (err != CL_SUCCESS)
    {
        TRACE(_T("OpenCL: Failed to create command queue\n"));
        return false;
    }

    // Compile program
    if (!CompileProgram())
    {
        TRACE(_T("OpenCL: Failed to compile program\n"));
        return false;
    }

    m_initialized = true;
    m_available = true;
    TRACE(_T("OpenCL: Initialized on %s (%s)\n"), m_deviceName, m_deviceType);
    return true;
#else
    TRACE(_T("OpenCL: Not compiled with OpenCL support\n"));
    return false;
#endif
}

#ifdef USE_OPENCL
bool OpenCLGravitySolver::CompileProgram()
{
    cl_int err;
    size_t sourceSize = strlen(CL_KERNEL_SOURCE);

    m_program = clCreateProgramWithSource(m_context, 1, &CL_KERNEL_SOURCE, &sourceSize, &err);
    if (err != CL_SUCCESS)
    {
        TRACE(_T("OpenCL: Failed to create program\n"));
        return false;
    }

    err = clBuildProgram(m_program, 1, &m_device, nullptr, nullptr, nullptr);
    if (err != CL_SUCCESS)
    {
        size_t logSize;
        clGetProgramBuildInfo(m_program, m_device, CL_PROGRAM_BUILD_LOG, 0, nullptr, &logSize);
        std::vector<char> log(logSize);
        clGetProgramBuildInfo(m_program, m_device, CL_PROGRAM_BUILD_LOG, logSize, log.data(), nullptr);
        TRACE(_T("OpenCL: Build failed: %s\n"), log.data());
        return false;
    }

    m_kernel = clCreateKernel(m_program, "GravityKernel", &err);
    if (err != CL_SUCCESS)
    {
        TRACE(_T("OpenCL: Failed to create kernel\n"));
        return false;
    }

    return true;
}
#endif

void OpenCLGravitySolver::Shutdown()
{
    if (!m_initialized) return;

#ifdef USE_OPENCL
    FreeDeviceMemory();

    if (m_kernel) { clReleaseKernel(m_kernel); m_kernel = nullptr; }
    if (m_program) { clReleaseProgram(m_program); m_program = nullptr; }
    if (m_queue) { clReleaseCommandQueue(m_queue); m_queue = nullptr; }
    if (m_context) { clReleaseContext(m_context); m_context = nullptr; }
#endif

    m_initialized = false;
}

#ifdef USE_OPENCL
bool OpenCLGravitySolver::AllocateDeviceMemory(int n)
{
    if (n <= m_allocatedSize) return true;

    FreeDeviceMemory();

    cl_int err;
    size_t posSize = n * 3 * sizeof(double);
    size_t massSize = n * sizeof(double);
    size_t accSize = n * sizeof(double);

    m_bufPositions = clCreateBuffer(m_context, CL_MEM_READ_ONLY, posSize, nullptr, &err);
    if (err != CL_SUCCESS) return false;

    m_bufMasses = clCreateBuffer(m_context, CL_MEM_READ_ONLY, massSize, nullptr, &err);
    if (err != CL_SUCCESS) return false;

    m_bufAccelX = clCreateBuffer(m_context, CL_MEM_WRITE_ONLY, accSize, nullptr, &err);
    if (err != CL_SUCCESS) return false;

    m_bufAccelY = clCreateBuffer(m_context, CL_MEM_WRITE_ONLY, accSize, nullptr, &err);
    if (err != CL_SUCCESS) return false;

    m_bufAccelZ = clCreateBuffer(m_context, CL_MEM_WRITE_ONLY, accSize, nullptr, &err);
    if (err != CL_SUCCESS) return false;

    m_allocatedSize = n;
    return true;
}

void OpenCLGravitySolver::FreeDeviceMemory()
{
    if (m_bufPositions) { clReleaseMemObject(m_bufPositions); m_bufPositions = nullptr; }
    if (m_bufMasses) { clReleaseMemObject(m_bufMasses); m_bufMasses = nullptr; }
    if (m_bufAccelX) { clReleaseMemObject(m_bufAccelX); m_bufAccelX = nullptr; }
    if (m_bufAccelY) { clReleaseMemObject(m_bufAccelY); m_bufAccelY = nullptr; }
    if (m_bufAccelZ) { clReleaseMemObject(m_bufAccelZ); m_bufAccelZ = nullptr; }
    m_allocatedSize = 0;
}
#endif

void OpenCLGravitySolver::ComputeAccelerations(
    const Universe& universe,
    CArray<double>& out_accX,
    CArray<double>& out_accY,
    CArray<double>& out_accZ
)
{
    auto startTime = std::chrono::high_resolution_clock::now();

    const int n = universe.GetMassCount();
    if (n == 0) return;

    if (out_accX.GetSize() != n)
    {
        out_accX.SetSize(n);
        out_accY.SetSize(n);
        out_accZ.SetSize(n);
    }

#ifndef USE_OPENCL
    TRACE(_T("OpenCL: Not available, would use CPU fallback\n"));
    return;
#endif

    if (!AllocateDeviceMemory(n))
    {
        TRACE(_T("OpenCL: Failed to allocate memory\n"));
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

    cl_int err;

    // Copy to device
    err = clEnqueueWriteBuffer(m_queue, m_bufPositions, CL_TRUE, 0, n * 3 * sizeof(double),
                               h_positions.data(), 0, nullptr, nullptr);
    if (err != CL_SUCCESS) return;

    err = clEnqueueWriteBuffer(m_queue, m_bufMasses, CL_TRUE, 0, n * sizeof(double),
                               h_masses.data(), 0, nullptr, nullptr);
    if (err != CL_SUCCESS) return;

    // Set kernel arguments
    err = clSetKernelArg(m_kernel, 0, sizeof(cl_mem), &m_bufPositions);
    err = clSetKernelArg(m_kernel, 1, sizeof(cl_mem), &m_bufMasses);
    err = clSetKernelArg(m_kernel, 2, sizeof(cl_mem), &m_bufAccelX);
    err = clSetKernelArg(m_kernel, 3, sizeof(cl_mem), &m_bufAccelY);
    err = clSetKernelArg(m_kernel, 4, sizeof(cl_mem), &m_bufAccelZ);
    err = clSetKernelArg(m_kernel, 5, sizeof(int), &n);
    err = clSetKernelArg(m_kernel, 6, sizeof(double), &G);
    err = clSetKernelArg(m_kernel, 7, sizeof(double), &(double){1e6});

    // Execute kernel
    size_t globalSize = n;
    err = clEnqueueNDRangeKernel(m_queue, m_kernel, 1, nullptr, &globalSize, nullptr,
                                  0, nullptr, nullptr);
    if (err != CL_SUCCESS) return;

    // Copy results back
    err = clEnqueueReadBuffer(m_queue, m_bufAccelX, CL_TRUE, 0, n * sizeof(double),
                               out_accX.GetData(), 0, nullptr, nullptr);
    if (err != CL_SUCCESS) return;

    err = clEnqueueReadBuffer(m_queue, m_bufAccelY, CL_TRUE, 0, n * sizeof(double),
                               out_accY.GetData(), 0, nullptr, nullptr);
    if (err != CL_SUCCESS) return;

    err = clEnqueueReadBuffer(m_queue, m_bufAccelZ, CL_TRUE, 0, n * sizeof(double),
                               out_accZ.GetData(), 0, nullptr, nullptr);
    if (err != CL_SUCCESS) return;

    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);

    m_stats.lastComputeTimeMs = duration.count() / 1000.0;
    m_stats.totalComputeTimeMs += m_stats.lastComputeTimeMs;
    m_stats.computeCount++;
    m_stats.bodyCount = n;
}
