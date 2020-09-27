@echo off
set src=..\..\..\tools\uGelisFlash_f6721b
copy %src%\Config.ini %src%\Config.ini.bak
copy %src%\images\ai\Config.ini %src%\Config.ini
cd %src%
start uGelisFlash.exe