@echo off
mkdir ..\..\build
pushd ..\..\build
cl -Zi c:\handmadehero\HandMadeHero\code\main_handmadehero.cpp user32.lib Gdi32.lib
popd