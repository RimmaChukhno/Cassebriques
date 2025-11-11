# Problèmes de Compilation et Solutions

## ✅ Problèmes résolus

1. **Conflit de noms Brick** : Résolu avec des namespaces (`ClassicGame::Brick` et `RebornGame::Brick`)
2. **sf::Color::Orange** : Remplacé par `sf::Color(255, 165, 0)`
3. **Avertissements de conversion** : Corrigés avec `static_cast<float>`

## ⚠️ Problème actuel : Conflit ABI SFML

### Symptôme
Erreurs de linkage LNK2019 : symboles SFML non résolus. SFML a été trouvé mais les bibliothèques ne peuvent pas être liées.

### Cause
- SFML installé via MSYS2/MinGW (dans `C:/msys64/mingw64/lib/cmake/SFML`)
- Compilateur utilisé : MSVC (Visual Studio)
- **Incompatibilité ABI** : Les bibliothèques MinGW ne peuvent pas être liées avec MSVC

### Solutions possibles

#### Solution 1 : Utiliser MinGW pour compiler (Recommandé)

1. Installer MinGW-w64 ou utiliser celui de MSYS2
2. Configurer CMake pour utiliser MinGW :
   ```powershell
   cd build
   cmake .. -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH=C:/msys64/mingw64
   cmake --build .
   ```

#### Solution 2 : Installer SFML pour MSVC

1. Télécharger SFML pour Visual C++ depuis https://www.sfml-dev.org/download.php
2. Extraire dans `C:\SFML`
3. Reconfigurer CMake :
   ```powershell
   cd build
   cmake .. -DCMAKE_PREFIX_PATH=C:/SFML
   cmake --build .
   ```

#### Solution 3 : Utiliser vcpkg (Recommandé pour MSVC)

```powershell
# Installer vcpkg
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat

# Installer SFML pour MSVC
.\vcpkg install sfml:x64-windows

# Configurer CMake avec vcpkg
cd ..\Cassebriques\build
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build . --config Release
```

## 📝 Commandes de compilation selon la solution

### Avec MinGW (Solution 1)
```powershell
cd build
cmake .. -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH=C:/msys64/mingw64
mingw32-make
# Ou
cmake --build .
```

### Avec SFML MSVC (Solution 2)
```powershell
cd build
cmake .. -DCMAKE_PREFIX_PATH=C:/SFML
cmake --build . --config Release
```

### Avec vcpkg (Solution 3)
```powershell
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build . --config Release
```

## 🔍 Vérification

Après compilation réussie, vérifier que l'exécutable existe :
```powershell
dir build\Release\CasseBriques.exe
# ou
dir build\CasseBriques.exe
```

## 📚 Ressources

- [SFML Downloads](https://www.sfml-dev.org/download.php)
- [vcpkg Documentation](https://github.com/Microsoft/vcpkg)
- [CMake Generators](https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html)

