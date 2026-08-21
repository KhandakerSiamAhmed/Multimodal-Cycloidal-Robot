@echo off
title Digital Twin — Local Server
color 0B

echo ================================================
echo   NEMA 17 Digital Twin — Local HTTP Server
echo ================================================
echo.
echo  Web Serial API requires localhost or HTTPS.
echo  This script serves the digital twin from:
echo.
echo    http://localhost:8080/digital_twin.html
echo.
echo  Press Ctrl+C to stop the server.
echo ================================================
echo.

:: Check if Python is available
python --version >nul 2>&1
if %errorlevel% neq 0 (
    echo [ERROR] Python not found in PATH.
    echo Please install Python from https://www.python.org/downloads/
    echo Make sure to check "Add Python to PATH" during installation.
    pause
    exit /b 1
)

:: Start the server on port 8080 in the current directory
echo [OK] Starting server...
echo.
python -m http.server 8080 --bind 127.0.0.1

pause
