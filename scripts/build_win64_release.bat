
cmd /c scripts\build_windows_kits.bat
mkdir build-win64
cd build-win64

cmake .. -G "Visual Studio 15 2017 Win64" -DCMAKE_BUILD_TYPE=Release -DNO_NET=TRUE -DNO_VR=TRUE -DNO_TESTS=TRUE -DINCLUDE_KITS=TRUE -DCMAKE_PROGRAM_PATH=C:/dev/vcpkg/installed/x64-windows/tools/lua -DCMAKE_TOOLCHAIN_FILE=C:/dev/vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows
cmake --build . --target package --config Release
