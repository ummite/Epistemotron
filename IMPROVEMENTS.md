# Improvements - Epistemotron

Cette liste suit les améliorations identifiées par l'analyse automatisée du code.

## Priorité Critique - Bugs qui doivent être corrigés immédiatement

### 1. Memory Leak - Universe objects not deleted (CRITICAL)
**File:** EpistemotronView.cpp:326-327, 360-363
**Issue:** Chaque appel à `GenerateSimulationStep()` crée un nouveau `Universe*` sans supprimer l'ancien.
**Impact:** Fuite mémoire massive - centaines d'objets perdus par seconde.

### 2. Const correctness bug in Universe.cpp
**File:** Universe.cpp:36
**Issue:** `m_arrMasses.GetAt(i).Randomize()` appelle une méthode non-const sur une référence const.
**Impact:** Le code ne compile probablement pas, ou l'appel est ignoré.

### 3. Missing default constructor declaration
**File:** Universe.h:11
**Issue:** Le constructeur par défaut est défini dans .cpp mais pas déclaré dans .h.
**Impact:** La méthode `Copy()` peut échouer silencieusement.

### 4. Physics - Euler integration order
**File:** Mass.cpp:42-47
**Issue:** Position mise à jour avec la VIEILLE vélocité, pas la nouvelle après accélération.
**Impact:** Les orbites spiralent vers l'extérieur ou l'intérieur (non-conservation de l'énergie).

## Priorité Haute - Problèmes importants

### 5. Physics - Add softening parameter
**File:** Mass.cpp:73-74
**Issue:** Pas de softening pour la force gravitationnelle - instabilité numérique à courte distance.

### 6. Physics - DIST_MIN value inappropriate
**File:** Environment.h:18
**Issue:** DIST_MIN = 1 km est trop petit pour les objets astronomiques.

### 7. GDI performance - object creation in loop
**File:** EpistemotronView.cpp:152-170
**Issue:** Création de CBrush/CPen dans la boucle de rendu pour chaque corps céleste.

### 8. No double buffering
**File:** EpistemotronView.cpp:63-90
**Issue:** Dessin direct sans double buffer - flickering visible.

### 9. O(n²) algorithm redundancy
**File:** Mass.cpp:56-85
**Issue:** Chaque paire de masses calcule la force deux fois (A→B et B→A).
**Improvement possible:** Diviser le temps de calcul par 2.

### 10. OutputWnd clipboard bugs
**File:** OutputWnd.cpp:195-221
**Issue:** Format clipboard incorrect (CF_TEXT vs CF_UNICODETEXT).

## Améliorations UI/UX

### 11. Add configuration dialog for simulation parameters
**Issue:** L'utilisateur ne peut pas configurer la taille du pas de temps, le nombre d'objets, etc.

### 12. Add pause/play controls
**Issue:** Pas de contrôle intuitif de la simulation.

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

### 18. Remove unprofessional function name
**File:** EpistemotronView.cpp:465
**Issue:** `RefreshThisShit()` doit être renommé.

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
