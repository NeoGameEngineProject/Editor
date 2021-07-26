
rem scripts\install_vcpkg.bat

mkdir build
cd build
cmake .. -DNO_NET=TRUE -DNO_VR=TRUE -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PROGRAM_PATH=C:/dev/vcpkg/installed/x64-windows/tools/lua -DCMAKE_TOOLCHAIN_FILE=C:/dev/vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows
pause
