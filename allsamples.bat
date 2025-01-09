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
set sample_6=cubemap
set sample_7=collision
set sample_8=background
set sample_9=nowloading
set sample_10=point-light
set sample_11=spot-light
set sample_12=blend
set sample_13=blur

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