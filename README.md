# 🎮 Casse-Briques - Projet C++/SFML

Projet de développement de deux versions d'un jeu de casse-briques en C++ avec SFML.

## 📚 Documentation

- **[GUIDE_INSTALLATION.md](GUIDE_INSTALLATION.md)** : Guide complet d'installation et de développement étape par étape
- **[SPECIFICATIONS_TECHNIQUES.md](SPECIFICATIONS_TECHNIQUES.md)** : Spécifications techniques détaillées et architecture du code

## 🚀 Démarrage rapide

### Prérequis

- Compilateur C++ (MinGW, MSVC, ou Clang)
- CMake 3.10+ (recommandé)
- SFML 2.6+

### Installation SFML (Windows)

**Option 1 : Téléchargement manuel**

1. Télécharger SFML depuis https://www.sfml-dev.org/download.php
2. Extraire dans `C:\SFML`
3. Configurer le chemin dans CMakeLists.txt si nécessaire

**Option 2 : Via vcpkg**

```bash
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat
.\vcpkg install sfml:x64-windows
```

### Compilation

```bash
mkdir build
cd build
cmake .. -DCMAKE_PREFIX_PATH=C:/SFML
cmake --build .
```

### Exécution

```bash
# Windows
.\CasseBriques.exe

C:\Users\Abricot\Desktop\Cassebriques\build-mingw\bin>.\CasseBriques.exe !!!!

# Linux/Mac
./CasseBriques
```

## 📁 Structure du projet

```
Cassebriques/
├── src/
│   ├── core/              # Moteur de jeu (GameObject, InputManager)
│   ├── game/              # Casse-Briques classique
│   └── game_reborn/       # Casse-Briques Reborn
├── assets/                # Ressources (textures, fonts)
├── levels/                # Fichiers de niveaux
├── main.cpp
├── CMakeLists.txt
└── README.md
```

## 🎯 Fonctionnalités

### Version Classique

- ✅ Raquette contrôlable
- ✅ Balle avec rebonds
- ✅ Grille de briques
- ✅ Score
- ✅ Conditions victoire/défaite
- ✅ Augmentation progressive de la difficulté

### Version Reborn

- ✅ Canon qui suit la souris
- ✅ Système de tir
- ✅ Briques avec points de vie
- ✅ Changement de couleur selon HP

### Bonus (optionnel)

- ⭐ Collision cercle-AABB précise
- ⭐ AssetManager (singleton)
- ⭐ Chargement de niveaux
- ⭐ Multi-tirs simultanés
- ⭐ Onde de choc (clic droit)

## 📝 Checklist de développement

Voir [GUIDE_INSTALLATION.md](GUIDE_INSTALLATION.md) pour la checklist complète.

## 🐛 Problèmes courants

### SFML non trouvé

Vérifier `CMAKE_PREFIX_PATH` ou les chemins dans CMakeLists.txt

### DLL manquantes (Windows)

Copier les DLL SFML dans le dossier de l'exécutable :

- `sfml-graphics-2.dll`
- `sfml-window-2.dll`
- `sfml-system-2.dll`

## 📖 Ressources

- [Documentation SFML](https://www.sfml-dev.org/documentation/)
- [Tutoriels SFML](https://www.sfml-dev.org/tutorials/)
- [CMake Documentation](https://cmake.org/documentation/)

## 👥 Auteur

Projet développé dans le cadre d'un cours de programmation C++.
