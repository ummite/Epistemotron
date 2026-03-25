# TODO - Améliorations Epistemotron

Généré automatiquement par analyse statique du code.

## 🔴 CRITIQUE - Bugs graves

- [x] **Science/Mass.cpp::Randomize()** - Bug de randomisation (rand() - 0.5 toujours positif) - CORIGÉ: chaque axe a maintenant sa propre valeur aléatoire
- [x] **Science/Mass.cpp::EffectuerPasChangementVitesse()** - Code verbeux avec if/else répétés
- [x] **Simulation/Mass.cpp** - Algorithme gravitationnel incorrect (delta^2/dist^2 au lieu de delta/dist)
- [x] **Science/Simulator.cpp:75** - Boucle infinie `while(true)` sans condition de sortie
- [x] **Science/Simulator.cpp:79** - `m_iIteration % (1) == 0` toujours vrai
- [x] **Science/EpistemotronView.cpp** - `ReleaseDC()` manquant après `GetDC()`
- [x] **Science/Universe.cpp** - Fuites mémoire dans `ExportPPM()` (remplacé new[] par std::vector)
- [x] **OutputWnd.cpp** - `ON_WM_WINDOWPOSCHANGING()` déclaré mais non implémenté
- [x] **PropertiesWnd.cpp** - Fuite mémoire `m_fntPropList` non détruit
- [x] **OutputWnd.cpp::OnContextMenu()** - Fuite mémoire CMFCPopupMenu en cas d'échec
- [x] **ClassView.cpp::OnContextMenu()** - Fuite mémoire CMFCPopupMenu en cas d'échec
- [x] **OutputWnd.cpp::OnEditCopy/Clear()** - Fonctions non fonctionnelles (juste MessageBox)
- [x] **EpistemotronView.cpp::OnDraw()** - Simulation bloquante dans OnDraw(), fige l'UI - N/A: simulation s'exécute dans OnTimer(), pas dans OnDraw()
- [x] **EpistemotronView.cpp::RenderUniverse3D()** - Division par zéro possible (m_Z >= cameraDistance)
- [x] **EpistemotronView.cpp::RenderUniverse3D()** - Ellipses non remplies (SetBkColor inefficace)
- [x] **Science/Universe.cpp::LoadBinaryStar()** - Formule de vitesse incorrecte (CORIGÉ: v=sqrt(G*m²/(M*a)))
- [x] **Science/Universe.cpp** - M_PI non portable (CORIGÉ: définition explicite de PI)
- [x] **MainFrm.cpp:220** - ModifyStyle no-op (CORIGÉ: ModifyStyle(0, FWS_PREFIXTITLE))
- [x] **EpistemotronView.cpp** - SetTimer échec non géré (CORIGÉ: vérification retour + TRACE)
- [x] **EpistemotronView.cpp::RenderUniverse3D()** - DeleteObject sur brush invalide (CORIGÉ: track brushValid)

## 🟠 MAJEUR - Visualisation & Animation

- [x] **EpistemotronView.cpp** - Pas de boucle d'animation (SetTimer) - Implémenté (OnTimer avec TIMER_ID)
- [x] **EpistemotronView.cpp** - Pas de vraie 3D, axe Z ignoré dans le rendu - Implémenté (rotation 3D avec pitch/yaw/roll)
- [x] **EpistemotronView.cpp** - Image rechargée depuis disque à chaque rafraîchissement - Implémenté (double buffering avec m_memDC/m_memBitmap)
- [x] **EpistemotronView.cpp** - Pas de contrôle utilisateur (pause, vitesse, reset) - Implémenté (Start/Pause/Resume/Stop/Reset/Step)
- [x] **MainFrm.cpp** - Pas de menu "Simulation" pour Start/Pause/Stop - Implémenté (menu Simulation complet)
- [x] **Universe.cpp::ExportPPM()** - Écrit toujours au même fichier `c:\temp\t.bmp` - CORIGÉ: paramètre p_path ajouté (optionnel, défaut conservé)

## 🟠 MAJEUR - Problèmes de conception

- [x] **Science/Universe.h** - Membres publics brisent l'encapsulation
- [x] **Science/Environment.h** - Classe vide, constantes mal définies (→ constexpr)
- [x] **Science/Universe.h:14** - `void Universe::Randomize();` syntaxe incorrecte
- [x] **EpistemotronDoc.cpp** - Serialize() vide, pas de copy constructor, OnNewDocument() fuit mémoire
- [x] **Simulation/Environment.h** - #define au lieu de constexpr (→ aligné avec Science/)
- [x] **CMakeLists.txt** - Référencement `Simulation/` vs `Science/` incohérent - DOCUMENTÉ: commentaire ajouté expliquant la dualité (Simulation/ pour CMake cross-platform, Science/ pour MFC)
- [x] **FileView.cpp/ClassView.cpp** - Données factices "FakeApp" au lieu de vrai projet - Implémenté (FileView: fichiers réels, ClassView: classes et méthodes complètes)
- [x] **ViewTree.h/cpp** - Classe inutile, wrapper vide de CTreeCtrl - Supprimé (remplacé par CTreeCtrl direct)

## 🟡 MOYEN - Performance

- [x] **Tous .cpp** - Includes redondants `pch.h` + `framework.h`
- [x] **pch.h** - `<afxcontrolbars.h>` inclus deux fois
- [x] **Science/Mass.cpp** - Calcul sqrt redondant - Optimisé (calcul distance² direct)
- [x] **Simulation/Mass.cpp** - Calcul sqrt redondant - Optimisé (aligné avec Science/)
- [x] **MainFrm.cpp** - Magic numbers remplacés par constantes nommées
- [x] **MainFrm.cpp** - Erreur handling améliorée (AttachToTabWnd, UserImages)
- [x] **ClassView.cpp** - Memory leaks fixes (GetSubMenu null check, GetCmdMgr null check)
- [x] **ClassView.cpp** - TODO comments remplacés par placeholders
- [x] **FileView.cpp** - Magic numbers remplacés par constantes nommées
- [x] **FileView.cpp** - Code quality fixes (SetItemState mask, static_cast, etc.)
- [x] **OutputWnd.cpp** - Performance: O(n²) → O(n) string operations in AdjustHorzScroll
- [x] **OutputWnd.cpp** - Clipboard error handling with TRACE logging
- [x] **OutputWnd.cpp** - Font cleanup documentation
- [x] **EpistemotronView.cpp** - Memory: std::vector instead of new[] for trail rendering
- [x] **EpistemotronView.cpp** - Performance: pen/brush caching, vector reserve()
- [x] **EpistemotronView.cpp** - Magic numbers: 30+ named constants
- [x] **EpistemotronView.cpp** - Error handling: null checks, TRACE logging
- [x] **EpistemotronView.cpp::RenderTrailForMasses()** - Trig calculs redondants (CORIGÉ: pre-calc cos/sin)
- [x] **Science/Universe.cpp** - Magic numbers: named constant namespaces for scenarios
- [x] **Science/Universe.cpp** - Performance: ComputeAccelerations() helper, sqrt elimination
- [x] **Science/Universe.cpp** - Code quality: std::sqrt(), constexpr, refactoring
- [x] **Science/Mass.h** - Rule of Five: explicit default for all 5 special members
- [x] **Science/Mass.cpp** - Magic numbers: named constants (RANDOM_POSITION_MAX_KM, etc.)
- [x] **Science/Mass.cpp** - Documentation: Doxygen-style comments, English comments
- [x] **Science/Mass.cpp** - Code quality: clear parameter names, consistent formatting
- [x] **Epistemotron.cpp** - Error handling: AfxOleTerm() on all failure paths
- [x] **Epistemotron.cpp** - Code quality: TODO→NOTE comments, MAX_MRU_FILES constant
- [x] **pch.h** - Formatting: endif comment spacing
- [x] **framework.h** - Include ordering: targetver.h first, explicit windows.h
- [x] **framework.h** - Organization: logical sections, removed 11 blank lines
- [x] **Resource.h** - ID conflicts fixed (IDD_SIM_CONFIG_DLG 313→314)
- [x] **Resource.h** - Missing IDs added (ID_WINDOW_NEW), gaps documented
- [x] **Resource.h** - Organization: section headers, grouped by type
- [x] **SimConfigDlg.h** - Invalid macro removed (DECLARE_DATA_EXCHANGE)
- [x] **SimConfigDlg.cpp** - DDV validation macros (DDV_MinMaxInt, DDV_MinMaxDouble)
- [x] **SimConfigDlg.cpp** - Magic numbers→constants (MIN/MAX_NUM_BODIES, etc.)
- [x] **SimConfigDlg.cpp** - Empty handler removed (OnEnChangeEditDescription)
- [x] **Science/Universe.cpp** - Allocation mémoire x2 inutile dans ExportPPM
- [x] **Science/EpistemotronView.cpp** - Image rechargée depuis disque à chaque rafraîchissement - N/A: fichier n'existe pas (Science/ ne contient que Mass, Universe, Simulator, Environment)

## 🟢 FAIBLE - Qualité du code

- [x] **Science/Mass.h** - Commentaires redondants - Nettoyé (commentaires supprimés pour les membres spéciaux par défaut)
- [x] **Science/Mass.cpp** - Destructeur vide inutile - Déjà supprimé (destructeur par défaut dans l'en-tête)
- [x] **Science/Simulator.cpp** - Code commenté abandonné
- [x] **EpistemotronView.cpp** - Nom de méthode `RefreshThisShit()` inapproprié
- [x] **Epistemotron.cpp** - Clé de registre générique "AppWizard-Generated Applications"
- [x] **Epistemotron.cpp** - `SetAppID` avec "NoVersion"
- [x] **Epistemotron.cpp** - Null pointer delete bug (delete pMainFrame when null)
- [x] **Epistemotron.cpp** - Include Simulator.h inutilisé
- [x] **MainFrm.cpp** - SetDockingWindowIcons() fuite GDI handles
- [x] **MainFrm.cpp** - Toolbar cleanup on LoadToolBar failure
- [x] **MainFrm.cpp** - Unchecked LoadImage() return values
- [x] **MainFrm.cpp** - String loading fallbacks (no more ASSERT-only)
- [x] **ChildFrm.cpp** - `m_wndSplitter` removed (dead code)
- [x] **Science/Environment.h** - #define au lieu de constexpr
- [x] **Science/Environment.h/cpp** - Empty singleton class removed (dead code)
- [x] **Science/Mass.cpp** - Trace() missing const qualifier (compiler error)
- [x] **EpistemotronDoc.cpp** - Missing include for K_NombreEtoile
- [x] **EpistemotronDoc.h/cpp** - Rule of Five (move semantics added)
- [x] **EpistemotronDoc.cpp** - GDI leak in OnDrawThumbnail fixed
- [x] **SimConfigDlg.cpp** - No input validation for numeric fields
- [x] **PropertiesWnd.cpp** - Negative height calculation in AdjustLayout()
- [x] **FileView.cpp** - ImageList dangling pointer on bitmap load failure
- [x] **OutputWnd.cpp** - Null pointer check for AfxGetMainWnd()
- [x] **OutputWnd.cpp** - wcscpy without bounds checking
- [x] **ViewTree.h/cpp** - Useless wrapper removed, replaced with CTreeCtrl

---
## 📋 À FAIRE - Améliorations futures

- [x] **EpistemotronView.cpp** - Ajouter statistiques (énergie) - Implémenté
- [x] **EpistemotronView.cpp** - Ajouter statistiques (moment linéaire et angulaire) - Implémenté
- [x] **EpistemotronView.cpp** - Contrôles caméra (zoom, pan, rotation) - Zoom implémenté
- [x] **EpistemotronView.cpp** - Contrôles caméra (pan) - Implémenté (drag left mouse)
- [x] **EpistemotronView.cpp** - Contrôles caméra (rotation) - Implémenté (right drag: pitch/yaw, middle drag: roll, R key: reset)
- [x] **Science/Universe.cpp** - Intégrateur Velocity Verlet (meilleure conservation énergie) - Implémenté
- [x] **Science/Mass.cpp** - Détection collisions et fusion corps - Déjà implémenté (GetPhysicalRadiusKM, IsCollidingWith, ProcessCollisionsSimple, visual feedback)
- [x] **EpistemotronView.cpp** - Visualisation trajectoires (orbit trails) - Implémenté (500 points max, T key toggle)
- [x] **Science/** - Scénarios prédéfinis (Solar System, Binary Star, Three-Body, Galaxy) - Implémenté avec 'S' key cycle
- [x] **EpistemotronView.cpp** - Export vidéo/image - Implémenté (Export Current Frame: BMP/PNG, Export Sequence: numbered BMP files)
- [x] **FileView.cpp** - Montrer les vrais fichiers du projet - Implémenté (fichiers Epistemotron organisés par catégorie)
- [x] **SimConfigDlg.cpp** - Créer la ressource .rc dans Visual Studio - Implémenté (IDD_SIM_CONFIG_DLG avec tous les contrôles)
- [x] **EpistemotronView.cpp** - Raccourcis clavier Save/Load state - Implémenté (U=Save, L=Load)
- [x] **EpistemotronView.cpp** - Minimap overview panel - Implémenté (M key toggle, 150x150px, shows all bodies + camera view)
- [x] **EpistemotronView.cpp** - Elapsed time display - Implémenté (years/days/hours/minutes/seconds based on iteration * stepSize)
- [x] **EpistemotronView.cpp** - Screenshot hotkey F12 - Implémenté (timestamp filename, auto-saves to screenshots folder)

---
## 🆕 NOUVEAU - Fonctionnalités demandées par les utilisateurs

- [x] **EpistemotronView.cpp** - Graphique temps réel de conservation d'énergie - Implémenté (G key toggle, shows total/kinetic/potential energy lines)
- [x] **EpistemotronView.cpp** - Editor de scénarios personnalisés (ajouter/supprimer corps)
  - [x] Part 1: Dialog UI (CustomScenarioDlg.h/cpp, resource definitions, listbox implementation)
  - [x] Part 2: Menu command integration (ID_SCENARIO_CUSTOM, menu item, handler, Universe::Clear/AddBody)
  - [x] Part 3: Polish dialog display (Z/VZ columns, 8-column format)
  - [x] Part 4: Save/load custom scenarios to file (.esc format with Save/Load buttons)
- [ ] **EpistemotronView.cpp** - Export vidéo (AVI/MKV) au lieu de séquence d'images
- [x] **EpistemotronView.cpp** - Pause à itération spécifique (point d'arrêt) - Implémenté (BreakpointDlg, K key toggle, menu item, UI display)
- [x] **EpistemotronView.cpp** - Contrôles de vitesse variable (0.1x à 10x) - Implémenté (m_speedMultiplier, SpeedUp/SlowDown cycle through 0.1x, 0.5x, 1x, 2x, 5x, 10x)
- [x] **EpistemotronView.cpp** - Sauvegarde/chargement de vues caméra (préréglages) - Implémenté (S=save preset, N=next, B=prev)
- [ ] **EpistemotronView.cpp** - Editor de propriétés des corps (masse, vitesse, position)
- [ ] **EpistemotronView.cpp** - Comparaison de deux univers côte à côte
- [x] **EpistemotronView.cpp** - Mode ralenti autour des événements (collisions) - Implémenté (W key toggle, 10% speed for 60 frames after collision, auto-restore)
- [x] **EpistemotronView.cpp** - Sous-titres pour les corps (noms personnalisables) - Implémenté (Mass.m_Name, D key toggle, auto-generated fallback names)

---
Dernière mise à jour: 2026-03-25 11:00 (Session 46 complete - Mass labels implemented)
