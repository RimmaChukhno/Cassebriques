# Guide d'Installation et de Développement - Casse-Briques

## 📋 Vue d'ensemble du projet

Ce projet consiste à développer deux versions d'un jeu de casse-briques en C++ avec SFML :
1. **Casse-Briques** (version classique avec raquette)
2. **Casse-Briques Reborn** (version avec canon)

---

## 🔧 ÉTAPE 1 : Installation de l'environnement de développement

### 1.1 Installer un compilateur C++

**Option A : MinGW-w64 (Recommandé pour Windows)**
- Télécharger MinGW-w64 depuis : https://www.mingw-w64.org/downloads/
- Ou utiliser MSYS2 : https://www.msys2.org/
- Ajouter le chemin `bin` de MinGW à la variable d'environnement PATH

**Option B : Visual Studio (avec MSVC)**
- Installer Visual Studio Community : https://visualstudio.microsoft.com/
- Cocher "Développement Desktop en C++" lors de l'installation

**Option C : Clang**
- Télécharger depuis : https://clang.llvm.org/

### 1.2 Installer CMake (optionnel mais recommandé)

- Télécharger CMake : https://cmake.org/download/
- Version minimale : 3.10
- Ajouter CMake au PATH

### 1.3 Installer SFML

**Méthode 1 : Téléchargement manuel (Windows)**
1. Aller sur https://www.sfml-dev.org/download.php
2. Télécharger SFML 2.6.x (ou version récente) pour votre compilateur
3. Extraire dans un dossier (ex: `C:\SFML`)
4. Structure attendue :
   ```
   C:\SFML\
   ├── include\
   │   └── SFML\
   ├── lib\
   └── bin\
   ```

**Méthode 2 : Via vcpkg (Recommandé)**
```bash
# Installer vcpkg
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat

# Installer SFML
.\vcpkg install sfml:x64-windows
```

**Méthode 3 : Via Conan (Alternative)**
```bash
pip install conan
conan install sfml/2.6.0@ -g cmake
```

### 1.4 Configurer l'éditeur de code

**Visual Studio Code :**
- Installer l'extension "C/C++" (Microsoft)
- Installer l'extension "CMake Tools" (optionnel)

**CLion / Visual Studio :**
- Support natif pour CMake

---

## 📁 ÉTAPE 2 : Structure du projet

### 2.1 Créer la structure de dossiers

```
Cassebriques/
├── src/
│   ├── core/              # Moteur de jeu (Étape 1)
│   │   ├── GameObject.h
│   │   ├── GameObject.cpp
│   │   ├── InputManager.h
│   │   └── InputManager.cpp
│   ├── game/              # Jeu Casse-Briques classique
│   │   ├── Paddle.h
│   │   ├── Paddle.cpp
│   │   ├── Ball.h
│   │   ├── Ball.cpp
│   │   ├── Brick.h
│   │   ├── Brick.cpp
│   │   └── Game.h
│   │   └── Game.cpp
│   └── game_reborn/       # Jeu Casse-Briques Reborn
│       ├── Cannon.h
│       ├── Cannon.cpp
│       ├── Projectile.h
│       ├── Projectile.cpp
│       ├── Brick.h
│       ├── Brick.cpp
│       └── GameReborn.h
│       └── GameReborn.cpp
├── assets/                # Ressources (images, sons, etc.)
│   ├── textures/
│   └── fonts/
├── levels/                # Fichiers de niveaux (bonus)
│   └── level1.txt
├── main.cpp               # Point d'entrée principal
├── CMakeLists.txt         # Configuration CMake
└── README.md
```

---

## 🎮 ÉTAPE 3 : Développement - Partie 1 : Mini moteur de jeu

### 3.1 GameObject (Base pour tous les objets du jeu)

**Fonctionnalités requises :**
- Position (x, y)
- Taille (width, height) ou rayon (pour cercle)
- Vitesse (vx, vy)
- Rotation (angle)
- Zone de collision AABB (Axis-Aligned Bounding Box)
- Méthode de détection de collision avec un autre GameObject
- Méthode de rendu (draw)

**Structure AABB :**
```cpp
struct AABB {
    float left, top, right, bottom;
};
```

### 3.2 InputManager (Gestionnaire d'entrées)

**Fonctionnalités requises :**
- Écoute des événements clavier (sf::Keyboard)
- Écoute des événements souris (sf::Mouse)
- Système de callbacks pour lier des fonctions à des actions
- Support pour plusieurs callbacks par action

**Actions à gérer :**
- Touches clavier (ex: E, Space, etc.)
- Clic gauche souris
- Clic droit souris
- Position de la souris

---

## 🎯 ÉTAPE 4 : Développement - Partie 2A : Casse-Briques Classique

### 4.1 Initialisation du jeu

- Fenêtre SFML (ex: 800x600 ou 1024x768)
- Raquette (rectangle) en bas de l'écran
- Balle (cercle) au centre ou sur la raquette
- Grille de briques en haut de l'écran

### 4.2 Mécaniques de gameplay

**Raquette :**
- Contrôle avec flèches gauche/droite ou A/D
- Mouvement horizontal uniquement
- Collision avec la balle

**Balle :**
- Vitesse initiale définie
- Rebonds sur :
  - Murs gauche, droit, haut
  - Raquette (angle selon point d'impact)
  - Briques
- Perdue si sortie par le bas

**Briques :**
- Disposition en grille
- Destruction au contact de la balle
- Score incrémenté à chaque destruction

### 4.3 Conditions de victoire/défaite

- **Victoire :** Toutes les briques détruites
- **Défaite :** Balle sortie par le bas
- Affichage du score

### 4.4 Augmentation progressive de la difficulté

- Vitesse de la balle qui augmente avec le temps ou le score

---

## 🚀 ÉTAPE 5 : Développement - Partie 2B : Casse-Briques Reborn

### 5.1 Initialisation du jeu

- Canon (rectangle) en bas de l'écran
- Grille de briques sur la moitié supérieure
- Chaque case = une brique destructible

### 5.2 Mécaniques de gameplay

**Canon :**
- Suit la position de la souris
- Rotation pour pointer vers la souris
- Calcul de l'angle : `atan2(mouseY - cannonY, mouseX - cannonX)`

**Tir :**
- Clic gauche = tir d'une balle (cercle)
- Direction = angle du canon
- Vitesse constante et rapide
- Une seule balle active à la fois (pas de double tir)

**Projectile (Balle) :**
- Rebonds sur murs gauche, droit, haut
- Collision avec brique = rebond + dégâts à la brique
- Perdue si sortie par le bas

**Briques :**
- Points de vie multiples (ex: 1-3 HP)
- Changement de couleur selon HP restant
- Affichage optionnel du nombre de HP
- Disparition quand HP = 0

---

## ⚙️ ÉTAPE 6 : Fonctionnalités bonus (optionnelles)

### 6.1 Collision cercle-AABB

- Implémenter une détection précise entre un cercle (balle) et un rectangle (brique)
- Gérer les cas de collision sur les bords et coins

### 6.2 AssetManager (Singleton)

- Chargement et gestion des textures
- Chargement et gestion des polices
- Chargement et gestion des sons
- Éviter les chargements multiples

### 6.3 Chargement de niveaux via fichier texte

**Format exemple :**
```
5 10          # 5 lignes, 10 colonnes
3 2 1 2 3 2 1 2 3 2    # HP de chaque brique
2 1 2 1 2 1 2 1 2 1
...
```

### 6.4 Multi-tirs simultanés

- Permettre plusieurs projectiles en même temps (limite : ex: 3)
- Gestion des collisions pour chaque projectile
- Système de cooldown ou limitation

### 6.5 Clic droit = Onde de choc

- Attaque de zone rectangulaire
- Direction = direction du canon
- Repousse toutes les balles touchées
- Effet visuel (rectangle animé)

---

## 🔨 ÉTAPE 7 : Configuration du build (CMakeLists.txt)

### 7.1 Configuration de base

- Version CMake minimale : 3.10
- Standard C++ : C++17 ou supérieur
- Lien avec SFML (graphics, window, system)
- Configuration des chemins d'includes
- Configuration des chemins de bibliothèques

### 7.2 Configuration selon l'OS

- Windows : .dll dans le dossier de sortie
- Linux : gestion des .so
- macOS : gestion des .dylib

---

## 📝 ÉTAPE 8 : Compilation et exécution

### 8.1 Avec CMake (Recommandé)

```bash
mkdir build
cd build
cmake .. -DCMAKE_PREFIX_PATH=C:/SFML  # Ajuster selon votre installation
cmake --build .
```

### 8.2 Compilation manuelle (MinGW)

```bash
g++ -std=c++17 main.cpp src/**/*.cpp -I C:/SFML/include -L C:/SFML/lib -lsfml-graphics -lsfml-window -lsfml-system -o cassebriques.exe
```

### 8.3 Exécution

- Copier les DLL SFML dans le dossier de l'exécutable (Windows)
- Ou configurer le PATH pour inclure le dossier bin de SFML

---

## ✅ Checklist de développement

### Phase 1 : Moteur de jeu
- [ ] GameObject avec position, taille, vitesse, rotation
- [ ] AABB pour collisions
- [ ] Détection de collision entre GameObjects
- [ ] InputManager avec callbacks
- [ ] Tests de base du moteur

### Phase 2A : Casse-Briques classique
- [ ] Fenêtre SFML initialisée
- [ ] Raquette contrôlable
- [ ] Balle avec rebonds
- [ ] Grille de briques
- [ ] Collisions balle-briques
- [ ] Score
- [ ] Conditions victoire/défaite
- [ ] Augmentation progressive de la difficulté

### Phase 2B : Casse-Briques Reborn
- [ ] Canon qui suit la souris
- [ ] Rotation du canon
- [ ] Système de tir
- [ ] Projectile avec rebonds
- [ ] Briques avec points de vie
- [ ] Changement de couleur selon HP
- [ ] Limitation à une balle active

### Phase 3 : Bonus (optionnel)
- [ ] Collision cercle-AABB précise
- [ ] AssetManager (singleton)
- [ ] Chargement de niveaux
- [ ] Multi-tirs
- [ ] Onde de choc (clic droit)

---

## 🐛 Problèmes courants et solutions

### Problème : SFML non trouvé
**Solution :** Vérifier CMAKE_PREFIX_PATH ou les chemins d'includes/libs

### Problème : DLL manquantes (Windows)
**Solution :** Copier sfml-graphics-2.dll, sfml-window-2.dll, sfml-system-2.dll dans le dossier de l'exécutable

### Problème : Collisions imprécises
**Solution :** Vérifier la logique AABB et les calculs de collision cercle-rectangle

### Problème : Performance
**Solution :** Optimiser les détections de collision (spatial partitioning si nécessaire)

---

## 📚 Ressources utiles

- Documentation SFML : https://www.sfml-dev.org/documentation/
- Tutoriels SFML : https://www.sfml-dev.org/tutorials/
- CMake Documentation : https://cmake.org/documentation/

---

## 🎯 Prochaines étapes

1. Installer l'environnement (Étape 1)
2. Créer la structure du projet (Étape 2)
3. Développer le moteur de jeu (Étape 3)
4. Implémenter Casse-Briques classique (Étape 4)
5. Implémenter Casse-Briques Reborn (Étape 5)
6. Ajouter les fonctionnalités bonus (Étape 6)
7. Tester et déboguer
8. Préparer la présentation

