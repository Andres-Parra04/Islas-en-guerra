@echo off
echo Cerrando instancias previas del juego...
taskkill /F /IM main.exe >nul 2>&1

echo Compilando...
"C:\Program Files (x86)\Embarcadero\Dev-Cpp\TDM-GCC-64\bin\gcc.exe" main.c batallas/batallas.c edificios/edificios.c guardado/guardado.c mapa/mapa.c mapa/menu.c recursos/navegacion.c recursos/recursos.c recursos/ui_compra.c recursos/ui_embarque.c recursos/ui_entrena.c -o main.exe -lgdi32 -luser32 -lwinmm -lmsimg32

if %errorlevel% neq 0 (
    echo.
    echo !ERROR FATAL DE COMPILACION!
    echo Revisa los mensajes de error arriba.
    pause
    exit /b %errorlevel%
)

echo.
echo !COMPILACION EXITOSA!
echo Iniciando el juego...
start main.exe
