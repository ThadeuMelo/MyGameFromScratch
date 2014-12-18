@echo off
mkdir ..\..\build
pushd ..\..\build
cl -Zi c:\handmadehero\HandMadeHero\code\Win32_main_handmadehero.cpp user32.lib Gdi32.lib
popd