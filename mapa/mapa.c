#include "mapa.h"
#include "menu.h"
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <stdio.h>
#include <string.h>
#include "../recursos/recursos.h"
#include <stdbool.h>

// --- CONFIGURACIÓN DE GENERACIÓN ---
#define NUM_RECURSOS 0      // Sin oro en el mapa
#define NUM_COMIDA 40
#define NUM_ARBOLES 150

// --- COLORES ---
#define COLOR_AGUA_BG 1
#define COLOR_AGUA_FG 9
#define COLOR_TIERRA_BG 2
#define COLOR_TIERRA_FG 10
#define COLOR_ARBOL 6
#define COLOR_MINA_BG 8
#define COLOR_MINA_FG 15
#define COLOR_COMIDA 14
#define COLOR_ORO 14
#define COLOR_CAMP_BG 4
#define COLOR_CAMP_FG 15
#define COLOR_CASA_BG 5
#define COLOR_CASA_FG 15

// Variables globales para el offset (vista de cámara)
int offset_f = 0;
int offset_c = 0;

// ============================================================================
// SISTEMA DE MINAS
// ============================================================================

#define MAX_MINAS 5
#define GENERACION_PIEDRA 10
#define GENERACION_ORO 5
#define TIEMPO_GENERACION 60000

typedef struct {
    int fila;
    int columna;
    int piedra_acumulada;
    int oro_acumulado;
    DWORD ultimo_generacion;
    int activa;
} Mina;

Mina minas[MAX_MINAS];
int total_minas = 0;

void inicializarSistemaMinas(void) {
    int i;
    for (i = 0; i < MAX_MINAS; i++) {
        minas[i].fila = -1;
        minas[i].columna = -1;
        minas[i].piedra_acumulada = 0;
        minas[i].oro_acumulado = 0;
        minas[i].ultimo_generacion = GetTickCount();
        minas[i].activa = 0;
    }
    total_minas = 0;
}

void registrarMina(int fila, int columna) {
    if (total_minas < MAX_MINAS) {
        minas[total_minas].fila = fila;
        minas[total_minas].columna = columna;
        minas[total_minas].piedra_acumulada = GENERACION_PIEDRA;
        minas[total_minas].oro_acumulado = GENERACION_ORO;
        minas[total_minas].ultimo_generacion = GetTickCount();
        minas[total_minas].activa = 1;
        total_minas++;
    }
}

void actualizarMinas(void) {
    int i;
    DWORD tiempo_actual = GetTickCount();
    
    for (i = 0; i < total_minas; i++) {
        if (minas[i].activa) {
            if (tiempo_actual - minas[i].ultimo_generacion >= TIEMPO_GENERACION) {
                minas[i].piedra_acumulada += GENERACION_PIEDRA;
                minas[i].oro_acumulado += GENERACION_ORO;
                minas[i].ultimo_generacion = tiempo_actual;
                
                if (minas[i].piedra_acumulada > 100) minas[i].piedra_acumulada = 100;
                if (minas[i].oro_acumulado > 50) minas[i].oro_acumulado = 50;
            }
        }
    }
}

void animarRecoleccionMina(int x, int y) {
    int i;
    char *frames[] = {
        "  M  ", " |M| ", " /M\\ ", " \\M/ ", " *M* ",
        " /M\\ ", " *M* ", " <M> ", " $M$ "
    };
    
    int screen_x = (y - offset_c) * 2 + 2;
    int screen_y = x - offset_f + 1;
    
    for (i = 0; i < 9; i++) {
        moverCursor(screen_x, screen_y);
        
        if (i < 3) {
            setColor(COLOR_MINA_BG, COLOR_MINA_FG);
        } else if (i == 4 || i == 6) {
            setColor(COLOR_MINA_BG, 14);
        } else if (i >= 7) {
            setColor(COLOR_MINA_BG, 11);
        } else {
            setColor(COLOR_MINA_BG, 7);
        }
        
        printf("%s", frames[i]);
        Sleep(150);
    }
    
    setColor(0, 15);
}

void mostrarEstadoMina(int x, int y, int piedra, int oro) {
    int i;
    moverCursor(0, FILAS + 2);
    for (i = 0; i < 80; i++) printf(" ");
    
    moverCursor(0, FILAS + 2);
    setColor(0, 15);
    printf("MINA: ");
    setColor(0, 7);
    printf("Piedra: %d  ", piedra);
    setColor(0, 14);
    printf("Oro: %d  ", oro);
    setColor(0, 8);
    printf("[Presiona ENTER para recolectar]");
    setColor(0, 15);
}

void recogerDeMina(struct Jugador *j, char mapa[MAPA_F][MAPA_C], int px, int py) {
    int i, rx, ry, m;
    
    // Verificar si estamos sobre una mina o hay una adyacente
    if (mapa[px][py] == 'M') {
        rx = px;
        ry = py;
    } else if (!hayRecursoAdyacente(mapa, px, py, 'M', &rx, &ry)) {
        return;
    }
    
    for (i = 0; i < total_minas; i++) {
        if (minas[i].activa && minas[i].fila == rx && minas[i].columna == ry) {
            if (minas[i].piedra_acumulada > 0 || minas[i].oro_acumulado > 0) {
                int piedra_recolectada = minas[i].piedra_acumulada;
                int oro_recolectado = minas[i].oro_acumulado;
                
                animarRecoleccionMina(rx, ry);
                
                j->Piedra += piedra_recolectada;
                j->Oro += oro_recolectado;
                
                int k;
                for (k = 0; k < 2; k++) {
                    moverCursor(0, FILAS + 2);
                    for (m = 0; m < 80; m++) printf(" ");
                    moverCursor(0, FILAS + 2);
                    
                    setColor(0, 14);
                    printf("Recolectado: +%d Oro  ", oro_recolectado);
                    setColor(0, 7);
                    printf("+%d Piedra", piedra_recolectada);
                    Sleep(250);
                    
                    moverCursor(0, FILAS + 2);
                    for (m = 0; m < 80; m++) printf(" ");
                    moverCursor(0, FILAS + 2);
                    
                    setColor(0, 11);
                    printf("Recolectado: +%d Oro  ", oro_recolectado);
                    setColor(0, 11);
                    printf("+%d Piedra", piedra_recolectada);
                    Sleep(250);
                }
                
                minas[i].piedra_acumulada = 0;
                minas[i].oro_acumulado = 0;
                minas[i].ultimo_generacion = GetTickCount();
                
                setColor(0, 15);
                Sleep(400);
                
                mostrarMapa(mapa, px, py);
                forzarRedibujoPanelEnMapa(*j);
            } else {
                moverCursor(0, FILAS + 2);
                for (m = 0; m < 80; m++) printf(" ");
                moverCursor(0, FILAS + 2);
                setColor(0, 8);
                printf("La mina esta vacia. Generara recursos en 60 segundos.");
                setColor(0, 15);
                Sleep(1500);
            }
            
            return;
        }
    }
}

void crearIsla(char mapa[MAPA_F][MAPA_C], int cx, int cy, int rx, int ry) {
    int i, j, dx, dy, dist, deformacion, nx, ny;
    int maxX = rx + 10;
    int maxY = ry + 10;

    for (i = -maxY; i <= maxY; i++) {
        for (j = -maxX; j <= maxX; j++) {
            dx = j; dy = i;
            dist = dx * dx + dy * dy;
            deformacion = rand() % 5;
            if (dist <= (rx * ry) + deformacion) {
                nx = cx + i; 
                ny = cy + j;
                
                if (nx >= 0 && nx < MAPA_F && ny >= 0 && ny < MAPA_C)
                    mapa[nx][ny] = '.';
            }
        }
    }
}

void crearPuente(char mapa[MAPA_F][MAPA_C], int x1, int y1, int x2, int y2) {
    int dx = abs(x2 - x1), dy = abs(y2 - y1);
    int sx = x1 < x2 ? 1 : -1, sy = y1 < y2 ? 1 : -1;
    int err = dx - dy;

    while (true) {
        if (x1 >= 0 && x1 < MAPA_F && y1 >= 0 && y1 < MAPA_C)
            mapa[x1][y1] = '.';
        if (x1 == x2 && y1 == y2) break;
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x1 += sx; }
        if (e2 < dx) { err += dx; y1 += sy; }
    }
}

int esPuente(int x, int y) {
    if (x == 25 && y >= 35 && y <= 265) return 1;
    if (x == 125 && y >= 35 && y <= 265) return 1;
    if (y == 35 && x >= 25 && x <= 125) return 1;
    if (y == 265 && x >= 25 && x <= 125) return 1;
    if (y == 150 && x >= 25 && x <= 125) return 1;
    if (x == 75 && y >= 35 && y <= 265) return 1;
    return 0;
}

void inicializarMapa(char mapa[MAPA_F][MAPA_C]) {
    int i, j, placed, rx, ry, ex, ey;
    srand((unsigned int)time(NULL));

    for (i = 0; i < MAPA_F; i++) {
        for (j = 0; j < MAPA_C; j++) {
            mapa[i][j] = '~';
        }
    }

    struct IslaInfo { int f, c, rf, rc; };
    struct IslaInfo islas[] = {
        {25, 35, 22, 20},
        {25, 150, 22, 20},
        {125, 35, 22, 20},
        {125, 150, 22, 20},
        {75, 75, 15, 25}
    };

    for (int k = 0; k < 5; k++) {
        crearIsla(mapa, islas[k].f, islas[k].c, islas[k].rf, islas[k].rc);
    }

    crearPuente(mapa, 25, 35, 25, 265);
    crearPuente(mapa, 125, 35, 125, 265);
    crearPuente(mapa, 25, 35, 125, 35);
    crearPuente(mapa, 25, 265, 125, 265);

    crearPuente(mapa, 75, 150, 25, 150);
    crearPuente(mapa, 75, 150, 125, 150);
    crearPuente(mapa, 75, 150, 75, 35);
    crearPuente(mapa, 75, 150, 75, 265);

    for (int k = 0; k < 5; k++) {
        placed = 0;
        while (placed < 1) {
            rx = islas[k].f + (rand() % 6) - 3;
            ry = islas[k].c + (rand() % 6) - 3;
            if (rx >= 0 && rx < MAPA_F && ry >= 0 && ry < MAPA_C && mapa[rx][ry] == '.') {
                mapa[rx][ry] = 'H';
                placed++;
            }
        }
        
        placed = 0;
        while (placed < 1) {
            rx = islas[k].f + (rand() % (islas[k].rf * 2)) - islas[k].rf;
            ry = islas[k].c + (rand() % (islas[k].rc * 2)) - islas[k].rc;
            if (rx >= 0 && rx < MAPA_F && ry >= 0 && ry < MAPA_C && mapa[rx][ry] == '.') {
                mapa[rx][ry] = 'M';
                registrarMina(rx, ry);
                placed++;
            }
        }

        placed = 0;
        while (placed < 4) {
            ex = islas[k].f + (rand() % (islas[k].rf * 2)) - islas[k].rf;
            ey = islas[k].c + (rand() % (islas[k].rc * 2)) - islas[k].rc;
            if (ex >= 0 && ex < MAPA_F && ey >= 0 && ey < MAPA_C && mapa[ex][ey] == '.') {
                mapa[ex][ey] = 'E';
                placed++;
            }
        }
    }

    placed = 0; 
    while (placed < NUM_ARBOLES) { 
        rx = rand() % MAPA_F; 
        ry = rand() % MAPA_C; 
        if (mapa[rx][ry] == '.' && !esPuente(rx, ry)) { 
            mapa[rx][ry] = 'A'; 
            placed++; 
        } 
    }
    
    placed = 0;
    while (placed < NUM_COMIDA) { 
        ex = rand() % MAPA_F; 
        ey = rand() % MAPA_C; 
        if (mapa[ex][ey] == '.') { 
            mapa[ex][ey] = 'F'; 
            placed++; 
        } 
    }
}

void dibujarMarcoMapa() {
    int i;
    setColor(0, 8);
    moverCursor(0, 0); printf("%c", 201);
    for (i = 1; i < COLUMNAS * 2 + 2; i++) printf("%c", 205);
    printf("%c", 187);

    moverCursor(0, FILAS + 1); printf("%c", 200);
    for (i = 1; i < COLUMNAS * 2 + 2; i++) printf("%c", 205);
    printf("%c", 188);

    for (i = 1; i < FILAS + 1; i++) {
        moverCursor(0, i); printf("%c", 186);
        moverCursor(COLUMNAS * 2 + 2, i); printf("%c", 186);
    }
    setColor(0, 15);
}

void mostrarMapa(char mapa[MAPA_F][MAPA_C], int px, int py) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    int bufferWidth = COLUMNAS * 2;
    int bufferHeight = FILAS;
    COORD bufferSize = { (short)bufferWidth, (short)bufferHeight };
    COORD bufferCoord = { 0, 0 };
    SMALL_RECT writeRegion = { 2, 1, (short)(2 + bufferWidth - 1), (short)(1 + bufferHeight - 1) };
    CHAR_INFO buffer[FILAS * COLUMNAS * 2];
    int i, j;

    static int marcoDibujado = 0;
    if (!marcoDibujado) {
        ocultarCursor();
        dibujarMarcoMapa();
        marcoDibujado = 1;
    }
    
    for (i = 0; i < FILAS; i++) {
        for (j = 0; j < COLUMNAS; j++) {
            int absX = offset_f + i;
            int absY = offset_c + j;
            char c = mapa[absX][absY];
            WORD attr = 0;
            char symbol = ' ';

            if (c == '~') { attr = (COLOR_AGUA_BG << 4) | COLOR_AGUA_FG; symbol = '~'; }
            else if (c == '.') { attr = (COLOR_TIERRA_BG << 4) | COLOR_TIERRA_FG; symbol = '.'; }
            else if (c == 'F') { attr = (COLOR_TIERRA_BG << 4) | COLOR_COMIDA; symbol = 'F'; }
            else if (c == 'M') { attr = (COLOR_MINA_BG << 4) | COLOR_MINA_FG; symbol = 'M'; }
            else if (c == 'A') { attr = (COLOR_TIERRA_BG << 4) | COLOR_ARBOL; symbol = 'A'; }
            else if (c == 'E') { attr = (COLOR_CAMP_BG << 4) | COLOR_CAMP_FG; symbol = 'E'; }
            else if (c == 'H') { attr = (5 << 4) | 15; symbol = 'H'; }
            else { attr = (0 << 4) | 15; symbol = c; }

            if (absX == px && absY == py) {
                symbol = 'P';
                attr = (0 << 4) | 10;
            }

            int index = (i * bufferWidth) + (j * 2);
            buffer[index].Char.AsciiChar = symbol;
            buffer[index].Attributes = attr;
            buffer[index + 1].Char.AsciiChar = ' ';
            buffer[index + 1].Attributes = attr;
        }
    }
    
    WriteConsoleOutput(hConsole, buffer, bufferSize, bufferCoord, &writeRegion);
    setColor(0, 15);
}

int hayRecursoAdyacente(char mapa[MAPA_F][MAPA_C], int px, int py, char recurso, int *rx, int *ry) {
    int i;
    int dx[] = {-1, -1, -1,  0,  0,  1,  1,  1};
    int dy[] = {-1,  0,  1, -1,  1, -1,  0,  1};
    
    for (i = 0; i < 8; i++) {
        int nx = px + dx[i];
        int ny = py + dy[i];
        
        if (nx >= 0 && nx < MAPA_F && ny >= 0 && ny < MAPA_C) {
            if (mapa[nx][ny] == recurso) {
                *rx = nx;
                *ry = ny;
                return 1;
            }
        }
    }
    return 0;
}

void animarTalado(int x, int y) {
    int i, j;
    char *frames[] = {
        "  A  ", " <A> ", " /A\\ ", " |A| ",
        " \\A/ ", " _|_ ", " ___ "
    };
    
    int screen_x = (y - offset_c) * 2 + 2;
    int screen_y = x - offset_f + 1;
    
    for (i = 0; i < 7; i++) {
        moverCursor(screen_x, screen_y);
        
        if (i < 3) {
            setColor(COLOR_TIERRA_BG, COLOR_ARBOL);
        } else if (i < 5) {
            setColor(COLOR_TIERRA_BG, 14);
        } else {
            setColor(COLOR_TIERRA_BG, 8);
        }
        
        printf("%s", frames[i]);
        Sleep(200);
    }
    
    moverCursor(0, FILAS + 2);
    for (j = 0; j < 60; j++) printf(" ");
    
    for (i = 0; i < 2; i++) {
        moverCursor(0, FILAS + 2);
        setColor(0, 14);
        printf("Has talado el arbol! +30 Madera");
        Sleep(200);
        
        moverCursor(0, FILAS + 2);
        setColor(0, 6);
        printf("Has talado el arbol! +30 Madera");
        Sleep(200);
    }
    
    setColor(0, 15);
    Sleep(400);
}

int explorarMapa(struct Jugador *j, char mapa[MAPA_F][MAPA_C], int *x, int *y, char direccion) {
    int nx = *x, ny = *y;
    char destino, msg[60] = "";
    int i, offset_changed = 0, recurso_recolectado = 0;
    int rx, ry;
    static int esperando_recoleccion_mina = 0;
    static int mina_rx = -1, mina_ry = -1;

    if (direccion >= 'a' && direccion <= 'z') direccion -= 32;
    
    if (direccion == 13) {
        // Si ya estamos esperando recolectar de la mina
        if (esperando_recoleccion_mina) {
            esperando_recoleccion_mina = 0;
            
            // Buscar y recolectar
            for (i = 0; i < total_minas; i++) {
                if (minas[i].activa && minas[i].fila == mina_rx && minas[i].columna == mina_ry) {
                    if (minas[i].piedra_acumulada > 0 || minas[i].oro_acumulado > 0) {
                        int piedra_rec = minas[i].piedra_acumulada;
                        int oro_rec = minas[i].oro_acumulado;
                        
                        animarRecoleccionMina(mina_rx, mina_ry);
                        
                        j->Piedra += piedra_rec;
                        j->Oro += oro_rec;
                        
                        int k, m;
                        for (k = 0; k < 2; k++) {
                            moverCursor(0, FILAS + 2);
                            for (m = 0; m < 80; m++) printf(" ");
                            moverCursor(0, FILAS + 2);
                            setColor(0, 14);
                            printf("Recolectado: +%d Oro  +%d Piedra", oro_rec, piedra_rec);
                            Sleep(250);
                            
                            moverCursor(0, FILAS + 2);
                            for (m = 0; m < 80; m++) printf(" ");
                            moverCursor(0, FILAS + 2);
                            setColor(0, 11);
                            printf("Recolectado: +%d Oro  +%d Piedra", oro_rec, piedra_rec);
                            Sleep(250);
                        }
                        
                        minas[i].piedra_acumulada = 0;
                        minas[i].oro_acumulado = 0;
                        minas[i].ultimo_generacion = GetTickCount();
                        
                        setColor(0, 15);
                        Sleep(400);
                        
                        mostrarMapa(mapa, *x, *y);
                        forzarRedibujoPanelEnMapa(*j);
                    } else {
                        moverCursor(0, FILAS + 2);
                        for (i = 0; i < 80; i++) printf(" ");
                        moverCursor(0, FILAS + 2);
                        setColor(0, 8);
                        printf("La mina esta vacia. Generara recursos en 60 segundos.");
                        setColor(0, 15);
                        Sleep(1500);
                    }
                    break;
                }
            }
            
            return 0;
        }
        
        // Primera detección de recursos
        if (hayRecursoAdyacente(mapa, *x, *y, 'A', &rx, &ry)) {
            animarTalado(rx, ry);
            j->Madera += 30;
            mapa[rx][ry] = '.';
            mostrarMapa(mapa, *x, *y);
            forzarRedibujoPanelEnMapa(*j);
            return 1;
        }
        else if (hayRecursoAdyacente(mapa, *x, *y, 'F', &rx, &ry)) {
            j->Comida += 25;
            mapa[rx][ry] = '.';
            
            moverCursor(0, FILAS + 2);
            for (i = 0; i < 60; i++) printf(" ");
            moverCursor(0, FILAS + 2);
            setColor(0, 10);
            printf("Has cosechado el cultivo! +25 Comida");
            setColor(0, 15);
            Sleep(800);
            
            mostrarMapa(mapa, *x, *y);
            forzarRedibujoPanelEnMapa(*j);
            return 1;
        }
        else if (hayRecursoAdyacente(mapa, *x, *y, 'E', &rx, &ry)) {
            mostrarMenuCampamento(j, mapa, *x, *y);
            return 0;
        }
        else if (hayRecursoAdyacente(mapa, *x, *y, 'H', &rx, &ry)) {
            mostrarMenuCasaCentral(j, mapa, *x, *y, rx, ry);
            return 0;
        }
        else if (mapa[*x][*y] == 'M' || hayRecursoAdyacente(mapa, *x, *y, 'M', &rx, &ry)) {
            // Detectar si estamos sobre la mina
            if (mapa[*x][*y] == 'M') {
                rx = *x;
                ry = *y;
            }
            
            // Guardar posición de la mina
            mina_rx = rx;
            mina_ry = ry;
            
            // Buscar la mina en el sistema y mostrar estado
            for (i = 0; i < total_minas; i++) {
                if (minas[i].activa && minas[i].fila == rx && minas[i].columna == ry) {
                    mostrarEstadoMina(rx, ry, minas[i].piedra_acumulada, minas[i].oro_acumulado);
                    esperando_recoleccion_mina = 1;
                    break;
                }
            }
            
            return 0;
        }
        else {
            moverCursor(0, FILAS + 2);
            for (i = 0; i < 60; i++) printf(" ");
            moverCursor(0, FILAS + 2);
            setColor(0, 8);
            printf("No hay nada con que interactuar aqui.");
            setColor(0, 15);
            Sleep(500);
            return 0;
        }
    }
    
    // Si el jugador se mueve, cancelar espera de mina
    if (direccion == 'W' || direccion == 'S' || direccion == 'A' || direccion == 'D') {
        esperando_recoleccion_mina = 0;
    }
    
    if (direccion == 'W') nx--;
    else if (direccion == 'S') nx++;
    else if (direccion == 'A') ny--;
    else if (direccion == 'D') ny++;
    else return 0;

    if (nx < 0 || nx >= MAPA_F || ny < 0 || ny >= MAPA_C) {
        moverCursor(0, FILAS + 2);
        setColor(0, 14);
        printf("Limite del mapa!               ");
        setColor(0, 15);
        return 0;
    }

    destino = mapa[nx][ny];
    
    if (destino == '~') {
        moverCursor(0, FILAS + 2);
        setColor(0, 14);
        printf("No puedes nadar!               ");
        setColor(0, 15);
        return 0;
    }
    
    if (destino == 'A') {
        moverCursor(0, FILAS + 2);
        setColor(0, 6);
        printf("Un arbol! Presiona ENTER para talarlo");
        setColor(0, 15);
        return 0;
    }
    
    if (destino == 'E') {
        moverCursor(0, FILAS + 2);
        setColor(0, 12);
        printf("Campamento! Presiona ENTER para entrar");
        setColor(0, 15);
        return 0;
    }
    
    if (destino == 'H') {
        moverCursor(0, FILAS + 2);
        setColor(0, 13);
        printf("Casa Central! Presiona ENTER para gestionar cultivos");
        setColor(0, 15);
        return 0;
    }
    
    if (destino == 'F') {
        moverCursor(0, FILAS + 2);
        setColor(0, 10);
        printf("Cultivo! Presiona ENTER para cosechar");
        setColor(0, 15);
        return 0;
    }

    int new_offset_f = nx - FILAS / 2;
    int new_offset_c = ny - COLUMNAS / 2;
    if (new_offset_f < 0) new_offset_f = 0;
    else if (new_offset_f > MAPA_F - FILAS) new_offset_f = MAPA_F - FILAS;
    if (new_offset_c < 0) new_offset_c = 0;
    else if (new_offset_c > MAPA_C - COLUMNAS) new_offset_c = MAPA_C - COLUMNAS;

    if (new_offset_f != offset_f || new_offset_c != offset_c) {
        offset_f = new_offset_f;
        offset_c = new_offset_c;
        offset_changed = 1;
    }

    if (!offset_changed) {
        moverCursor((short)((*y - offset_c) * 2 + 2), (short)(*x - offset_f + 1));
        char actual = mapa[*x][*y];
        if (actual == '~') setColor(COLOR_AGUA_BG, COLOR_AGUA_FG);
        else if (actual == '.') setColor(COLOR_TIERRA_BG, COLOR_TIERRA_FG);
        else if (actual == 'F') setColor(COLOR_TIERRA_BG, COLOR_COMIDA);
        else if (actual == 'M') setColor(COLOR_MINA_BG, COLOR_MINA_FG);
        else if (actual == 'A') setColor(COLOR_TIERRA_BG, COLOR_ARBOL);
        else if (actual == 'E') setColor(COLOR_CAMP_BG, COLOR_CAMP_FG);
        else setColor(0, 15);
        printf("%c ", actual);
    }

    *x = nx;
    *y = ny;

    if (offset_changed) {
        mostrarMapa(mapa, *x, *y);
        if (j != NULL) forzarRedibujoPanelEnMapa(*j);
    } else {
        moverCursor((short)((ny - offset_c) * 2 + 2), (short)(nx - offset_f + 1));
        setColor(0, 10);
        printf("P ");
        setColor(0, 15);
    }

    ocultarCursor();

    moverCursor(0, FILAS + 2);
    setColor(0, 0);
    for (i = 0; i < 120; i++) printf(" ");

    moverCursor(0, FILAS + 2);
    if (msg[0] != '\0') {
        setColor(0, 11);
        printf("%s", msg);
    } else {
        setColor(0, 15);
        printf("Usa WASD para moverte. ENTER para interactuar. ESC para salir.");
    }
    setColor(0, 15);

    return recurso_recolectado;
}

void animarAgua(char mapa[MAPA_F][MAPA_C]) {
    int i, j;
    static int frame = 0;
    static DWORD last_frame_time = 0;
    DWORD current_time = GetTickCount();
    
    if (current_time - last_frame_time < 300) {
        return;
    }
    
    last_frame_time = current_time;
    frame++;
    
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    
    for (i = 0; i < FILAS; i++) {
        for (j = 0; j < COLUMNAS; j++) { 
            if (mapa[offset_f + i][offset_c + j] == '~') {
                int screen_x = (j * 2) + 2;
                int screen_y = i + 1;
                
                moverCursor(screen_x, screen_y);
                if ((i + j + frame) % 3 == 0) { 
                    setColor(COLOR_AGUA_BG, COLOR_AGUA_FG); 
                    printf("~ "); 
                } else { 
                    setColor(COLOR_AGUA_BG, COLOR_AGUA_FG); 
                    printf("  "); 
                }
            }
        }
    }
    
    SetConsoleCursorPosition(hConsole, csbi.dwCursorPosition);
    ocultarCursor();
    setColor(0, 15);
}


void mostrarMenuCampamento(struct Jugador *j, char mapa[MAPA_F][MAPA_C], int px, int py) {
    int tecla, i;
    int seleccion = 0;
    int actualizado = 1;
    
    int costos[4][4] = {
        {50, 30, 20, 10},
        {40, 25, 30, 5},
        {35, 20, 25, 15},
        {80, 50, 40, 20}
    };
    
    char* nombres[4] = {"Espadachin", "Arquero", "Piquero", "Caballeria"};
    int stats[4][4] = {
        {100, 15, 2, 1},
        {80, 12, 3, 5},
        {90, 10, 2, 2},
        {120, 20, 1, 1}
    };
    
    while (1) {
        if (actualizado) {
            system("cls");
            
            setColor(0, 15);
            printf("\n");
            printf("  ================================================================================\n");
            printf("  ||                     CUARTEL DE ENTRENAMIENTO - %s                  ||\n", j->Nombre);
            printf("  ================================================================================\n\n");
            
            setColor(0, 11);
            printf("  TROPAS ACTUALES:\n");
            setColor(0, 15);
            printf("  -------------------------------------------------------------------------------\n");
            printf("  Total: %d/%d | Espadas: %d | Arqueros: %d | Picas: %d | Caballeria: %d\n",
                   j->NumeroTropas, j->Capacidad > 0 ? j->Capacidad : 100,
                   j->CantidadEspadas, j->CantidadArqueros, j->CantidadPicas, j->CantidadCaballeria);
            printf("  -------------------------------------------------------------------------------\n\n");
            
            setColor(0, 14);
            printf("  RECURSOS DISPONIBLES:\n");
            setColor(0, 15);
            printf("  -------------------------------------------------------------------------------\n");
            printf("  ");
            setColor(0, 14); printf("Oro: %d   ", j->Oro);
            setColor(0, 12); printf("Comida: %d   ", j->Comida);
            setColor(0, 6); printf("Madera: %d   ", j->Madera);
            setColor(0, 7); printf("Piedra: %d\n", j->Piedra);
            setColor(0, 15);
            printf("  -------------------------------------------------------------------------------\n\n");
            
            setColor(0, 10);
            printf("  TROPAS DISPONIBLES PARA ENTRENAR:\n");
            setColor(0, 15);
            printf("  ===============================================================================\n");
            printf("  | # | Tipo        | Vida | Ataque | Vel.Atq | Alcance | Costo (O/C/M/P)     |\n");
            printf("  |---|-------------|------|--------|---------|---------|---------------------|\n");
            
            for (i = 0; i < 4; i++) {
                if (i == seleccion) {
                    setColor(2, 15);
                    printf("  |>>>");
                } else {
                    setColor(0, 15);
                    printf("  | %d |", i + 1);
                }
                
                if (i == seleccion) setColor(2, 11); else setColor(0, 11);
                printf(" %-11s", nombres[i]);
                
                if (i == seleccion) setColor(2, 15); else setColor(0, 15);
                printf(" | %3d  |  %3d   |   %2d    |   %2d    | ",
                       stats[i][0], stats[i][1], stats[i][2], stats[i][3]);
                
                if (i == seleccion) setColor(2, 14); else setColor(0, 14);
                printf("%d", costos[i][0]);
                if (i == seleccion) setColor(2, 15); else setColor(0, 15);
                printf("/");
                if (i == seleccion) setColor(2, 12); else setColor(0, 12);
                printf("%d", costos[i][1]);
                if (i == seleccion) setColor(2, 15); else setColor(0, 15);
                printf("/");
                if (i == seleccion) setColor(2, 6); else setColor(0, 6);
                printf("%d", costos[i][2]);
                if (i == seleccion) setColor(2, 15); else setColor(0, 15);
                printf("/");
                if (i == seleccion) setColor(2, 7); else setColor(0, 7);
                printf("%d", costos[i][3]);
                if (i == seleccion) setColor(2, 15); else setColor(0, 15);
                
                if (i == seleccion) {
                    printf("   <<<|\n");
                } else {
                    printf("       |\n");
                }
            }
            
            setColor(0, 15);
            printf("  ===============================================================================\n\n");
            
            setColor(0, 8);
            printf("  Vida: Puntos de vida | Ataque: Dano por golpe | Vel.Atq: Velocidad de ataque\n");
            printf("  Alcance: Distancia de ataque | Costo: Oro/Comida/Madera/Piedra\n\n");
            
            setColor(0, 14);
            printf("  ===============================================================================\n");
            printf("  ||  W/S: Mover  ||  ENTER: Entrenar tropa  ||  ESC: Volver al mapa         ||\n");
            printf("  ===============================================================================\n");
            setColor(0, 15);
            
            actualizado = 0;
        }
        
        if (_kbhit()) {
            tecla = _getch();
            
            if (tecla == 27) {
                break;
            }
            
            if (tecla == 'w' || tecla == 'W') {
                seleccion--;
                if (seleccion < 0) seleccion = 3;
                actualizado = 1;
            }
            else if (tecla == 's' || tecla == 'S') {
                seleccion++;
                if (seleccion > 3) seleccion = 0;
                actualizado = 1;
            }
            
            else if (tecla == 13) {
                if (j->Oro >= costos[seleccion][0] &&
                    j->Comida >= costos[seleccion][1] &&
                    j->Madera >= costos[seleccion][2] &&
                    j->Piedra >= costos[seleccion][3]) {
                    
                    j->Oro -= costos[seleccion][0];
                    j->Comida -= costos[seleccion][1];
                    j->Madera -= costos[seleccion][2];
                    j->Piedra -= costos[seleccion][3];
                    
                    j->NumeroTropas++;
                    if (seleccion == 0) j->CantidadEspadas++;
                    else if (seleccion == 1) j->CantidadArqueros++;
                    else if (seleccion == 2) j->CantidadPicas++;
                    else if (seleccion == 3) j->CantidadCaballeria++;
                    
                    actualizado = 1;
                    
                    setColor(0, 10);
                    printf("\n  >> %s entrenado exitosamente! <<\n", nombres[seleccion]);
                    setColor(0, 15);
                    Sleep(500);
                }
                else {
                    setColor(0, 12);
                    printf("\n  >> Recursos insuficientes para entrenar %s! <<\n", nombres[seleccion]);
                    setColor(0, 15);
                    Sleep(1000);
                    actualizado = 1;
                }
            }
            
            if (tecla == 224 || tecla == 0) {
                _getch();
            }
        }
        
        Sleep(50);
    }
    
    system("cls");
    
    setColor(0, 8);
    moverCursor(0, 0); 
    printf("%c", 201);
    for (i = 1; i < COLUMNAS * 2 + 2; i++) printf("%c", 205);
    printf("%c", 187);

    moverCursor(0, FILAS + 1); 
    printf("%c", 200);
    for (i = 1; i < COLUMNAS * 2 + 2; i++) printf("%c", 205);
    printf("%c", 188);

    for (i = 1; i < FILAS + 1; i++) {
        moverCursor(0, i); 
        printf("%c", 186);
        moverCursor(COLUMNAS * 2 + 2, i); 
        printf("%c", 186);
    }
    setColor(0, 15);
    
    mostrarMapa(mapa, px, py);
    forzarRedibujoPanelEnMapa(*j);
    
    moverCursor(0, FILAS + 2);
    setColor(0, 15);
    printf("Usa WASD para moverte. ENTER para interactuar. ESC para salir.");
    
    ocultarCursor();
}

int generarParcelaCultivos(char mapa[MAPA_F][MAPA_C], int casa_x, int casa_y, int ancho, int alto) {
    int intentos = 0;
    int max_intentos = 100;
    int px, py, i, j;
    int valido;
    
    while (intentos < max_intentos) {
        px = casa_x + (rand() % 40) - 20;
        py = casa_y + (rand() % 40) - 20;
        
        valido = 1;
        for (i = 0; i < alto && valido; i++) {
            for (j = 0; j < ancho && valido; j++) {
                int nx = px + i;
                int ny = py + j;
                
                if (nx < 0 || nx >= MAPA_F || ny < 0 || ny >= MAPA_C) {
                    valido = 0;
                } else if (mapa[nx][ny] != '.' && !esPuente(nx, ny)) {
                    valido = 0;
                }
            }
        }
        
        if (valido) {
            for (i = 0; i < alto; i++) {
                for (j = 0; j < ancho; j++) {
                    mapa[px + i][py + j] = 'F';
                }
            }
            return 1;
        }
        
        intentos++;
    }
    
    return 0;
}

void mostrarMenuCasaCentral(struct Jugador *j, char mapa[MAPA_F][MAPA_C], int px, int py, int casa_x, int casa_y) {
    int tecla, i;
    int seleccion = 0;
    int actualizado = 1;
    
    int parcelas[3][6] = {
        {20, 10, 30, 5,  3, 3},
        {40, 20, 60, 10, 5, 5},
        {80, 40, 120, 20, 7, 7}
    };
    
    char* nombres[3] = {"Pequena", "Mediana", "Grande"};
    
    while (1) {
        if (actualizado) {
            system("cls");
            
            setColor(0, 15);
            printf("\n");
            printf("  ================================================================================\n");
            printf("  ||                       CASA CENTRAL - %s                           ||\n", j->Nombre);
            printf("  ================================================================================\n\n");
            
            setColor(0, 14);
            printf("  RECURSOS DISPONIBLES:\n");
            setColor(0, 15);
            printf("  -------------------------------------------------------------------------------\n");
            printf("  ");
            setColor(0, 14); printf("Oro: %d   ", j->Oro);
            setColor(0, 12); printf("Comida: %d   ", j->Comida);
            setColor(0, 6); printf("Madera: %d   ", j->Madera);
            setColor(0, 7); printf("Piedra: %d\n", j->Piedra);
            setColor(0, 15);
            printf("  -------------------------------------------------------------------------------\n\n");
            
            setColor(0, 10);
            printf("  PARCELAS DE CULTIVO DISPONIBLES:\n");
            setColor(0, 15);
            printf("  ===============================================================================\n");
            printf("  | # | Tamano  | Dimension | Cultivos | Costo (O/C/M/P)                      |\n");
            printf("  |---|---------|-----------|----------|--------------------------------------|\n");
            
            for (i = 0; i < 3; i++) {
                if (i == seleccion) {
                    setColor(2, 15);
                    printf("  |>>>");
                } else {
                    setColor(0, 15);
                    printf("  | %d |", i + 1);
                }
                
                if (i == seleccion) setColor(2, 11); else setColor(0, 11);
                printf(" %-7s", nombres[i]);
                
                if (i == seleccion) setColor(2, 15); else setColor(0, 15);
                printf(" |   %dx%-2d    |   %2d     | ",
                       parcelas[i][4], parcelas[i][5], 
                       parcelas[i][4] * parcelas[i][5]);
                
                if (i == seleccion) setColor(2, 14); else setColor(0, 14);
                printf("%d", parcelas[i][0]);
                if (i == seleccion) setColor(2, 15); else setColor(0, 15);
                printf("/");
                if (i == seleccion) setColor(2, 12); else setColor(0, 12);
                printf("%d", parcelas[i][1]);
                if (i == seleccion) setColor(2, 15); else setColor(0, 15);
                printf("/");
                if (i == seleccion) setColor(2, 6); else setColor(0, 6);
                printf("%d", parcelas[i][2]);
                if (i == seleccion) setColor(2, 15); else setColor(0, 15);
                printf("/");
                if (i == seleccion) setColor(2, 7); else setColor(0, 7);
                printf("%d", parcelas[i][3]);
                if (i == seleccion) setColor(2, 15); else setColor(0, 15);
                
                if (i == seleccion) {
                    printf("                   <<<|\n");
                } else {
                    printf("                      |\n");
                }
            }
            
            setColor(0, 15);
            printf("  ===============================================================================\n\n");
            
            setColor(0, 8);
            printf("  Las parcelas generan cultivos automaticamente en la isla.\n");
            printf("  A mayor tamano, mas cultivos tendras disponibles para recolectar.\n");
            printf("  Los cultivos aparecen en rectangulos cerca de la Casa Central.\n\n");
            
            setColor(0, 14);
            printf("  ===============================================================================\n");
            printf("  ||  W/S: Mover  ||  ENTER: Comprar parcela  ||  ESC: Volver al mapa        ||\n");
            printf("  ===============================================================================\n");
            setColor(0, 15);
            
            actualizado = 0;
        }
        
        if (_kbhit()) {
            tecla = _getch();
            
            if (tecla == 27) break;
            
            if (tecla == 'w' || tecla == 'W') {
                seleccion--;
                if (seleccion < 0) seleccion = 2;
                actualizado = 1;
            }
            else if (tecla == 's' || tecla == 'S') {
                seleccion++;
                if (seleccion > 2) seleccion = 0;
                actualizado = 1;
            }
            
            else if (tecla == 13) {
                if (j->Oro >= parcelas[seleccion][0] &&
                    j->Comida >= parcelas[seleccion][1] &&
                    j->Madera >= parcelas[seleccion][2] &&
                    j->Piedra >= parcelas[seleccion][3]) {
                    
                    if (generarParcelaCultivos(mapa, casa_x, casa_y, 
                                               parcelas[seleccion][4], 
                                               parcelas[seleccion][5])) {
                        j->Oro -= parcelas[seleccion][0];
                        j->Comida -= parcelas[seleccion][1];
                        j->Madera -= parcelas[seleccion][2];
                        j->Piedra -= parcelas[seleccion][3];
                        
                        setColor(0, 10);
                        printf("\n  >> Parcela %s creada exitosamente! <<\n", nombres[seleccion]);
                        setColor(0, 15);
                        Sleep(800);
                        actualizado = 1;
                    } else {
                        setColor(0, 12);
                        printf("\n  >> No hay espacio disponible en la isla para esta parcela! <<\n");
                        setColor(0, 15);
                        Sleep(1200);
                        actualizado = 1;
                    }
                }
                else {
                    setColor(0, 12);
                    printf("\n  >> Recursos insuficientes para crear parcela %s! <<\n", nombres[seleccion]);
                    setColor(0, 15);
                    Sleep(1000);
                    actualizado = 1;
                }
            }
            
            if (tecla == 224 || tecla == 0) {
                _getch();
            }
        }
        
        Sleep(50);
    }
    
    system("cls");
    
    setColor(0, 8);
    moverCursor(0, 0); 
    printf("%c", 201);
    for (i = 1; i < COLUMNAS * 2 + 2; i++) printf("%c", 205);
    printf("%c", 187);

    moverCursor(0, FILAS + 1); 
    printf("%c", 200);
    for (i = 1; i < COLUMNAS * 2 + 2; i++) printf("%c", 205);
    printf("%c", 188);

    for (i = 1; i < FILAS + 1; i++) {
        moverCursor(0, i); 
        printf("%c", 186);
        moverCursor(COLUMNAS * 2 + 2, i); 
        printf("%c", 186);
    }
    setColor(0, 15);
    
    mostrarMapa(mapa, px, py);
    forzarRedibujoPanelEnMapa(*j);
    
    moverCursor(0, FILAS + 2);
    setColor(0, 15);
    printf("Usa WASD para moverte. ENTER para interactuar. ESC para salir.");
    
    ocultarCursor();
}

void dibujarPanelEnMapa(struct Jugador j) {
    static int last_oro = -1;
    static int last_comida = -1;
    static int last_madera = -1;
    static int last_piedra = -1;
    static int last_espadas = -1;
    static int last_arqueros = -1;
    static int last_picas = -1;
    static int last_caballeria = -1;
    static int first_draw = 1;
    
    int cambios = (j.Oro != last_oro || 
                   j.Comida != last_comida || 
                   j.Madera != last_madera || 
                   j.Piedra != last_piedra ||
                   j.CantidadEspadas != last_espadas ||
                   j.CantidadArqueros != last_arqueros ||
                   j.CantidadPicas != last_picas ||
                   j.CantidadCaballeria != last_caballeria ||
                   first_draw);
    
    if (!cambios) return;
    
    last_oro = j.Oro;
    last_comida = j.Comida;
    last_madera = j.Madera;
    last_piedra = j.Piedra;
    last_espadas = j.CantidadEspadas;
    last_arqueros = j.CantidadArqueros;
    last_picas = j.CantidadPicas;
    last_caballeria = j.CantidadCaballeria;
    first_draw = 0;
    
    forzarRedibujoPanelEnMapa(j);
}

void limpiarMensajes() {
    int i, j;
    for (i = FILAS + 2; i <= FILAS + 10; i++) {
        moverCursor(0, i);
        setColor(0, 0);
        for (j = 0; j < 120; j++) {
            printf(" ");
        }
    }
    setColor(0, 15);
}

void resetearCachePanelEnMapa() {
}

void forzarRedibujoPanelEnMapa(struct Jugador j) {
    int panel_x = COLUMNAS * 2 + 6;
    int panel_y = 1;
    int panel_width = 20;
    int i;
    
    moverCursor(panel_x, panel_y);
    setColor(0, 8);
    printf("%c", 218);
    for (i = 0; i < panel_width - 2; i++) printf("%c", 196);
    printf("%c", 191);
    
    moverCursor(panel_x, panel_y + 1);
    setColor(0, 8);
    printf("%c", 179);
    setColor(0, 11);
    printf(" %-*s", panel_width - 3, j.Nombre);
    setColor(0, 8);
    printf("%c", 179);
    
    moverCursor(panel_x, panel_y + 2);
    printf("%c", 195);
    for (i = 0; i < panel_width - 2; i++) printf("%c", 196);
    printf("%c", 180);
    
    moverCursor(panel_x, panel_y + 3);
    printf("%c", 179);
    setColor(0, 14);
    printf(" Oro   : %-6d ", j.Oro);
    setColor(0, 8);
    printf("%c", 179);
    
    moverCursor(panel_x, panel_y + 4);
    printf("%c", 179);
    setColor(0, 12);
    printf(" Comida: %-6d ", j.Comida);
    setColor(0, 8);
    printf("%c", 179);
    
    moverCursor(panel_x, panel_y + 5);
    printf("%c", 179);
    setColor(0, 6);
    printf(" Madera: %-6d ", j.Madera);
    setColor(0, 8);
    printf("%c", 179);
    
    moverCursor(panel_x, panel_y + 6);
    printf("%c", 179);
    setColor(0, 7);
    printf(" Piedra: %-6d ", j.Piedra);
    setColor(0, 8);
    printf("%c", 179);
    
    moverCursor(panel_x, panel_y + 7);
    printf("%c", 195);
    for (i = 0; i < panel_width - 2; i++) printf("%c", 196);
    printf("%c", 180);
    
    moverCursor(panel_x, panel_y + 8);
    printf("%c", 179);
    setColor(0, 11);
    printf(" Espadas : %-4d ", j.CantidadEspadas);
    setColor(0, 8);
    printf("%c", 179);
    
    moverCursor(panel_x, panel_y + 9);
    printf("%c", 179);
    setColor(0, 11);
    printf(" Arqueros: %-4d ", j.CantidadArqueros);
    setColor(0, 8);
    printf("%c", 179);
    
    moverCursor(panel_x, panel_y + 10);
    printf("%c", 179);
    setColor(0, 11);
    printf(" Picas   : %-4d ", j.CantidadPicas);
    setColor(0, 8);
    printf("%c", 179);
    
    moverCursor(panel_x, panel_y + 11);
    printf("%c", 179);
    setColor(0, 11);
    printf(" Caball. : %-4d ", j.CantidadCaballeria);
    setColor(0, 8);
    printf("%c", 179);
    
    moverCursor(panel_x, panel_y + 12);
    printf("%c", 195);
    for (i = 0; i < panel_width - 2; i++) printf("%c", 196);
    printf("%c", 180);
    
    moverCursor(panel_x, panel_y + 13);
    printf("%c", 179);
    setColor(0, 10);
    printf(" TOTAL   : %-4d ", j.NumeroTropas);
    setColor(0, 8);
    printf("%c", 179);
    
    moverCursor(panel_x, panel_y + 14);
    printf("%c", 192);
    for (i = 0; i < panel_width - 2; i++) printf("%c", 196);
    printf("%c", 217);
    
    setColor(0, 15);
}
