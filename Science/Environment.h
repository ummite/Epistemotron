#pragma once

/// @file Environment.h
/// @brief Physical constants and simulation parameters for the N-body gravitational simulation
///
/// This header defines type-safe constexpr constants that replace traditional #define macros.
/// All constants use SI units or clearly documented alternative units.

// ============================================================================
// Physical Constants
// ============================================================================

/// Gravitational constant (Newton's constant)
/// Value: 6.67430 × 10⁻¹¹ N⋅m²/kg² (approximated as 6.67e-11)
/// @see https://en.wikipedia.org/wiki/Gravitational_constant
constexpr double G = 6.67e-11;  // N⋅m²/kg²

/// Solar mass reference value
/// Value: 1.989 × 10³⁰ kg (mass of the Sun)
/// Used as a reference for stellar mass calculations
constexpr double MASSE_SOLEIL = 1.989e30;  // kg

// ============================================================================
// Simulation Bounds
// ============================================================================

/// Maximum universe creation radius for random body placement
/// Bodies created randomly will be positioned within this radius from origin
constexpr double K_RayonUniversCreationAleatoireMasses = 1e20;  // km

/// Maximum velocity clamp to prevent numerical instability
/// Velocities exceeding this value will be clamped during simulation
/// Value: ~speed of light (299,792,458 m/s, approximated as 300,000,000 m/s)
constexpr double K_VitesseMAX = 300000000.0;  // m/s

/// Default number of stars for galaxy scenario
constexpr int K_NombreEtoile = 15;

// ============================================================================
// Collision and Stability Parameters
// ============================================================================

/// Minimum distance threshold to skip gravitational interaction
/// Prevents self-interaction and numerical singularities when bodies are too close
/// Bodies closer than this distance (center-to-center) skip force calculation
constexpr int DIST_MIN = 1000;  // km

/// Plummer softening parameter for gravitational force calculation
/// Added to distance-squared in the denominator to prevent numerical instability
/// when bodies approach very close distances
/// Formula: F = G*m1*m2 / (r² + ε²) where ε² = SOFTENING_PARAM
/// Value: (1000 m)² = 1,000,000 m²
constexpr double SOFTENING_PARAM = 1000000.0;  // m²