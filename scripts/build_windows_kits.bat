
cd SDK

cmd /c scripts\build_kit_win64_shared.bat Release
cmd /c scripts\build_kit_win64_shared.bat Debug

rem cmd /c scripts\build_kit_win64_static.bat Release
rem cmd /c scripts\build_kit_win64_static.bat Debug

cd ..

