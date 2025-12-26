#include "mapa.h"
#include <stdio.h>
#include <time.h>

// Definiciones manuales de rutas
#define RUTA_MAPA "../assets/mapaDemo2.bmp"
#define RUTA_MAPA_ALT "assets/mapaDemo2.bmp"

#define ARBOL1 "../assets/arboles/arbol1.bmp"
#define ARBOL1_ALT "assets/arboles/arbol1.bmp"

#define ARBOL2 "../assets/arboles/arbol2.bmp"
#define ARBOL2_ALT "assets/arboles/arbol2.bmp"

#define ARBOL3 "../assets/arboles/arbol3.bmp"
#define ARBOL3_ALT "assets/arboles/arbol3.bmp"

#define ARBOL4 "../assets/arboles/arbol4.bmp"
#define ARBOL4_ALT "assets/arboles/arbol4.bmp"

static HBITMAP hMapaBmp = NULL;
static HBITMAP hArboles[4] = {NULL};

// Matriz lógica 32x32
int mapaObjetos[GRID_SIZE][GRID_SIZE] = {0}; 

void generarBosqueAutomatico() {
    if (!hMapaBmp) return;

    HDC hdcPantalla = GetDC(NULL);
    HDC hdcMem = CreateCompatibleDC(hdcPantalla);
    HBITMAP hOldBmp = (HBITMAP)SelectObject(hdcMem, hMapaBmp);

    // Requisito: Aritmética de punteros para manejar la matriz
    int (*ptrMatriz)[GRID_SIZE] = mapaObjetos;
    srand((unsigned int)time(NULL));
    int contador = 0;

    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            // Analizamos el píxel central de cada celda de 64x64
            int px = (j * TILE_SIZE) + (TILE_SIZE / 2);
            int py = (i * TILE_SIZE) + (TILE_SIZE / 2);

            COLORREF color = GetPixel(hdcMem, px, py);
            if (color == CLR_INVALID) continue;

            BYTE r = GetRValue(color);
            BYTE g = GetGValue(color);
            BYTE b = GetBValue(color);

            // Detección de color verde para mapaDemo2.bmp
            if (g > r && g > b && g > 45) { 
                if ((rand() % 100) < 25) { 
                    // Acceso por punteros: *(base + desplazamiento)
                    *(*(ptrMatriz + i) + j) = (rand() % 4) + 1; 
                    contador++;
                }
            }
        }
    }
    printf("[DEBUG] Logica: %d arboles registrados en la matriz con punteros.\n", contador);

    SelectObject(hdcMem, hOldBmp);
    DeleteDC(hdcMem);
    ReleaseDC(NULL, hdcPantalla);
}

void cargarRecursosGraficos() {
    // 1. Cargar Mapa Base (Intento doble)
    hMapaBmp = (HBITMAP)LoadImageA(NULL, RUTA_MAPA, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
    if (!hMapaBmp) hMapaBmp = (HBITMAP)LoadImageA(NULL, RUTA_MAPA_ALT, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);

    // 2. Cargar cada árbol individualmente con sistema de respaldo (Fallback)
    const char* rutasPrincipales[] = {ARBOL1, ARBOL2, ARBOL3, ARBOL4};
    const char* rutasAlternas[] = {ARBOL1_ALT, ARBOL2_ALT, ARBOL3_ALT, ARBOL4_ALT};

    int cargados = 0;
    for(int i = 0; i < 4; i++) {
        // Intento 1: Con "../"
        hArboles[i] = (HBITMAP)LoadImageA(NULL, rutasPrincipales[i], IMAGE_BITMAP, SPRITE_ARBOL, SPRITE_ARBOL, LR_LOADFROMFILE);
        
        // Intento 2: Sin "../" (si el primer intento falló)
        if (!hArboles[i]) {
            hArboles[i] = (HBITMAP)LoadImageA(NULL, rutasAlternas[i], IMAGE_BITMAP, SPRITE_ARBOL, SPRITE_ARBOL, LR_LOADFROMFILE);
        }

        if (hArboles[i]) cargados++;
    }
    
    printf("[DEBUG] Recursos: %d/4 arboles cargados fisicamente.\n", cargados);
    generarBosqueAutomatico();
}

void dibujarMundo(HDC hdc, RECT rect, Camara cam) {
    if (!hMapaBmp) return;

    int anchoP = rect.right - rect.left;
    int altoP = rect.bottom - rect.top;

    HDC hdcBuffer = CreateCompatibleDC(hdc);
    HBITMAP hbmBuffer = CreateCompatibleBitmap(hdc, anchoP, altoP);
    HBITMAP hOldBuffer = (HBITMAP)SelectObject(hdcBuffer, hbmBuffer);

    HDC hdcMapa = CreateCompatibleDC(hdc);
    HBITMAP hOldMapa = (HBITMAP)SelectObject(hdcMapa, hMapaBmp);

    // 1. Dibujar Suelo (Mapa base)
    SetStretchBltMode(hdcBuffer, HALFTONE);
    StretchBlt(hdcBuffer, 0, 0, anchoP, altoP, 
               hdcMapa, cam.x, cam.y, (int)(anchoP/cam.zoom), (int)(altoP/cam.zoom), SRCCOPY);

    // 2. Dibujar Árboles con MATRIZ y PUNTEROS
    HDC hdcSprites = CreateCompatibleDC(hdc);
    int (*ptrFila)[GRID_SIZE] = mapaObjetos;

    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            // Lectura mediante aritmética de punteros
            int tipo = *(*(ptrFila + i) + j); 

            if (tipo >= 1 && tipo <= 4 && hArboles[tipo-1] != NULL) {
                int mundoX = j * TILE_SIZE;
                int mundoY = i * TILE_SIZE;

                // Ajuste visual (centrar 128x128 sobre 64x64)
                int dibX = mundoX - (SPRITE_ARBOL - TILE_SIZE) / 2;
                int dibY = mundoY - (SPRITE_ARBOL - TILE_SIZE);

                int pantX = (int)((dibX - cam.x) * cam.zoom);
                int pantY = (int)((dibY - cam.y) * cam.zoom);
                int tamZoom = (int)(SPRITE_ARBOL * cam.zoom);

                if (pantX + tamZoom > 0 && pantX < anchoP && pantY + tamZoom > 0 && pantY < altoP) {
                    SelectObject(hdcSprites, hArboles[tipo - 1]);
                    // Transparencia (Blanco)
                    TransparentBlt(hdcBuffer, pantX, pantY, tamZoom, tamZoom,
                                   hdcSprites, 0, 0, SPRITE_ARBOL, SPRITE_ARBOL, RGB(255, 255, 255));
                }
            }
        }
    }

    BitBlt(hdc, 0, 0, anchoP, altoP, hdcBuffer, 0, 0, SRCCOPY);

    // Limpieza
    DeleteDC(hdcSprites);
    SelectObject(hdcMapa, hOldMapa); DeleteDC(hdcMapa);
    SelectObject(hdcBuffer, hOldBuffer); DeleteObject(hbmBuffer); DeleteDC(hdcBuffer);
}