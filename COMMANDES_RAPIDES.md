# 🚀 Commandes Rapides - Guide d'exécution

## 📋 Checklist pré-compilation

### 1. Vérifier l'installation de CMake
```bash
cmake --version
```
**Résultat attendu :** Version 3.10 ou supérieure

### 2. Vérifier l'installation du compilateur C++
```bash
# MinGW
g++ --version

# MSVC (Visual Studio)
cl

# Clang
clang++ --version
```

### 3. Vérifier SFML
```bash
# Vérifier que les fichiers suivants existent :
# - C:\SFML\include\SFML\ (ou chemin personnalisé)
# - C:\SFML\lib\
# - C:\SFML\bin\
```

---

## 🔨 Compilation avec CMake (Recommandé)

### Étape 1 : Créer le dossier build
```bash
mkdir build
cd build
```

### Étape 2 : Configuration CMake

**Si SFML est dans C:\SFML :**
```bash
cmake .. -DCMAKE_PREFIX_PATH=C:/SFML
```

**Si SFML est installé via vcpkg :**
```bash
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake
```

**Si SFML est dans le PATH système :**
```bash
cmake ..
```

### Étape 3 : Compilation
```bash
# Windows (MinGW)
cmake --build .

# Windows (Visual Studio)
cmake --build . --config Release

# Linux/Mac
make
```

### Étape 4 : Vérification
```bash
# Vérifier que l'exécutable existe
dir CasseBriques.exe    # Windows
ls CasseBriques         # Linux/Mac
```

---

## 🎮 Exécution

### Windows
```bash
# Depuis le dossier build
.\CasseBriques.exe

# Ou depuis le dossier racine
.\build\CasseBriques.exe
```

### Linux/Mac
```bash
./CasseBriques
```

### Si erreur "DLL manquantes" (Windows)
```bash
# Copier les DLL depuis C:\SFML\bin\ vers le dossier de l'exécutable
copy C:\SFML\bin\sfml-*.dll build\
```

---

## 🔧 Compilation manuelle (sans CMake)

### Windows (MinGW)
```bash
g++ -std=c++17 ^
    main.cpp src/core/*.cpp src/game/*.cpp src/game_reborn/*.cpp ^
    -I C:/SFML/include ^
    -L C:/SFML/lib ^
    -lsfml-graphics -lsfml-window -lsfml-system ^
    -o CasseBriques.exe
```

### Linux
```bash
g++ -std=c++17 \
    main.cpp src/core/*.cpp src/game/*.cpp src/game_reborn/*.cpp \
    -I /usr/include/SFML \
    -L /usr/lib \
    -lsfml-graphics -lsfml-window -lsfml-system \
    -o CasseBriques
```

### Mac
```bash
g++ -std=c++17 \
    main.cpp src/core/*.cpp src/game/*.cpp src/game_reborn/*.cpp \
    -I /usr/local/include/SFML \
    -L /usr/local/lib \
    -lsfml-graphics -lsfml-window -lsfml-system \
    -o CasseBriques
```

---

## 🧹 Nettoyage

### Supprimer les fichiers de build
```bash
# Windows
rmdir /s /q build

# Linux/Mac
rm -rf build
```

### Recompilation propre
```bash
rm -rf build
mkdir build
cd build
cmake ..
cmake --build .
```

---

## 🐛 Dépannage rapide

### Erreur : "SFML not found"
```bash
# Vérifier le chemin
cmake .. -DCMAKE_PREFIX_PATH=C:/SFML -DCMAKE_VERBOSE_MAKEFILE=ON
```

### Erreur : "undefined reference to sf::..."
```bash
# Vérifier que les bibliothèques sont bien liées
# Vérifier l'ordre des flags : -L puis -l
```

### Erreur : "DLL not found"
```bash
# Copier les DLL dans le même dossier que l'exécutable
# Ou ajouter C:\SFML\bin au PATH
```

### Erreur : "C++17 not supported"
```bash
# Mettre à jour le compilateur
# Ou changer CMAKE_CXX_STANDARD dans CMakeLists.txt
```

---

## 📊 Vérification de la compilation

### Vérifier les warnings
```bash
cmake --build . --verbose
```

### Mode Debug
```bash
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build .
```

### Mode Release (optimisé)
```bash
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

---

## 🔍 Commandes utiles

### Lister les fichiers sources
```bash
# Windows
dir /s /b *.cpp *.h

# Linux/Mac
find . -name "*.cpp" -o -name "*.h"
```

### Vérifier les dépendances (Windows)
```bash
# Utiliser Dependency Walker ou
dumpbin /dependents CasseBriques.exe
```

### Vérifier les dépendances (Linux)
```bash
ldd CasseBriques
```

---

## 📝 Notes importantes

1. **Chemins Windows :** Utiliser des slashes `/` ou des doubles backslashes `\\` dans CMake
2. **DLL SFML :** Toujours copier les DLL dans le dossier de l'exécutable sous Windows
3. **Architecture :** S'assurer que SFML et le compilateur utilisent la même architecture (x86 ou x64)
4. **Version SFML :** Utiliser SFML 2.6+ pour la compatibilité

---

## 🎯 Workflow recommandé

```bash
# 1. Nettoyer
rm -rf build

# 2. Créer build
mkdir build && cd build

# 3. Configurer
cmake .. -DCMAKE_PREFIX_PATH=C:/SFML

# 4. Compiler
cmake --build .

# 5. Exécuter
.\CasseBriques.exe
```

