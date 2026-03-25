// GravitySolverFactory.cpp - Factory for creating gravity solver instances

#include "pch.h"
#include "IGravitySolver.h"
#include "CPUGravitySolver.h"

#ifdef USE_CUDA
#include "CUDAGravitySolver.h"
#endif

#ifdef USE_OPENCL
#include "OpenCLGravitySolver.h"
#endif

IGravitySolver* GravitySolverFactory::CreateBestSolver()
{
    // Try CUDA first (typically best performance)
#ifdef USE_CUDA
    {
        CUDAGravitySolver* solver = new CUDAGravitySolver();
        if (solver->IsAvailable() && solver->Initialize())
        {
            return solver;
        }
        delete solver;
    }
#endif

    // Try OpenCL next (good cross-platform support)
#ifdef USE_OPENCL
    {
        OpenCLGravitySolver* solver = new OpenCLGravitySolver();
        if (solver->IsAvailable() && solver->Initialize())
        {
            return solver;
        }
        delete solver;
    }
#endif

    // Fall back to CPU (always available)
    CPUGravitySolver* solver = new CPUGravitySolver();
    if (solver->Initialize())
    {
        return solver;
    }
    delete solver;

    return nullptr;
}

IGravitySolver* GravitySolverFactory::CreateSolver(const char* name)
{
    if (_stricmp(name, "CPU") == 0)
    {
        CPUGravitySolver* solver = new CPUGravitySolver();
        if (!solver->Initialize())
        {
            delete solver;
            return nullptr;
        }
        return solver;
    }
#ifdef USE_CUDA
    else if (_stricmp(name, "CUDA") == 0)
    {
        CUDAGravitySolver* solver = new CUDAGravitySolver();
        if (!solver->IsAvailable() || !solver->Initialize())
        {
            delete solver;
            return nullptr;
        }
        return solver;
    }
#endif
#ifdef USE_OPENCL
    else if (_stricmp(name, "OpenCL") == 0)
    {
        OpenCLGravitySolver* solver = new OpenCLGravitySolver();
        if (!solver->IsAvailable() || !solver->Initialize())
        {
            delete solver;
            return nullptr;
        }
        return solver;
    }
#endif

    return nullptr;
}

void GravitySolverFactory::GetAvailableSolvers(CArray<CString>& out_names)
{
    out_names.RemoveAll();

    // CPU is always available
    out_names.Add(_T("CPU"));

#ifdef USE_CUDA
    {
        CUDAGravitySolver solver;
        if (solver.IsAvailable())
        {
            out_names.Add(_T("CUDA"));
        }
    }
#endif

#ifdef USE_OPENCL
    {
        OpenCLGravitySolver solver;
        if (solver.IsAvailable())
        {
            out_names.Add(_T("OpenCL"));
        }
    }
#endif
}
