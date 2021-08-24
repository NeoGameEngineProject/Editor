
cd C:\dev\

git clone https://github.com/microsoft/vcpkg.git
cd vcpkg

bootstrap-vcpkg.bat -disableMetrics
vcpkg.exe install qt5:x64-windows qt5-winextras:x64-windows
