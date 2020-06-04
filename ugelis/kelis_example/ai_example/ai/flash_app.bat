@echo off
set src=..\..\..\tools\uGelisFlash_f6721b
copy %src%\images\ai\ai.bin %src%\images\ai\ai.bin.bak
copy mdk_80\ai.bin %src%\images\ai\ai.bin
copy %src%\Config.ini %src%\Config.ini.bak
copy %src%\images\ai\Config.system.ini %src%\Config.ini
cd %src%
start uGelisFlash.exe