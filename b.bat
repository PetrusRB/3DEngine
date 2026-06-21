@echo off
setlocal EnableDelayedExpansion

title Build System

:: ========================================
:: CONFIG
:: ========================================

set "BUILD_DIR=build"
set "GENERATOR=MinGW Makefiles"

set "GCC=C:/msys64/mingw64/bin/gcc.exe"
set "GXX=C:/msys64/mingw64/bin/g++.exe"
set "MAKE=C:/msys64/mingw64/bin/mingw32-make.exe"
set "DIRECT_RUN=0"
if /I "%~1"=="--direct" set "DIRECT_RUN=1"

:: ========================================
:: EXECUTE
:: ========================================

if "!DIRECT_RUN!"=="1" (

  echo [INFO] modo direct foi ativado.
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
:: BUILDZINHA NORMAL
:: ========================================

echo.
echo ==========================================
echo            PROJECT BUILDER
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

if not exist "%MAKE%" (
    echo [ERROR] mingw32-make.exe nao encontrado:
    echo %MAKE%
    exit /b 1
)

:: ========================================
:: CLEAN
:: ========================================

if exist "%BUILD_DIR%" (
    echo [INFO] Limpando build antiga...
    rmdir /s /q "%BUILD_DIR%"
)

:: ========================================
:: CONFIGURE
:: ========================================

echo.
echo [1/3] Configurando CMake...
echo.

cmake ^
-G "%GENERATOR%" ^
-DCMAKE_MAKE_PROGRAM="%MAKE%" ^
-DCMAKE_C_COMPILER="%GCC%" ^
-DCMAKE_CXX_COMPILER="%GXX%" ^
-B "%BUILD_DIR%" .

if errorlevel 1 (
    echo.
    echo [FAILED] Erro ao configurar o projeto.
    exit /b 1
)

:: ========================================
:: BUILD
:: ========================================

echo.
echo [2/3] Compilando...
echo.

cmake --build "%BUILD_DIR%" --parallel

if errorlevel 1 (
    echo.
    echo [FAILED] Erro na compilacao.
    exit /b 1
)

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

