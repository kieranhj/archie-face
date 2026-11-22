# Lets plain `make` work from PowerShell / cmd on Windows: run recipes in Git's sh,
# default ARCHIESDK to the D: location, and put the MSYS2 UCRT64 bin on PATH
# (the ArchieSDK gcc needs its DLLs). GNU make reads this before Makefile.
ifeq ($(OS),Windows_NT)
SHELL := C:/Program Files/Git/usr/bin/sh.exe
export PATH := C:\msys64\ucrt64\bin;C:\Program Files\Git\usr\bin;$(PATH)
ARCHIESDK ?= D:/tools/archie/ArchieSDK
export ARCHIESDK
endif

include Makefile
