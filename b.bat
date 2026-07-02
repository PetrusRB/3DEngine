@echo off
setlocal EnableDelayedExpansion

title Build System - Ninja

:: ========================================
:: CONFIG
:: ========================================

set "BUILD_DIR=build"
set "GENERATOR=Ninja"
set "BUILD_TYPE=Release"

set "GCC=C:/msys64/mingw64/bin/gcc.exe"
set "GXX=C:/msys64/mingw64/bin/g++.exe"
set "NINJA=ninja"

:: Flags de otimizacao por build type
:: Release: O3 + LTO + avx2 (se disponivel)
:: Debug: O0 + debug info
if /I "%BUILD_TYPE%"=="Release" (
    set "CXX_FLAGS=-O3 -DNDEBUG -flto -march=x86-64-v3"
    set "C_FLAGS=-O3 -DNDEBUG -flto -march=x86-64-v3"
) else (
    set "CXX_FLAGS=-O0 -g3 -D_DEBUG"
    set "C_FLAGS=-O0 -g3 -D_DEBUG"
)

:: Flag de execucao direta
set "DIRECT_RUN=0"
if /I "%~1"=="--direct" set "DIRECT_RUN=1"
if /I "%~1"=="-d" set "DIRECT_RUN=1"

:: Flag para limpar build
set "CLEAN_BUILD=0"
if /I "%~1"=="--clean" set "CLEAN_BUILD=1"
if /I "%~1"=="-c" set "CLEAN_BUILD=1"

:: Flag para build debug
if /I "%~2"=="--debug" set "BUILD_TYPE=Debug"
if /I "%~2"=="-g" set "BUILD_TYPE=Debug"

:: ========================================
:: EXECUTE DIRETO
:: ========================================

if "!DIRECT_RUN!"=="1" (
    echo [INFO] Modo direto - executando sem compilar.
    set "EXE="
    for /r "%BUILD_DIR%" %%f in (*.exe) do (
        if /I not "%%~nxf"=="cmake.exe" (
            if not defined EXE set "EXE=%%f"
        )
    )

    if not defined EXE (
        echo [ERROR] Nenhum executavel encontrado em "%BUILD_DIR%".
        echo Execute sem --direct primeiro para compilar o projeto.
        exit /b 1
    )

    echo [INFO] Executando: !EXE!
    echo.
    "!EXE!"
    exit /b 0
)

:: ========================================
:: INTERFACE
:: ========================================

echo.
echo ==========================================
echo         PROJECT BUILDER [NINJA]
echo ==========================================
echo  Build Type: !BUILD_TYPE!
echo  Generator:  !GENERATOR!
echo  Otimizacao: !CXX_FLAGS!
echo ==========================================
echo.

:: ========================================
:: VALIDATION
:: ========================================

if not exist "%GCC%" (
    echo [ERROR] gcc.exe nao encontrado:
    echo %GCC%
    exit /b 1
)

if not exist "%GXX%" (
    echo [ERROR] g++.exe nao encontrado:
    echo %GXX%
    exit /b 1
)

where ninja >nul 2>&1
if errorlevel 1 (
    echo [ERROR] ninja nao encontrado no PATH.
    echo Instale: scoop install ninja  ou  choco install ninja
    exit /b 1
)

:: ========================================
:: CLEAN (opcional)
:: ========================================

if "!CLEAN_BUILD!"=="1" (
    if exist "%BUILD_DIR%" (
        echo [INFO] Limpando build antiga...
        rmdir /s /q "%BUILD_DIR%"
    )
)

:: ========================================
:: CONFIGURE
:: ========================================

echo [1/3] Configurando CMake...
echo.

:: So reconfigura se nao existe build.ninja
if not exist "%BUILD_DIR%\build.ninja" (
    cmake ^
        -G "%GENERATOR%" ^
        -DCMAKE_BUILD_TYPE=!BUILD_TYPE! ^
        -DCMAKE_C_COMPILER="%GCC%" ^
        -DCMAKE_CXX_COMPILER="%GXX%" ^
        -DCMAKE_C_FLAGS="!C_FLAGS!" ^
        -DCMAKE_CXX_FLAGS="!CXX_FLAGS!" ^
        -B "%BUILD_DIR%" -S .

    if errorlevel 1 (
        echo.
        echo [FAILED] Erro ao configurar o projeto.
        exit /b 1
    )
) else (
    echo [INFO] build.ninja existe, reutilizando configuracao.
)

:: ========================================
:: BUILD
:: ========================================

echo.
echo [2/3] Compilando...
echo.

%NINJA% -C "%BUILD_DIR%"

if errorlevel 1 (
    echo.
    echo [FAILED] Erro na compilacao.
    exit /b 1
)

:: ========================================
:: EXECUTE
:: ========================================

echo.
echo [3/3] Executando:
echo.

set "EXE="
for /r "%BUILD_DIR%" %%f in (*.exe) do (
    if /I not "%%~nxf"=="cmake.exe" (
        if not defined EXE set "EXE=%%f"
    )
)

if not defined EXE (
    echo [WARNING] Nenhum executavel encontrado.
    exit /b 0
)

echo ==========================================
echo Executando: !EXE!
echo ==========================================
echo.

"!EXE!"

endlocal
