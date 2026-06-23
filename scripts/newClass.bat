@REM creates the cpp and hpp files for a given class name at src/.. and include/..
@echo off

if "%~1"=="" (
    echo Usage: newclass name
    exit /b 1
)

type nul > "src\%~1.cpp"
type nul > "include\%~1.hpp"

echo Created:
echo   src\%~1.cpp
echo   include\%~1.hpp