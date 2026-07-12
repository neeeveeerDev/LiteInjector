@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat"

echo Compiling Nwxor.dll...
cl.exe /std:c++17 /O2 /EHsc /MT /LD dllmain.cpp /Fe:Nwxor.dll /link d3d11.lib dxgi.lib d3dcompiler.lib dwmapi.lib user32.lib gdi32.lib

echo Compiling liteInjector.exe...
cl.exe /std:c++17 /O2 /EHsc /MT liteInjector.cpp /Fe:liteInjector.exe /link user32.lib

echo.
echo =========================================
echo Build complete! 
echo =========================================
pause