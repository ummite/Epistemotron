# TODO - Améliorations Epistemotron

Généré automatiquement par analyse statique du code.

## 🔴 CRITIQUE - Bugs graves

- [x] **Science/Mass.cpp::Randomize()** - Bug de randomisation (rand() - 0.5 toujours positif)
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
- [ ] **EpistemotronView.cpp::OnDraw()** - Simulation bloquante dans OnDraw(), fige l'UI

## 🟠 MAJEUR - Visualisation & Animation

- [ ] **EpistemotronView.cpp** - Pas de boucle d'animation (SetTimer)
- [ ] **EpistemotronView.cpp** - Pas de vraie 3D, axe Z ignoré dans le rendu
- [ ] **EpistemotronView.cpp** - Image rechargée depuis disque à chaque rafraîchissement
- [ ] **EpistemotronView.cpp** - Pas de contrôle utilisateur (pause, vitesse, reset)
- [ ] **MainFrm.cpp** - Pas de menu "Simulation" pour Start/Pause/Stop
- [ ] **Universe.cpp::ExportPPM()** - Écrit toujours au même fichier `c:\temp\t.bmp`

## 🟠 MAJEUR - Problèmes de conception

- [x] **Science/Universe.h** - Membres publics brisent l'encapsulation
- [ ] **Science/Environment.h** - Classe vide, constantes mal définies
- [x] **Science/Universe.h:14** - `void Universe::Randomize();` syntaxe incorrecte
- [ ] **EpistemotronDoc.cpp** - Document vide, architecture Doc/View ignorée
- [ ] **CMakeLists.txt** - Référencement `Simulation/` vs `Science/` incohérent
- [ ] **FileView.cpp/ClassView.cpp** - Données factices "FakeApp" au lieu de vrai projet
- [ ] **ViewTree.h/cpp** - Classe inutile, wrapper vide de CTreeCtrl

## 🟡 MOYEN - Performance

- [x] **Tous .cpp** - Includes redondants `pch.h` + `framework.h`
- [x] **pch.h** - `<afxcontrolbars.h>` inclus deux fois
- [ ] **Science/Mass.cpp** - Calcul sqrt redondant
- [x] **Science/Universe.cpp** - Allocation mémoire x2 inutile dans ExportPPM
- [ ] **Science/EpistemotronView.cpp** - Image rechargée depuis disque à chaque rafraîchissement

## 🟢 FAIBLE - Qualité du code

- [ ] **Science/Mass.h** - Commentaires redondants
- [ ] **Science/Mass.cpp** - Destructeur vide inutile
- [x] **Science/Simulator.cpp** - Code commenté abandonné
- [ ] **EpistemotronView.cpp** - Nom de méthode `RefreshThisShit()` inapproprié
- [ ] **Epistemotron.cpp** - Clé de registre générique "AppWizard-Generated Applications"
- [ ] **Epistemotron.cpp** - `SetAppID` avec "NoVersion"
- [ ] **ChildFrm.cpp** - `m_wndSplitter` déclaré mais jamais utilisé

---
Dernière mise à jour: 2026-03-11
