#pragma once

// Framework header for Epistemotron MFC application
// Defines Windows and MFC includes used throughout the project

// Include targetver.h first to set the minimum Windows platform version
#include "targetver.h"

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used Windows API elements
#endif

// ATL/MFC settings
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS  // Require explicit CString constructors
#define _AFX_ALL_WARNINGS                   // Disable MFC warning suppression

// Core Windows and MFC headers
#include <windows.h>        // Windows API
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

// MFC Automation (required for OLE/COM support)
#include <afxdisp.h>

// Common Controls support
#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>       // Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>         // Windows Common Controls (toolbar, status bar, etc.)
#endif

// Modern UI components (ribbons, docking, etc.)
#include <afxcontrolbars.h>

// C++ Standard Library
#include <algorithm>        // For std::sort, std::find, etc.
#include <vector>           // For std::vector
#include <cmath>            // For trigonometric functions

// Simulation enums (placed here in framework.h to ensure visibility in PCH)
enum class SimulationState { Stopped, Running, Paused };
enum class ScenarioType { SolarSystem, BinaryStar, ThreeBody, Galaxy };
enum class IntegratorType { SymplecticEuler, VelocityVerlet };

// Visual Styles manifest for Common Controls v6
#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif
