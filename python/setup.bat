@echo off
REM Simple setup script for Python gRPC File Service
REM Run this script to set up and run the project

echo ============================================
echo   gRPC File Service - Python Setup
echo ============================================
echo.

REM Check Python
python --version >nul 2>&1
if errorlevel 1 (
    echo ERROR: Python not found. Please install Python 3.8+
    pause
    exit /b 1
)

echo [1/3] Installing Python dependencies...
pip install grpcio grpcio-tools

echo.
echo [2/3] Generating gRPC code from proto file...
python -m grpc_tools.protoc -I../proto --python_out=. --grpc_python_out=. ../proto/file_service.proto

echo.
echo [3/3] Creating storage directory...
if not exist "file_storage" mkdir file_storage

echo.
echo ============================================
echo   Setup Complete!
echo ============================================
echo.
echo To run the server:
echo   python server.py
echo.
echo To run the client (in another terminal):
echo   python client.py
echo.
pause
