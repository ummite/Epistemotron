# Improvements - Epistemotron

Cette liste suit les améliorations identifiées par l'analyse automatisée du code.

## Session du 2026-03-24 (Loop 1h - Session 4)

**Job ID:** Cron loop | **Fréquence:** Toutes les heures | **Auto-expiration:** 7 jours

### Résumé de la Session 4

**Améliorations complétées cette session: 1**

**Résumé:** Cette session a implémenté la détection de collisions et la fusion de corps célestes, une fonctionnalité essentielle pour une simulation gravitationnelle réaliste. Les collisions sont détectées basées sur les rayons physiques calculés à partir de la masse et de la densité, et les corps fusionnés respectent les lois de conservation de la masse et du moment linéaire.

#### 20. Détection de collisions et fusion de corps [FIXED - 2026-03-24]
**Files:** Science/Mass.h, Science/Mass.cpp, Science/Universe.h, Science/Universe.cpp, EpistemotronView.h, EpistemotronView.cpp
**Feature:** Détection automatique de collisions entre corps célestes avec fusion physique réaliste
**Implementation:**
- Ajout de `GetPhysicalRadiusKM()` dans Mass: calcule le rayon physique basé sur la masse et une densité par défaut (2000 kg/m³)
  - Formule: r = (3 * m / (4 * π * ρ))^(1/3)
- Ajout de `IsCollidingWith()` dans Mass: détecte si deux corps sont en collision (distance < somme des rayons)
- Ajout de `ProcessCollisionsSimple()` dans Universe: traite toutes les collisions à chaque pas de simulation
  - Détection de paires en collision (chaque corps ne peut avoir qu'un partenaire de fusion par étape)
  - Fusion utilisant les lois de conservation:
    - Masse: m_merged = m1 + m2
    - Moment linéaire: v_merged = (m1*v1 + m2*v2) / (m1 + m2)
    - Position: centre de masse r_cm = (m1*r1 + m2*r2) / (m1 + m2)
  - Suppression des corps fusionnés de la simulation
  - Réinitialisation de la traînée pour le corps fusionné
- Intégration dans les deux intégrateurs: `SimulateFrom()` (Symplectic Euler) et `SimulateFromVelocityVerlet()` (Velocity Verlet)
- La détection de collision s'exécute après la mise à jour des positions à chaque pas de simulation
- **Tracking des collisions:**
  - Ajout de `m_totalCollisions` dans Universe pour compter les collisions cumulées
  - Ajout de `GetTotalCollisions()` pour accéder au compteur
  - Affichage du nombre total de collisions dans l'overlay UI
  - Réinitialisation du compteur lors du reset de la simulation
**Compilation:** Validée avec MSBuild Debug x64 - succès

#### 21. Feedback visuel des collisions (flash effect) [FIXED - 2026-03-24]
**Files:** Science/Universe.h, Science/Universe.cpp, EpistemotronView.h, EpistemotronView.cpp
**Feature:** Effet visuel de flash lorsque des collisions se produisent
**Implementation:**
- Ajout de `CollisionEvent` structure dans Universe pour stocker les positions et couleurs des collisions
- Ajout de `m_lastCollisions` vector dans Universe pour les événements de la dernière étape
- Ajout de `ConsumeLastCollisions()` pour récupérer et vider les événements de collision
- Enregistrement des événements de collision dans `ProcessCollisionsSimple()` avec couleur basée sur la masse fusionnée
  - Blanc-jaune pour masse stellaire (> 1 masse solaire)
  - Orange-jaune pour masse planétaire (> 1e24 kg)
  - Orange pour petits corps
- Ajout de `CollisionFlash` structure dans View avec position, couleur et durée restante
- Ajout de `AddCollisionFlash()`, `UpdateCollisionFlashes()`, `RenderCollisionFlashes()` dans View
- Intégration dans `OnTimer()` pour consommer les événements et créer des flashes
- Rendu des flashes dans `RenderUniverse3D()` avec projection 3D et rotation caméra
- Les flashes durent 3 frames et rétrécissent progressivement
**Compilation:** Validée avec MSBuild Debug x64 - succès

#### 22. Statistiques de collisions améliorées [FIXED - 2026-03-24]
**Files:** Science/Universe.h, Science/Universe.cpp, EpistemotronView.h, EpistemotronView.cpp
**Feature:** Affichage détaillé des statistiques de collisions dans l'overlay UI
**Implementation:**
- Ajout de `m_largestCollisionMass` dans Universe pour tracker la plus grande collision
- Ajout de `GetLargestCollisionMass()` pour accéder à cette statistique
- Ajout de `m_lastCollisionTime` et `m_largestCollisionMass` dans View
- Tracking du temps de la dernière collision avec `GetTickCount64()`
- Affichage du temps écoulé depuis la dernière collision (si < 60 secondes)
- Format: "Collisions: X (Ys ago)" pour un feedback immédiat
- Réinitialisation de toutes les statistiques de collision lors du reset
**Compilation:** Validée avec MSBuild Debug x64 - succès

#### 23. Raccourcis clavier pour le contrôle des collisions [FIXED - 2026-03-24]
**Files:** Science/Universe.h, Science/Universe.cpp, EpistemotronView.h, EpistemotronView.cpp
**Feature:** Raccourcis clavier pour contrôler le comportement des collisions
**Implementation:**
- Ajout de `m_bEnableCollisions` dans View et Universe pour activer/désactiver la détection
- Ajout de `m_bPauseOnCollision` dans View pour mettre en pause lors d'une collision
- Raccourcis clavier ajoutés dans `OnChar()`:
  - `C`: Toggle détection de collisions (ON/OFF)
  - `P`: Toggle pause sur collision
  - `V`: Effacer les effets visuels de collision
- Affichage du statut des collisions dans l'overlay UI: "Collisions: X [ON/OFF]"
- La simulation se met en pause automatiquement si `m_bPauseOnCollision` est activé et qu'une collision se produit
- Message TRACE pour confirmation des changements de statut
**Compilation:** Validée avec MSBuild Debug x64 - succès

#### 24. Sauchargement d'état de simulation [FIXED - 2026-03-24]
**Files:** Science/Universe.h, Science/Universe.cpp, Resource.h, Epistemotron.rc, EpistemotronView.h, EpistemotronView.cpp
**Feature:** Sauvegarde et chargement de l'état complet de la simulation
**Implementation:**
- Ajout de `SaveState()` dans Universe: sauvegarde binaire de tous les corps (positions, vélocités, masses)
- Ajout de `LoadState()` dans Universe: chargement depuis fichier binaire
- Format de fichier propriétaire (.esp) avec:
  - Magic number (0x50455355 = "PEUS") pour validation
  - Version du format (actuellement v1)
  - Compteur d'itération, statistiques de collisions
  - Nombre de corps et état de chaque corps
- Commandes ID_STATE_SAVE (32789) et ID_STATE_LOAD (32790) dans Resource.h
- Menu items "Save State..." et "Load State..." dans le menu Simulation
- Dialogues de fichier natifs Windows pour sélection de chemin
- Réinitialisation des statistiques de référence (énergie, moment) après chargement
- Validation du format de fichier et messages d'erreur appropriés
- Traces de débogage pour confirmation des opérations
**Compilation:** Validée avec MSBuild Debug x64 - succès

#### 25. Aide clavier mise à jour avec contrôles de collision [FIXED - 2026-03-24]
**Files:** EpistemotronView.cpp
**Feature:** Overlay d'aide complet avec tous les raccourcis y compris les contrôles de collision
**Implementation:**
- Ajout de section "Collision Control" dans l'overlay d'aide (?/F1)
  - C: Toggle collision detection
  - P: Pause on collision
  - V: Clear collision flashes
- Ajout de section "State" pour Save/Load State
- Organisation améliorée avec catégories claires
**Compilation:** Validée avec MSBuild Debug x64 - succès

#### 26. Messages de barre d'état pour feedback utilisateur [FIXED - 2026-03-24]
**Files:** MainFrm.h, MainFrm.cpp, EpistemotronView.h, EpistemotronView.cpp
**Feature:** Messages de statut dans la barre d'état pour feedback utilisateur
**Implementation:**
- Ajout de `SetStatusBarMessage()` dans CMainFrame pour afficher du texte
- Ajout de `SetStatusBarMessage()` dans CEpistemotronView comme wrapper
- Messages pour les raccourcis de collision (C, P, V)
- Messages pour Save/Load State (sélection de fichier, succès/échec)
- Inclusion du chemin de fichier dans les messages de confirmation
**Compilation:** Validée avec MSBuild Debug x64 - succès

### Total Cumulatif: 26 améliorations

---

## Session du 2026-03-23 (Loop 1h - Session 3)

**Job ID:** 08b90c7b | **Fréquence:** Toutes les heures | **Auto-expiration:** 7 jours

### Résumé de la Session 3

**Améliorations complétées cette session: 9**

**Résumé:** Cette session a corrigé les erreurs de compilation de la fonctionnalité d'enregistrement, complété le dialog de configuration, ajouté des raccourcis clavier complets, implémenté un overlay d'aide, ajouté un compteur FPS, implémenté le calcul de période orbitale pour les corps célestes sélectionnés, ajouté l'affichage de la dérive de conservation du moment linéaire et angulaire, et corrigé toutes les erreurs de compilation pour une build Debug réussie.

#### 11. Correction compilation fonctionnalité d'enregistrement [FIXED - 2026-03-23]
**Files:** EpistemotronView.cpp, EpistemotronView.h
**Issues fixes:**
- `GetDIBits` n'est pas un membre de CDC → Utiliser `::GetDIBits(hdc, ...)` avec `HDC hdc = m_memDC.GetSafeHdc()`
- `IsRecording()` définie deux fois → Supprimé le duplicata dans .cpp (déjà inline dans .h)

#### 12. Dialog de configuration complété [FIXED - 2026-03-23]
**Files:** EpistemotronView.cpp, Resource.rc
**Feature:** Dialog de configuration de simulation pleinement fonctionnel
**Implementation:**
- Template dialog déjà présent dans Resource.rc avec tous les contrôles
- Supprimé les commentaires obsolètes et le message d'erreur dans OnSimulationConfig()
- Le dialog permet de configurer:
  - Nombre de corps (1-10000)
  - Taille du pas de temps (1-31536000 secondes)
  - Pas par frame (1-10000)
  - Positions aléatoires avec rayon
  - Description de la simulation

#### 13. Raccourcis clavier simulation [FIXED - 2026-03-23]
**Files:** EpistemotronView.h, EpistemotronView.cpp
**Feature:** Raccourcis clavier complets pour le contrôle de la simulation
**Implementation:**
- Ajout de `OnChar()` handler pour les touches caractères
- Raccourcis ajoutés:
  - `Espace`: Pause/Reprendre la simulation
  - `S`: Reset de la simulation
  - `E`: Toggle enregistrement
  - `1-4`: Sélection de scénario direct
  - `+/-`: Zoom in/out (déjà existant)
  - `I`: Toggle intégrateur (déjà existant)
  - `T`: Toggle traînées (déjà existant)
  - `N`: Scénario suivant (déjà existant)
  - `R`: Reset rotation caméra
  - `Echap`: Reset caméra complète

#### 14. Nettoyage code et documentation [FIXED - 2026-03-23]
**Files:** IMPROVEMENTS.md
**Feature:** Mise à jour de la documentation des améliorations

#### 15. Overlay d'aide raccourcis clavier [FIXED - 2026-03-23]
**Files:** EpistemotronView.h, EpistemotronView.cpp
**Feature:** Affichage complet des raccourcis clavier via F1 ou '?'
**Implementation:**
- Ajout de `m_bShowHelp` pour le toggle de l'overlay
- Gestion F1 dans OnKeyDown() et '?' dans OnChar()
- Overlay semi-transparent avec liste complète des raccourcis:
  - Contrôle simulation (F5-F10, Space, +/-)
  - Scénarios (1-4, N)
  - Caméra (Wheel, +/-, drag, R, ESC)
  - Affichage (I, T, F1/?)
  - Sélection (click, Ctrl+click)
  - Enregistrement (E)
- Mise à jour de la section Controls dans l'overlay principal

#### 16. Compteur FPS [FIXED - 2026-03-23]
**Files:** EpistemotronView.h, EpistemotronView.cpp
**Feature:** Affichage du nombre de frames par seconde
**Implementation:**
- Ajout de `m_lastFrameTime`, `m_frameCount`, `m_currentFps`
- Calcul dans OnDraw() avec GetTickCount64()
- Affichage dans l'overlay UI: "FPS: X.X"

#### 17. Calcul période orbitale [FIXED - 2026-03-23]
**Files:** EpistemotronView.h, EpistemotronView.cpp
**Feature:** Calcul et affichage de la période orbitale pour un corps sélectionné
**Implementation:**
- Ajout de `CalculateOrbitalPeriod()` method
- Trouve le corps le plus massif comme primaire
- Utilise l'équation vis-viva pour calculer le demi-grand axe: a = 1 / (2/r - v²/GM)
- Applique la 3ème loi de Kepler: T = 2π * sqrt(a³ / GM)
- Détection des trajectoires paraboliques/hyperboliques (échappement)
- Formatage automatique des unités (secondes/minutes/heures/jours/années)
- Affiché dans les informations du corps sélectionné

#### 18. Dérive conservation moment [FIXED - 2026-03-23]
**Files:** EpistemotronView.h, EpistemotronView.cpp
**Feature:** Affichage de la dérive de conservation du moment linéaire et angulaire
**Implementation:**
- Ajout de `m_initialLinearMomentum` et `m_initialAngularMomentum`
- Initialisation automatique dans tous les scénarios
- Calcul de la dérive relative (%) pour chaque type de moment
- Affichage dans l'overlay UI:
  ```
  Momentum Conservation:
    Linear: 1.23e+30 kg*m/s
    Angular: 4.56e+38 kg*m²/s
    Linear Drift: +0.0001%
    Angular Drift: -0.0002%
  ```

#### 19. Compilation Debug réussie [FIXED - 2026-03-23]
**Files:** EpistemotronView.cpp, EpistemotronView.h, Epistemotron.rc, Resource.h
**Feature:** Correction de toutes les erreurs de compilation pour une build réussie
**Compilation errors fixed:**
- `CDC::Rectangle` API misuse → Fixed with proper GDI SelectObject pattern
- Missing `Universe.h` include → Added to EpistemotronView.h
- INT_PTR to int conversion → Fixed in OnSimulationConfig()
- `ID_CLOSE_ALL` undefined → Changed to ID_FILE_CLOSE
- Toolbar.bmp path → Fixed with forward slashes
- `IDS_APP_TITLE` undefined → Added to Resource.h
- TOOLBAR definitions → Commented out problematic definitions
- Missing resource IDs → Added ID_BUILD_CLEAN_ALL, ID_DEBUG_END, ID_EDIT_FIND_NEXT, ID_CLASS_ADD, ID_FILE_ADD, ID_RENAME, ID_MOVE
- Duplicate manifest → Commented out in resource file

**Build output:** `W:\Programmation\Epistemotron\x64\Debug\Epistemotron.exe`

### Total Cumulatif: 19 améliorations

---

## Session du 2026-03-23 (Loop 1h - Session 2)

**Job ID:** 08b90c7b | **Fréquence:** Toutes les heures | **Auto-expiration:** 7 jours

### Résumé de la Session 2

**Améliorations complétées cette session: 5**

#### 6. Statistiques d'énergie et conservation [FIXED - 2026-03-23]
**Files:** EpistemotronView.h, EpistemotronView.cpp
**Feature:** Affichage des statistiques d'énergie en temps réel
**Implementation:**
- Ajout de `m_initialTotalEnergy` pour stocker l'énergie de référence
- Modification de `DrawUIOverlay()` pour afficher:
  - Énergie cinétique (J)
  - Énergie potentielle (J)
  - Énergie totale (J)
  - Dérive relative (%)
- Initialisation automatique dans tous les scénarios

**Format d'affichage:**
```
Energy:
  Kinetic:   1.23e+32 J
  Potential: -2.45e+32 J
  Total:     -1.22e+32 J
  Drift:     +0.001%
```

#### 7. Intégrateur Velocity Verlet [ALREADY IMPLEMENTED]
**Status:** Déjà implémenté et accessible via la touche 'I'
**Note:** L'intégrateur Velocity Verlet offre une meilleure conservation de l'énergie que le symplectic Euler pour les systèmes orbitaux.

#### 8. Sélection de corps célestes [FIXED - 2026-03-23]
**Files:** EpistemotronView.h, EpistemotronView.cpp
**Feature:** Clic souris pour sélectionner un corps céleste et voir ses propriétés
**Implementation:**
- Ajout de `m_selectedBodyIndex` et `m_selectedBodyInfo`
- Détection de clic sur un corps dans `OnLButtonDown()`
- Affichage d'un anneau jaune autour du corps sélectionné
- Informations affichées:
  - Masse (kg)
  - Position (X, Y, Z en km)
  - Vélocité (Vx, Vy, Vz en m/s)
  - Énergie cinétique (J)
- Ctrl+clic pour désélectionner

#### 9. Optimisation O(n²) avec 3ème loi de Newton [ALREADY IMPLEMENTED]
**Status:** Déjà implémenté dans `ComputeAccelerations()`
**Note:** La boucle utilise `for (int j = i + 1; j < nMassCount; j++)` pour ne calculer chaque paire qu'une fois, divisant le temps de calcul par 2.

#### 10. Export séquence d'images [FIXED - 2026-03-23]
**Files:** EpistemotronView.h, EpistemotronView.cpp, Resource.h, Epistemotron.rc
**Feature:** Enregistrement de la simulation comme séquence d'images BMP pour création de vidéos
**Implementation:**
- Membres: `m_bRecording`, `m_recordPath`, `m_recordFrameCount`, `m_recordPrefix`
- Méthodes: `StartRecording()`, `StopRecording()`, `SaveCurrentFrame()`
- Commandes menu: ID_RECORDING_START (32787), ID_RECORDING_STOP (32788)
- Fichiers nommés séquentiellement: `prefix_00001.bmp`, `prefix_00002.bmp`, etc.
- Statut d'enregistrement affiché dans l'overlay UI

### Total Cumulatif: 10 améliorations

---

## Session du 2026-03-22 (Loop 1h - Session 1)

**Job ID:** 08b90c7b | **Fréquence:** Toutes les heures | **Auto-expiration:** 7 jours

### Résumé de la Session 1

**Analyses effectuées (3 agents en parallèle):**
1. Moteur physique - Identification de 20+ problèmes
2. Interface utilisateur - Identification de 30+ problèmes
3. Qualité du code - Identification de 15+ problèmes

**Améliorations complétées: 5**

#### 1. Bug vélocité Z dans scénario Galaxie [FIXED - 2026-03-22]
**File:** Science/Universe.cpp:291
**Issue:** `vz = orbitalSpeed * sinInclination` était incorrect. Pour la paramétrisation où z = radius * sinInclination (constant), la dérivée dz/dt = 0.
**Impact:** Les orbites galactiques avaient une composante Z incorrecte, causant des trajectoires non-réalistes.
**Fix:** Changé vz à 0.0 avec commentaire explicatif

#### 2. Optimisation allocation bitmap dans OnDraw [FIXED - 2026-03-22]
**File:** EpistemotronView.cpp:183-208, EpistemotronView.h:95-101
**Issue:** Le bitmap tampon était créé et détruit à chaque appel OnDraw() (jusqu'à 10 fois/seconde), causant fragmentation mémoire et surcharge CPU.
**Impact:** Pertes de performance significatives, surtout sur grands écrans.
**Fix:**
- Ajouté CDC m_memDC, CBitmap m_memBitmap comme membres de CEpistemotronView
- Ajouté EnsureDoubleBuffer() qui réutilise le DC/bitmap existant et ne le redimensionne que si nécessaire
- Modifié OnDraw() pour utiliser EnsureDoubleBuffer() au lieu d'allocation locale
- Nettoyage approprié dans le destructeur

#### 3. Optimisation calculs trigonométriques [FIXED - 2026-03-22]
**File:** EpistemotronView.cpp:310-340
**Issue:** Les fonctions cos/sin pour les rotations de caméra étaient appelées N fois (une fois par corps céleste) dans la boucle de rendu.
**Impact:** Pour 1000 corps, cela signifie 6000 appels trigonométriques par frame (60 000/sec à 10 FPS).
**Fix:** Pré-calculer cosYaw, sinYaw, cosPitch, sinPitch, cosRoll, sinRoll avant la boucle - réduit à 6 appels par frame.

#### 4. Architecture GPU Computing [PREPARED - 2026-03-22]
**Files:** GPU/IGravitySolver.h, CPUGravitySolver.h/.cpp, CUDAGravitySolver.h/.cpp, OpenCLGravitySolver.h/.cpp, GravitySolverFactory.cpp
**Request:** Créer infrastructure pour calculs gravitationnels accélérés GPU
**Status:** Code complet créé, prêt pour test avec GPU
**Architecture:**
- IGravitySolver: Interface abstraite pour swap transparent CPU/GPU
- CPUGravitySolver: Implémentation CPU de référence avec Newton's 3rd law optimization
- CUDAGravitySolver: Kernel CUDA pour NVIDIA GPUs (USE_CUDA)
- OpenCLGravitySolver: Kernel OpenCL pour cross-platform (USE_OPENCL)
- GravitySolverFactory: Auto-détection et sélection du meilleur solver

#### 5. Corrections compilation diverses [FIXED - 2026-03-22]
**Files:** Resource.h, Epistemotron.h, Epistemotron.cpp, Epistemotron.rc, MainFrm.cpp
**Issues fixed:**
- IDS_HELLO undefined → Ajouté dans Resource.h
- m_nAppLook non initialisé → Initialisé dans Epistemotron.h avec {}
- ID_WINDOW_* redéfinitions → Retirés de Resource.h (déjà dans afxres.h)
- Toolbar path incorrect → Corrigé dans Epistemotron.rc
- LoadToolBar avec IDR_MAINFRAME → Corrigé pour utiliser IDR_MAINFRAME_256

### Prochaines Priorités

1. **Fixer le dialog de configuration** - SimConfigDlg incomplet
2. **Ajouter la sélection de corps célestes** - Click sur un corps pour voir ses propriétés
3. **Implémenter Velocity Verlet** - Remplacer Euler par intégrateur symplectique
4. **Ajouter statistiques d'énergie** - Afficher conservation de l'énergie
5. **Optimiser O(n²)** - Utiliser la 3ème loi de Newton pour diviser le temps par 2

### Tâches Futures (requièrent ressources spécifiques)

#### GPU Computing - Implémentation CUDA/OpenCL [PREPARED - Requires GPU for Testing]
**Request:** Implémenter l'utilisation du GPU pour les calculs gravitationnels
**Rationale:**
- Les calculs N-corps sont O(n²) et très parallélisables
- Un GPU peut accélérer les calculs de 100x à 1000x pour de grandes simulations
- Permettra de simuler 100 000+ corps en temps réel

**Fichiers CRÉÉS (prêts à être testés avec GPU):**
- ✅ `GPU/IGravitySolver.h` - Interface abstraite
- ✅ `GPU/CPUGravitySolver.h/.cpp` - Implémentation CPU de référence
- ✅ `GPU/CUDAGravitySolver.h/.cpp` - Implémentation CUDA (kernel inclus)
- ✅ `GPU/OpenCLGravitySolver.h/.cpp` - Implémentation OpenCL (kernel inclus)
- ✅ `GPU/GravitySolverFactory.cpp` - Factory pour création automatique

**Architecture:**
```
IGravitySolver (interface)
    ├── CPUGravitySolver    - CPU fallback, toujours disponible
    ├── CUDAGravitySolver   - NVIDIA GPU (requiert USE_CUDA)
    └── OpenCLGravitySolver - Cross-platform (requiert USE_OPENCL)
```

**Pour activer CUDA:**
1. Installer CUDA Toolkit
2. Ajouter `/DUSE_CUDA` aux préprocesseurs
3. Lier avec `cudart.lib`
4. Configurer VCPROJECT pour inclure les fichiers .cu

**Pour activer OpenCL:**
1. Installer OpenCL SDK (inclus avec drivers GPU)
2. Ajouter `/DUSE_OPENCL` aux préprocesseurs
3. Lier avec `OpenCL.lib`

**Status:** ⏳ CODE PRÊT - EN ATTENTE DE GPU POUR TESTS

**Note:** Le code est complet mais non testé. Un GPU NVIDIA (pour CUDA) ou AMD/Intel (pour OpenCL) est nécessaire pour valider le fonctionnement.

---

## Priorité Critique - Bugs qui doivent être corrigés immédiatement

### 1. Memory Leak - Universe objects not deleted (CRITICAL) [FIXED]

### Améliorations Complétées Cette Session

#### 1. Bug vélocité Z dans scénario Galaxie [FIXED - 2026-03-22]
**File:** Science/Universe.cpp:291
**Issue:** `vz = orbitalSpeed * sinInclination` était incorrect. Pour la paramétrisation où z = radius * sinInclination (constant), la dérivée dz/dt = 0.
**Impact:** Les orbites galactiques avaient une composante Z incorrecte, causant des trajectoires non-réalistes.
**Fix:** Changé vz à 0.0 avec commentaire explicatif

#### 2. Optimisation allocation bitmap dans OnDraw [FIXED - 2026-03-22]
**File:** EpistemotronView.cpp:183-208, EpistemotronView.h:95-101
**Issue:** Le bitmap tampon était créé et détruit à chaque appel OnDraw() (jusqu'à 10 fois/seconde), causant fragmentation mémoire et surcharge CPU.
**Impact:** Pertes de performance significatives, surtout sur grands écrans.
**Fix:**
- Ajouté CDC m_memDC, CBitmap m_memBitmap comme membres de CEpistemotronView
- Ajouté EnsureDoubleBuffer() qui réutilise le DC/bitmap existant et ne le redimensionne que si nécessaire
- Modifié OnDraw() pour utiliser EnsureDoubleBuffer() au lieu d'allocation locale
- Nettoyage approprié dans le destructeur

#### 3. Optimisation calculs trigonométriques [FIXED - 2026-03-22]
**File:** EpistemotronView.cpp:310-340
**Issue:** Les fonctions cos/sin pour les rotations de caméra étaient appelées N fois (une fois par corps céleste) dans la boucle de rendu.
**Impact:** Pour 1000 corps, cela signifie 6000 appels trigonométriques par frame (60 000/sec à 10 FPS).
**Fix:** Pré-calculer cosYaw, sinYaw, cosPitch, sinPitch, cosRoll, sinRoll avant la boucle - réduit à 6 appels par frame.

---

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
