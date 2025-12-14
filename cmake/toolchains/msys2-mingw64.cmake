set(CMAKE_SYSTEM_NAME Windows)

# Toolchain for MSYS2 MinGW-w64 (64-bit)
# Adjust paths if your MSYS2 is installed elsewhere.
set(MSYS2_MINGW64_ROOT "C:/msys64/mingw64" CACHE PATH "MSYS2 MinGW64 root")

set(CMAKE_C_COMPILER   "${MSYS2_MINGW64_ROOT}/bin/gcc.exe"   CACHE FILEPATH "" FORCE)
set(CMAKE_CXX_COMPILER "${MSYS2_MINGW64_ROOT}/bin/g++.exe"   CACHE FILEPATH "" FORCE)
set(CMAKE_RC_COMPILER  "${MSYS2_MINGW64_ROOT}/bin/windres.exe" CACHE FILEPATH "" FORCE)

# Make sure find_package prefers this prefix
set(CMAKE_PREFIX_PATH "${MSYS2_MINGW64_ROOT}" CACHE PATH "" FORCE)


