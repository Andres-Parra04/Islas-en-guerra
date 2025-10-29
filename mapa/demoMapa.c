// demoMapa.c
#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <time.h>
#include "mapa.h"

// Convertir códigos de tecla a nuestros comandos 'w','a','s','d','q'
char capturarTecla() {
    if (!_kbhit()) return 0;
    int c = _getch();
    if (c == 0 || c == 0xE0) {
        // Tecla especial — leer el código siguiente
        int c2 = _getch();
        switch (c2) {
            case 72: return 'w'; // Flecha arriba
            case 80: return 's'; // Flecha abajo
            case 75: return 'a'; // Flecha izquierda
            case 77: return 'd'; // Flecha derecha
            default: return 0;
        }
    } else {
        // Tecla normal
        if (c >= 'A' && c <= 'Z') c = c - 'A' + 'a'; // tolower
        return (char)c;
    }
}

int main() {
    Mapa miMapa;
    inicializarMapa(&miMapa);

    // Ajustar consola al tamaño del mapa
    int ancho = COLUMNAS;
    int alto = FILAS + 1; // +1 para la línea de ayuda
    inicializarConsola(ancho, alto);

    clock_t inicio = clock();
    double tiempo = 0.0;
    const int msFrame = 1000 / 15; // ~15 FPS suficiente para consola

    while (1) {
        // Captura no bloqueante de tecla
        char tecla = capturarTecla();
        if (tecla) {
            if (tecla == 'q') break;
            if (tecla == 'w' || tecla == 'a' || tecla == 's' || tecla == 'd') {
                moverJugador(&miMapa, tecla);
            }
        }

        tiempo = (double)(clock() - inicio) / CLOCKS_PER_SEC;
        dibujarMapa(&miMapa, tiempo);

        Sleep(msFrame);
    }

    // Restaurar cursor visible antes de salir
    restaurarCursorVisible();
    return 0;
}
