@echo off
mkdir ..\..\build
pushd ..\..\build
cl -MT -nologo -Gm- -GR- -EHa- -Od -Oi -WX -W4 -wd4201 -wd4100 -wd4189 -DHANDMADE_INTERNAL=1 -DHANDMADE_SLOW=1 -DHANDMADE_WIN32=1 -FC -Z7 -FmWin32_main_handmadehero.map c:\handmadehero\HandMadeHero\code\Win32_main_handmadehero.cpp user32.lib Gdi32.lib
popd