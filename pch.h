// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

// Ensure Winsock2.h is included before any MFC headers
// MFC requires Winsock2.h and will error if winsock.h is included instead
#include <winsock2.h>

// add headers that you want to pre-compile here
#include "framework.h"

#endif // PCH_H
