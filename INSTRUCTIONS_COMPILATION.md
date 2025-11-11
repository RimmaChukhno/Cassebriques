# Instructions de Compilation

## Prérequis

1. **Compilateur C++** : MinGW-w64, MSVC, ou Clang
2. **CMake** : Version 3.10 ou supérieure
3. **SFML** : Version 2.6 ou supérieure

## Installation de SFML (Windows)

### Option 1 : Téléchargement manuel

1. Télécharger SFML depuis https://www.sfml-dev.org/download.php
2. Choisir la version correspondant à votre compilateur (MinGW ou MSVC)
3. Extraire dans `C:\SFML` (ou un autre dossier de votre choix)
4. Structure attendue :
   ```
   C:\SFML\
   ├── include\
   │   └── SFML\
   ├── lib\
   └── bin\
   ```

### Option 2 : Via vcpkg

```powershell
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat
.\vcpkg install sfml:x64-windows
```

## Compilation avec CMake

### Étape 1 : Créer le dossier build

```powershell
mkdir build
cd build
```

### Étape 2 : Configuration CMake

**Si SFML est dans C:\SFML :**
```powershell
cmake .. -DCMAKE_PREFIX_PATH=C:/SFML
```

**Si SFML est installé via vcpkg :**
```powershell
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake
```

**Si SFML est dans le PATH système :**
```powershell
cmake ..
```

### Étape 3 : Compilation

```powershell
cmake --build .
```

### Étape 4 : Copier les DLL (Windows uniquement)

Si vous obtenez une erreur "DLL manquantes", copiez les DLL SFML :

```powershell
copy C:\SFML\bin\sfml-*.dll build\
```

### Étape 5 : Exécution

```powershell
.\CasseBriques.exe
```

## Compilation manuelle (sans CMake)

### Windows (MinGW)

```powershell
g++ -std=c++17 main.cpp src/core/*.cpp src/game/*.cpp src/game_reborn/*.cpp -I C:/SFML/include -L C:/SFML/lib -lsfml-graphics -lsfml-window -lsfml-system -o CasseBriques.exe
```

N'oubliez pas de copier les DLL dans le même dossier que l'exécutable.

## Dépannage

### Erreur : "SFML not found"

Vérifiez que le chemin vers SFML est correct :
```powershell
cmake .. -DCMAKE_PREFIX_PATH=C:/SFML -DCMAKE_VERBOSE_MAKEFILE=ON
```

### Erreur : "DLL not found"

Copiez les DLL depuis `C:\SFML\bin\` vers le dossier de l'exécutable.

### Erreur : "undefined reference to sf::..."

Vérifiez que les bibliothèques sont bien liées dans CMakeLists.txt.

## Structure du projet

```
Cassebriques/
├── src/
│   ├── core/           # Moteur de jeu
│   ├── game/           # Casse-Briques classique
│   └── game_reborn/    # Casse-Briques Reborn
├── assets/             # Ressources
├── levels/             # Fichiers de niveaux
├── main.cpp
└── CMakeLists.txt
```

## Notes

- Assurez-vous que SFML et votre compilateur utilisent la même architecture (x86 ou x64)
- Les chemins dans CMake utilisent des slashes `/` même sous Windows
- Pour le mode Debug, utilisez : `cmake .. -DCMAKE_BUILD_TYPE=Debug`

