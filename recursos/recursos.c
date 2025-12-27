#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "../mapa/menu.h"
#include "../mapa/mapa.h"
#include "recursos.h"
#include "stdbool.h"
#include <math.h>

// --- Animaciones Lógicas ---
static const Animation gAnimFront = { DIR_FRONT, 4, 6 };
static const Animation gAnimBack  = { DIR_BACK,  4, 6 };
static const Animation gAnimLeft  = { DIR_LEFT,  4, 6 };
static const Animation gAnimRight = { DIR_RIGHT, 4, 6 };

static const Animation *animPorDireccion(Direccion d) {
    switch (d) {
        case DIR_BACK:  return &gAnimBack;
        case DIR_LEFT:  return &gAnimLeft;
        case DIR_RIGHT: return &gAnimRight;
        case DIR_FRONT:
        default:        return &gAnimFront;
    }
}

// --- Utilidades de Grid ---
static int clampInt(int v, int lo, int hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

static int pixelACelda(float px) {
    int c = (int)(px / (float)TILE_SIZE);
    return clampInt(c, 0, GRID_SIZE - 1);
}

static float celdaCentroPixel(int celda) {
    return (float)(celda * TILE_SIZE) + (float)TILE_SIZE * 0.5f;
}

static float celdaAPixel(int celda) {
    return (float)(celda * TILE_SIZE);
}

static int obreroFilaActual(const UnidadObrero *o) {
    return pixelACelda(o->y + 32.0f);
}

static int obreroColActual(const UnidadObrero *o) {
    return pixelACelda(o->x + 32.0f);
}

static void ocupacionActualizarUnidad(int **collision, UnidadObrero *o, int nuevaF, int nuevaC) {
    if (!collision) return;
    // Liberar celda vieja si era ocupada por unidad (2)
    if (o->celdaFila >= 0 && o->celdaCol >= 0) {
        if (collision[o->celdaFila][o->celdaCol] == 2) {
            collision[o->celdaFila][o->celdaCol] = 0;
        }
    }
    // Marcar nueva celda
    if (collision[nuevaF][nuevaC] == 0) {
        collision[nuevaF][nuevaC] = 2;
    }
    o->celdaFila = nuevaF;
    o->celdaCol = nuevaC;
}

static bool buscarCeldaLibreCerca(int **collision, int startF, int startC, int *outF, int *outC) {
    for (int r = 1; r <= 5; r++) {
        for (int df = -r; df <= r; df++) {
            for (int dc = -r; dc <= r; dc++) {
                int f = startF + df, c = startC + dc;
                if (f >= 0 && f < GRID_SIZE && c >= 0 && c < GRID_SIZE) {
                    if (collision[f][c] == 0) {
                        *outF = f; *outC = c; return true;
                    }
                }
            }
        }
    }
    return false;
}

static void obreroLiberarRuta(UnidadObrero *o) {
    if (o->rutaCeldas) free(o->rutaCeldas);
    o->rutaCeldas = NULL;
    o->rutaLen = 0;
    o->rutaIdx = 0;
}

// BFS OPTIMIZADO: Ignora unidades (2) para evitar que se bloqueen entre sí
static bool pathfindBFS(int startF, int startC, int goalF, int goalC, int **collision, int **outRuta, int *outLen) {
    if (startF == goalF && startC == goalC) return false;
    
    int total = GRID_SIZE * GRID_SIZE;
    int *cola = (int*)malloc(total * sizeof(int));
    int *prev = (int*)malloc(total * sizeof(int));
    char *visit = (char*)calloc(total, 1);

    for(int i=0; i<total; i++) prev[i] = -1;

    int head = 0, tail = 0;
    cola[tail++] = startF * GRID_SIZE + startC;
    visit[startF * GRID_SIZE + startC] = 1;

    int dF[] = {-1, 1, 0, 0}, dC[] = {0, 0, -1, 1};
    bool found = false;

    while(head < tail) {
        int cur = cola[head++];
        int cf = cur / GRID_SIZE, cc = cur % GRID_SIZE;
        if (cf == goalF && cc == goalC) { found = true; break; }

        for(int i=0; i<4; i++) {
            int nf = cf + dF[i], nc = cc + dC[i];
            if (nf >= 0 && nf < GRID_SIZE && nc >= 0 && nc < GRID_SIZE) {
                // SOLO los árboles (1) bloquean el paso del BFS
                if (!visit[nf * GRID_SIZE + nc] && collision[nf][nc] != 1) {
                    visit[nf * GRID_SIZE + nc] = 1;
                    prev[nf * GRID_SIZE + nc] = cur;
                    cola[tail++] = nf * GRID_SIZE + nc;
                }
            }
        }
    }

    if (found) {
        int pasos = 0;
        for (int v = goalF * GRID_SIZE + goalC; v != -1; v = prev[v]) pasos++;
        int *ruta = (int*)malloc((pasos-1) * sizeof(int));
        int idx = pasos - 2;
        for (int v = goalF * GRID_SIZE + goalC; v != (startF * GRID_SIZE + startC); v = prev[v]) {
            ruta[idx--] = v;
        }
        *outRuta = ruta;
        *outLen = pasos - 1;
    }

    free(cola); free(prev); free(visit);
    return found;
}

void IniciacionRecursos(struct Jugador *j, const char *Nombre) {
    strcpy(j->Nombre, Nombre);
    j->Comida = 200; j->Oro = 100; j->Madera = 150; j->Piedra = 100;
    for (int i = 0; i < 6; i++) {
        j->obreros[i].x = 400.0f + (i * 70.0f);
        j->obreros[i].y = 400.0f;
        j->obreros[i].moviendose = false;
        j->obreros[i].seleccionado = false;
        j->obreros[i].celdaFila = -1;
        j->obreros[i].celdaCol = -1;
        j->obreros[i].rutaCeldas = NULL;
        j->obreros[i].animActual = animPorDireccion(DIR_FRONT);
    }
}

void actualizarObreros(struct Jugador *j) {
    const float vel = 4.0f;
    int **col = mapaObtenerCollisionMap();
    if (!col) return;

    for (int i = 0; i < 6; i++) {
        UnidadObrero *o = &j->obreros[i];
        
        // Sincronizar ocupación en el primer frame
        if (o->celdaFila == -1) {
            ocupacionActualizarUnidad(col, o, obreroFilaActual(o), obreroColActual(o));
        }

        if (!o->moviendose) continue;

        int nextF, nextC;
        if (o->rutaCeldas && o->rutaIdx < o->rutaLen) {
            int targetCelda = o->rutaCeldas[o->rutaIdx];
            nextF = targetCelda / GRID_SIZE;
            nextC = targetCelda % GRID_SIZE;
        } else {
            o->moviendose = false; continue;
        }

        float tx = celdaCentroPixel(nextC), ty = celdaCentroPixel(nextF);
        float cx = o->x + 32.0f, cy = o->y + 32.0f;
        float vx = tx - cx, vy = ty - cy;
        float dist = sqrtf(vx*vx + vy*vy);

        // Animación y Dirección
        if (dist > 0.1f) {
            if (fabsf(vx) > fabsf(vy)) o->dir = (vx > 0) ? DIR_RIGHT : DIR_LEFT;
            else o->dir = (vy > 0) ? DIR_FRONT : DIR_BACK;
            o->animActual = animPorDireccion(o->dir);
            o->animTick++;
            if (o->animTick >= o->animActual->ticksPerFrame) {
                o->animTick = 0;
                o->frame = (o->frame + 1) % o->animActual->frameCount;
            }
        }

        if (dist <= vel) {
            o->x = tx - 32.0f; o->y = ty - 32.0f;
            o->rutaIdx++;
            int cf = obreroFilaActual(o), cc = obreroColActual(o);
            ocupacionActualizarUnidad(col, o, cf, cc);
            if (o->rutaIdx >= o->rutaLen) {
                o->moviendose = false; obreroLiberarRuta(o);
            }
        } else {
            o->x += (vx / dist) * vel;
            o->y += (vy / dist) * vel;
        }
    }
}

void rtsComandarMovimiento(struct Jugador *j, float mundoX, float mundoY) {
    int **col = mapaObtenerCollisionMap();
    int gF = pixelACelda(mundoY), gC = pixelACelda(mundoX);

    if (col[gF][gC] == 1) { // Si es árbol, buscar cerca
        if (!buscarCeldaLibreCerca(col, gF, gC, &gF, &gC)) return;
    }

    for (int i = 0; i < 6; i++) {
        UnidadObrero *o = &j->obreros[i];
        if (!o->seleccionado) continue;

        int sF = obreroFilaActual(o), sC = obreroColActual(o);
        int *ruta = NULL; int len = 0;
        
        if (pathfindBFS(sF, sC, gF, gC, col, &ruta, &len)) {
            obreroLiberarRuta(o);
            o->rutaCeldas = ruta;
            o->rutaLen = len;
            o->rutaIdx = 0;
            o->moviendose = true;
        }
    }
}

void rtsLiberarMovimientoJugador(struct Jugador *j) {
    for (int i = 0; i < 6; i++) obreroLiberarRuta(&j->obreros[i]);
}