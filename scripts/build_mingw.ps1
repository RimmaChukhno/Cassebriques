param(
  [ValidateSet("Debug","Release")]
  [string]$Config = "Release"
)

$ErrorActionPreference = "Stop"

$root = (Resolve-Path "$PSScriptRoot\..").Path
Set-Location $root

$msysRoot = "C:\msys64\mingw64"
$msys2Root = "C:\msys64"
$toolchain = Join-Path $root "cmake\toolchains\msys2-mingw64.cmake"
$buildDir = Join-Path $root "build-mingw"

if (-not (Test-Path $msysRoot)) {
  throw "MSYS2 MinGW64 not found at '$msysRoot'. Install MSYS2 or edit scripts/build_mingw.ps1."
}
if (-not (Test-Path $toolchain)) {
  throw "Toolchain file not found at '$toolchain'."
}

$gcc = Join-Path $msysRoot "bin\gcc.exe"
$gxx = Join-Path $msysRoot "bin\g++.exe"
if (-not (Test-Path $gcc) -or -not (Test-Path $gxx)) {
  throw "MinGW compilers not found at '$gcc' and '$gxx'. Install them in MSYS2: pacman -S --needed mingw-w64-x86_64-toolchain"
}

$cmake = "cmake"
$generator = "MinGW Makefiles"
$makeCandidates = @(
  (Join-Path $msysRoot "bin\mingw32-make.exe"),
  (Join-Path $msysRoot "bin\make.exe"),
  (Join-Path $msys2Root "usr\bin\make.exe")
)
$make = $makeCandidates | Where-Object { Test-Path $_ } | Select-Object -First 1

$cmakePath = (Get-Command $cmake -ErrorAction SilentlyContinue | Select-Object -ExpandProperty Source -ErrorAction SilentlyContinue)
if (-not $cmakePath) {
  throw "CMake not found in PATH. Install CMake or add it to PATH (so 'cmake' works)."
}
if (-not (Test-Path $make)) {
  throw "GNU make not found. Install it in MSYS2: pacman -S --needed mingw-w64-x86_64-make"
}

Write-Host "Root: $root"
Write-Host "CMake: $cmakePath"
Write-Host "Make: $make"

Write-Host "Configuring (MinGW) into $buildDir ..."
& $cmake -S . -B $buildDir -G $generator `
  -DCMAKE_TOOLCHAIN_FILE="$toolchain" `
  -DCMAKE_MAKE_PROGRAM="$make" `
  -DSFML_DIR="$msysRoot\lib\cmake\SFML" `
  -DCMAKE_BUILD_TYPE="$Config"

Write-Host "Building ($Config) ..."
& $cmake --build $buildDir -- -j

$exe = Join-Path $buildDir "bin\CasseBriques.exe"
if (-not (Test-Path $exe)) {
  # Fallback if user changed output settings
  $exe = Get-ChildItem -Path $buildDir -Recurse -Filter "CasseBriques.exe" | Select-Object -First 1 | ForEach-Object { $_.FullName }
}
if (-not $exe -or -not (Test-Path $exe)) {
  throw "Built exe not found under '$buildDir'."
}

Write-Host "Running: $exe"
& $exe


