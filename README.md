# Aqua Engine
Game Engine for Windows
## Build
### Add FBX, png files
- test/engine/directx: isu.fbx, isu.png, icon.png
- sample/fbx: isu.fbx. isu.png, ninja.fbx, ninja.fbx
- sample/light: isu.fbx. isu.png, ninja.fbx, ninja.fbx

### Install dependencies
- Install FBXSDK, set `FBXSDK_DIR` for root directory
- Install vcpkg
- Install dependencies with vcpkg
```
vcpkg install --triplet x64-windows
```
### cmake build
```
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=<VCPKG ROOT>/vcpkg/scripts/buildsystems/vcpkg.cmake -DLOCAL_BUILD=ON
cmake --build build
```

## Run Tests
```
cd build && ctest
```

## Run Samples
```
allsamples.bat
```
