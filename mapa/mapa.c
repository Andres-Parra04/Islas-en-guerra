#include "mapa.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <windows.h>
#include "../recursos/recursos.h"

// Definiciones manuales de rutas
#define RUTA_MAPA "../assets/islas/isla1.bmp"
#define RUTA_MAPA_ALT "assets/islas/isla1.bmp"

#define ARBOL1 "../assets/arboles/arbol1.bmp"
#define ARBOL1_ALT "assets/arboles/arbol1.bmp"

#define ARBOL2 "../assets/arboles/arbol2.bmp"
#define ARBOL2_ALT "assets/arboles/arbol2.bmp"

#define ARBOL3 "../assets/arboles/arbol3.bmp"
#define ARBOL3_ALT "assets/arboles/arbol3.bmp"

#define ARBOL4 "../assets/arboles/arbol4.bmp"
#define ARBOL4_ALT "assets/arboles/arbol4.bmp"

#define obrero_front "../assets/obrero/obrero_front.bmp"
#define obrero_back  "../assets/obrero/obrero_back.bmp"
#define obrero_left  "../assets/obrero/obrero_left.bmp"
#define obrero_right "../assets/obrero/obrero_right.bmp"


static HBITMAP hMapaBmp = NULL;
static HBITMAP hArboles[4] = {NULL};

// Matriz lógica 32x32
int mapaObjetos[GRID_SIZE][GRID_SIZE] = {0}; 

// --- COLISIONES (matriz dinámica int**)
// 0 = libre, 1 = ocupado (árboles u obstáculos)
static int **gCollisionMap = NULL;

static void collisionMapAllocIfNeeded(void) {
    if (gCollisionMap) return;

    gCollisionMap = (int**)malloc(GRID_SIZE * sizeof(int*));
    if (!gCollisionMap) return;

    for (int i = 0; i < GRID_SIZE; i++) {
        // Use calloc to zero-initialize memory (clean map)
        *(gCollisionMap + i) = (int*)calloc(GRID_SIZE, sizeof(int));
        if (!*(gCollisionMap + i)) {
            for (int k = 0; k < i; k++) free(*(gCollisionMap + k));
            free(gCollisionMap);
            gCollisionMap = NULL;
            return;
        }
    }
}

static void collisionMapClear(int value) {
    if (!gCollisionMap) return;
    for (int i = 0; i < GRID_SIZE; i++) {
        int *row = *(gCollisionMap + i);
        for (int j = 0; j < GRID_SIZE; j++) {
            *(row + j) = value;
        }
    }
}

int **mapaObtenerCollisionMap(void) {
    collisionMapAllocIfNeeded();
    return gCollisionMap;
}

void mapaReconstruirCollisionMap(void) {
    collisionMapAllocIfNeeded();
    collisionMapClear(0); // Todo libre al inicio
    for (int f = 0; f < GRID_SIZE; f++) {
        for (int c = 0; c < GRID_SIZE; c++) {
            if (mapaObjetos[f][c] > 0) { // Hay un árbol
                for (int df = 0; df < 2; df++) {
                    for (int dc = 0; dc < 2; dc++) {
                        if (f+df < GRID_SIZE && c+dc < GRID_SIZE)
                            gCollisionMap[f+df][c+dc] = 1; // 1 = ÁRBOL (Impasable)
                    }
                }
            }
        }
    }
}

void mapaLiberarCollisionMap(void) {
    if (!gCollisionMap) return;
    for (int i = 0; i < GRID_SIZE; i++) {
        free(*(gCollisionMap + i));
    }
    free(gCollisionMap);
    gCollisionMap = NULL;
}

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

    // Construir la grilla de colisión a partir de la matriz lógica (movimiento/colisiones)
    mapaReconstruirCollisionMap();

    SelectObject(hdcMem, hOldBmp);
    DeleteDC(hdcMem);
    ReleaseDC(NULL, hdcPantalla);
}

static HBITMAP hObreroBmp[4] = {NULL}; // Front, Back, Left, Right

// Definiciones para obrerro fallback
#define OBRERO_F_ALT "assets/obrero/obrero_front.bmp"
#define OBRERO_B_ALT "assets/obrero/obrero_back.bmp"
#define OBRERO_L_ALT "assets/obrero/obrero_left.bmp"
#define OBRERO_R_ALT "assets/obrero/obrero_right.bmp"

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
    
    // --- AGREGA ESTO PARA LOS OBREROS (CON FALLBACK) ---
    const char *rutasObr[] = {obrero_front, obrero_back, obrero_left, obrero_right};
    const char *rutasObrAlt[] = {OBRERO_F_ALT, OBRERO_B_ALT, OBRERO_L_ALT, OBRERO_R_ALT};

    for(int i = 0; i < 4; i++) {
        hObreroBmp[i] = (HBITMAP)LoadImageA(NULL, rutasObr[i], IMAGE_BITMAP, 64, 64, LR_LOADFROMFILE);
        if (!hObreroBmp[i]) {
             hObreroBmp[i] = (HBITMAP)LoadImageA(NULL, rutasObrAlt[i], IMAGE_BITMAP, 64, 64, LR_LOADFROMFILE);
        }
        
        if (!hObreroBmp[i]) {
            printf("[ERROR] No se pudo cargar el BMP del obrero indice %d. Ruta 1: %s, Ruta 2: %s\n", 
                i, rutasObr[i], rutasObrAlt[i]);
        } else {
            printf("[OK] Obrero BMP %d cargado correctamente.\n", i);
        }
    }

    printf("[DEBUG] Recursos: %d/4 arboles cargados fisicamente.\n", cargados);
    generarBosqueAutomatico();
}
void dibujarObreros(HDC hdcBuffer, struct Jugador *j, Camara cam, int anchoP, int altoP) {
    HDC hdcSprites = CreateCompatibleDC(hdcBuffer);
    for (int i = 0; i < 6; i++) {
        UnidadObrero *o = &j->obreros[i];
        int pantX = (int)((o->x - cam.x) * cam.zoom);
        int pantY = (int)((o->y - cam.y) * cam.zoom);
        int tam = (int)(64 * cam.zoom);


        // LOG DE POSICIÓN (Solo para el primer obrero para no saturar)
        // if (i == 0) {
        //     printf("[DEBUG] Obrero 0 -> Mundo(%.1f, %.1f) | Pantalla(%d, %d) | Zoom: %.2f\n", 
        //            o->x, o->y, pantX, pantY, cam.zoom);
        // }

        if (pantX + tam > 0 && pantX < anchoP && pantY + tam > 0 && pantY < altoP) {
            SelectObject(hdcSprites, hObreroBmp[o->dir]);
            TransparentBlt(hdcBuffer, pantX, pantY, tam, tam, hdcSprites, 0, 0, 64, 64, RGB(255, 255, 255));
            
            if (o->seleccionado) { // Círculo de selección
                HBRUSH nullBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
                HPEN verde = CreatePen(PS_SOLID, 2, RGB(0, 255, 0));
                SelectObject(hdcBuffer, nullBrush);
                SelectObject(hdcBuffer, verde);
                Ellipse(hdcBuffer, pantX, pantY + tam - 10, pantX + tam, pantY + tam + 5);
                DeleteObject(verde);
            }
        }
    }
    DeleteDC(hdcSprites);
    }

void dibujarMundo(HDC hdc, RECT rect, Camara cam, struct Jugador *pJugador) {
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
    // Dentro de dibujarMundo, después de dibujar los árboles:

    // 3. DIBUJAR OBREROS (Ahora dentro del búfer)
    // Llamamos a la función usando el puntero del jugador
    dibujarObreros(hdcBuffer, pJugador, cam, anchoP, altoP);

    BitBlt(hdc, 0, 0, anchoP, altoP, hdcBuffer, 0, 0, SRCCOPY);

    // Limpieza
    DeleteDC(hdcSprites);
    SelectObject(hdcMapa, hOldMapa); DeleteDC(hdcMapa);
    SelectObject(hdcBuffer, hOldBuffer); DeleteObject(hbmBuffer); DeleteDC(hdcBuffer);
}

