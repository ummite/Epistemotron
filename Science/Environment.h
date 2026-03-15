#pragma once

// Physical constants (type-safe constexpr instead of #define)
constexpr double K_RayonUniversCreationAleatoireMasses = 1e20;    // KM - Universe creation radius
constexpr double G = 6.67e-11;  // Gravitational constant (N⋅m²/kg²)
constexpr double MASSE_SOLEIL = 1.989e30;  // KG - Solar mass
constexpr double K_VitesseMAX = 300000000.0;  // M/s - Maximum velocity
constexpr int K_NombreEtoile = 15;  // Number of stars

constexpr int DIST_MIN = 1000;  // km - minimum distance to skip interaction (avoid self-interaction)
constexpr double SOFTENING_PARAM = 1000000.0;  // m^2 - Plummer softening parameter to prevent numerical instability