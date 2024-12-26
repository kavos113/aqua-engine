@echo off
setlocal enabledelayedexpansion

set bin=cmake-build-debug
set time=5

make build

set sample_1=display
set sample_2=triangle
set sample_3=texture
set sample_4=fbx
set sample_5=light

set i=1
:SAMPLE_LOOP
set sam=!sample_%i%!
if defined sam (
    echo Running %bin%\sample\%sam%\sample-%sam%.exe
    cd %bin%\sample\%sam%
    sample-%sam%.exe
    cd ../../../
    set /a i+=1
    goto :SAMPLE_LOOP
)