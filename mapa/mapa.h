#ifndef MAPA_H
#define MAPA_H

#include <windows.h>

// --- CONSTANTES DE DIMENSIÓN ---
#define MAPA_SIZE 2048    //
#define TILE_SIZE 64      // Tamaño lógico (celda de matriz)
#define GRID_SIZE (MAPA_SIZE / TILE_SIZE) // 32x32 celdas
#define SPRITE_ARBOL 128  // Tamaño visual del BMP de árbol

typedef struct {
    int x;       // Posicion X en el mapa 2048
    int y;       // Posicion Y en el mapa 2048
    float zoom;  // Nivel de zoom
} Camara;

// Funciones Gráficas
void cargarRecursosGraficos();
void dibujarMundo(HDC hdc, RECT rectPantalla, Camara cam);
void explorarMapaGrafico(Camara *cam, int dx, int dy);

#endif