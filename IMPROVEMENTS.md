# Improvements - Epistemotron

Cette liste suit les améliorations identifiées par l'analyse automatisée du code.

## Priorité Critique - Bugs qui doivent être corrigés immédiatement

### 1. Memory Leak - Universe objects not deleted (CRITICAL) [FIXED]
**File:** EpistemotronView.cpp:326-327, 360-363
**Issue:** Chaque appel à `GenerateSimulationStep()` crée un nouveau `Universe*` sans supprimer l'ancien.
**Impact:** Fuite mémoire massive - centaines d'objets perdus par seconde.
**Fix:** Added delete before reassignment in StepSimulation() and OnTimer()

### 2. Const correctness bug in Universe.cpp [FIXED]
**File:** Universe.cpp:36
**Issue:** `m_arrMasses.GetAt(i).Randomize()` appelle une méthode non-const sur une référence const.
**Impact:** Le code ne compile probablement pas, ou l'appel est ignoré.
**Fix:** Changed GetAt(i) to operator[](i) for non-const reference

### 3. Missing default constructor declaration [FIXED]
**File:** Universe.h:11
**Issue:** Le constructeur par défaut est défini dans .cpp mais pas déclaré dans .h.
**Impact:** La méthode `Copy()` peut échouer silencieusement.
**Fix:** Added Universe() declaration to header

### 4. Physics - Euler integration order [FIXED]
**File:** Mass.cpp:42-47
**Issue:** Position mise à jour avec la VIEILLE vélocité, pas la nouvelle après accélération.
**Impact:** Les orbites spiralent vers l'extérieur ou l'intérieur (non-conservation de l'énergie).
**Fix:** Changed to symplectic Euler (position first, then velocity)

## Priorité Haute - Problèmes importants

### 5. Physics - Add softening parameter [FIXED]
**File:** Mass.cpp:73-74
**Issue:** Pas de softening pour la force gravitationnelle - instabilité numérique à courte distance.
**Fix:** Added Plummer softening with SOFTENING_PARAM = 1000000.0 m²

### 6. Physics - DIST_MIN value inappropriate [FIXED]
**File:** Environment.h:18
**Issue:** DIST_MIN = 1 km est trop petit pour les objets astronomiques.
**Fix:** Increased to 1000 km

### 7. GDI performance - object creation in loop [FIXED]
**File:** EpistemotronView.cpp:152-170
**Issue:** Création de CBrush/CPen dans la boucle de rendu pour chaque corps céleste.
**Fix:** Moved CPen creation outside the loop, only create CBrush per iteration

### 8. No double buffering [FIXED]
**File:** EpistemotronView.cpp:63-90
**Issue:** Dessin direct sans double buffer - flickering visible.
**Fix:** Implemented double buffering using CMemoryDC-compatible approach with CreateCompatibleDC/Bitmap

### 9. O(n²) algorithm redundancy
**File:** Mass.cpp:56-85
**Issue:** Chaque paire de masses calcule la force deux fois (A→B et B→A).
**Improvement possible:** Diviser le temps de calcul par 2.

### 10. OutputWnd clipboard bugs [FIXED]
**File:** OutputWnd.cpp:195-221
**Issue:** Format clipboard incorrect (CF_TEXT vs CF_UNICODETEXT).
**Fix:** Changed to CF_UNICODETEXT for proper Unicode clipboard support

## Améliorations UI/UX

### 11. Add configuration dialog for simulation parameters [IN PROGRESS]
**Issue:** L'utilisateur ne peut pas configurer la taille du pas de temps, le nombre d'objets, etc.
**Progress:** Created SimConfigDlg.h/cpp with dialog framework, added IDs to Resource.h

### 12. Add pause/play controls [DONE]
**Issue:** Pas de contrôle intuitif de la simulation.
**Status:** Already implemented with F5 and menu commands

### 13. Add statistics display (energy, momentum conservation)
**Issue:** Impossible de vérifier la précision de la simulation.

### 14. Add camera controls (zoom, pan)
**Issue:** Vue fixe sans interaction.

## Code Quality

### 15. Remove unused Environment class singleton
**File:** Science/Environment.cpp
**Issue:** Classe singleton vide qui ne sert à rien.

### 16. Replace #define with constexpr
**File:** Environment.h
**Issue:** Utilisation de #define au lieu de constexpr pour les constantes.

### 17. Fix inconsistent naming conventions
**Issue:** Mélange français/anglais dans les noms de méthodes.

### 18. Remove unprofessional function name [FIXED]
**File:** EpistemotronView.cpp:465
**Issue:** `RefreshThisShit()` doit être renommé.
**Fix:** Renamed to `RefreshView()`

## Summary

**Completed:** 9 critical/high priority fixes
- Memory leak in Universe objects
- Const correctness bug in Randomize()
- Missing default constructor declaration
- Physics integration order (symplectic Euler)
- Softening parameter for gravitational stability
- DIST_MIN value correction
- GDI performance optimization
- Double buffering implementation
- Clipboard Unicode support
- Unprofessional function name cleanup

**In Progress:** 1 (configuration dialog)
- SimConfigDlg.h/cpp created
- Resource IDs added
- Message handlers added

## Additional Critical Issues Found (Not Yet Fixed)

### EpistemotronDoc Issues
- **Missing Serialize()** - Files cannot be saved/loaded
- **Missing copy semantics** - Rule of Three violation with raw pointer
- **OnNewDocument() incomplete** - Old data persists
- **No UpdateAllViews()** - Breaks document-view pattern
- **m_pCurrentUniverse public** - Breaks encapsulation

### ClassView Issues
- **Memory leak** - CMFCPopupMenu never deleted
- **Menu handle issue** - Local CMenu destroyed while button uses handle
- **Dummy data** - Shows "FakeApp" instead of real project

### Simulation/Mass.cpp Issues
- **Inconsistent Randomize()** vs Science/Mass.cpp
  - Simulation: velocity = 0, range ±146M km
  - Science: velocity ±29720 m/s, range ±73M km
- **Self-interaction bug** - Mass calculates gravity from itself
- **Numerical overflow risk** - Distance squared can overflow

### PropertiesWnd Issues
- **Font resource leak** - Destroy() may not clean GDI handle
- **Property items ownership unclear**

**Remaining:** 20+ improvements/features
- O(n²) algorithm optimization (Newton's 3rd law)
- Fix Serialize() in EpistemotronDoc
- Fix ClassView memory leaks
- Fix Simulation/Mass.cpp Randomize() inconsistency
- Statistics display (energy, momentum)
- Camera controls (zoom, pan)
- Remove unused Environment class
- Replace #define with constexpr
- Fix naming conventions
- Velocity Verlet integrator
- Collision detection and merging
- Trail visualization
- Preset scenarios
- Export functionality (video, images)

## Build Notes

The project uses MFC with Visual Studio 2017+ (v142 toolset).
To build: Open Epistemotron.sln in Visual Studio and build.

New files added:
- Science/test_simulation.cpp - Standalone test program
- SimConfigDlg.h/cpp - Configuration dialog
- IMPROVEMENTS.md - This file
- SESSION_SUMMARY.md - Session summary

## Features à ajouter

### 19. Implement Velocity Verlet integrator
**Issue:** Remplacer Euler par un intégrateur symplectique pour la conservation de l'énergie.

### 20. Add collision detection and merging
**Issue:** Les corps qui se touchent devraient fusionner.

### 21. Add trail visualization for orbits
**Issue:** Pas d'historique des trajectoires.

### 22. Add preset scenarios (Solar System, galaxy, etc.)
**Issue:** L'utilisateur doit tout configurer manuellement.

### 23. Add export functionality (video, image sequence)
**Issue:** Impossible de partager les simulations.
