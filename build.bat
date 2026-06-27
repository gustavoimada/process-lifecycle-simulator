@echo off
set "GCC=gcc"

where gcc >nul 2>nul
if errorlevel 1 (
    if exist "C:\Dev-Cpp\MinGW64\bin\gcc.exe" (
        set "GCC=C:\Dev-Cpp\MinGW64\bin\gcc.exe"
    )
)

"%GCC%" main.c -o processo_simulador.exe
