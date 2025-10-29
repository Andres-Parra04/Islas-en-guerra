// mapa.c
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include "mapa.h"

// ===================================================
// Ajustes de consola (tamaño y utilidades)
// ===================================================
static HANDLE hConsole = NULL;

void inicializarConsola(int ancho, int alto) {
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    // Ajustar tamaño del buffer primero
    COORD bufferSize = { (SHORT)ancho, (SHORT)(alto + 2) }; // +2 para la línea de ayuda
    SetConsoleScreenBufferSize(hConsole, bufferSize);

    // Ajustar ventana
    SMALL_RECT windowSize = { 0, 0, (SHORT)(ancho - 1), (SHORT)(alto - 1) };
    SetConsoleWindowInfo(hConsole, TRUE, &windowSize);

    // Opcional: esconder cursor para evitar parpadeo
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(hConsole, &cursorInfo);
}

void restaurarCursorVisible() {
    if (!hConsole) return;
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = TRUE;
    SetConsoleCursorInfo(hConsole, &cursorInfo);
}

// Mueve el cursor a (x,y)
void moverCursor(int x, int y) {
    if (!hConsole) hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD pos = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(hConsole, pos);
}

// Cambia color (atributo) del texto (0..255, usamos bajo 16)
void setColor(int color) {
    if (!hConsole) hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, (WORD)color);
}

// No rellenamos todo el buffer para evitar efectos secundarios.
// Solo colocamos el cursor al inicio para sobrescribir lo existente.
void prepararFrame() {
    moverCursor(0, 0);
}

// ===================================================
// Generador de ruido (idéntico al anterior)
// ===================================================
static float ruidoPerlin(int x, int y, int semilla) {
    int n = x + y * 57 + semilla * 131;
    n = (n << 13) ^ n;
    return (1.0f - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f);
}

// ===================================================
// Inicializa el mapa con islas y recursos
// ===================================================
void inicializarMapa(Mapa *mapa) {
    srand((unsigned)time(NULL));
	int i,j,n;
    for ( i= 0; i < FILAS; i++)
        for ( j = 0; j < COLUMNAS; j++)
            mapa->celdas[i][j] = AGUA;

    int centros[4][2] = {{10, 20}, {10, 60}, {30, 20}, {30, 60}};

    for ( n = 0; n < 4; n++) {
        int cx = centros[n][0], cy = centros[n][1];
        int radio = 8 + rand() % 5;
        int semilla1 = rand() % 1000;
        int semilla2 = rand() % 1000;

        for ( i = -radio * 2; i <= radio * 2; i++) {
            for ( j = -radio * 2; j <= radio * 2; j++) {
                int x = cx + i;
                int y = cy + j;
                if (x < 0 || x >= FILAS || y < 0 || y >= COLUMNAS)
                    continue;

                float dist = (i * i + j * j) / (float)(radio * radio);
                float deform1 = (ruidoPerlin(x, y, semilla1) + 1.0f) / 2.0f;
                float deform2 = (ruidoPerlin(x / 3, y / 3, semilla2) + 1.0f) / 2.0f;
                float ruido_total = (deform1 * 0.3f) + (deform2 * 0.7f);

                if (dist + ruido_total * 0.8f < 1.0f) {
                    int r = rand() % 100;
                    if (r < 5)
                        mapa->celdas[x][y] = COMIDA;
                    else if (r < 10)
                        mapa->celdas[x][y] = MADERA;
                    else if (r < 15)
                        mapa->celdas[x][y] = RECURSO;
                    else
                        mapa->celdas[x][y] = TIERRA;
                }
            }
        }
    }

    mapa->jugadorX = centros[0][0];
    mapa->jugadorY = centros[0][1];
    mapa->celdaBajoJugador = mapa->celdas[mapa->jugadorX][mapa->jugadorY];
    mapa->celdas[mapa->jugadorX][mapa->jugadorY] = JUGADOR;
}

// ===================================================
// Movimiento del jugador
// ===================================================
void moverJugador(Mapa *mapa, char dir) {
    int nx = mapa->jugadorX, ny = mapa->jugadorY;
    if (dir == 'w') nx--;
    if (dir == 's') nx++;
    if (dir == 'a') ny--;
    if (dir == 'd') ny++;

    if (nx < 0 || nx >= FILAS || ny < 0 || ny >= COLUMNAS)
        return;

    if (mapa->celdas[nx][ny] != AGUA) {
        mapa->celdas[mapa->jugadorX][mapa->jugadorY] = mapa->celdaBajoJugador;
        mapa->celdaBajoJugador = mapa->celdas[nx][ny];
        mapa->jugadorX = nx;
        mapa->jugadorY = ny;
        mapa->celdas[nx][ny] = JUGADOR;
    }
}

// ===================================================
// Dibuja el mapa en consola con colores, sobrescribiendo la pantalla
// recibe 'tiempo' para animación
// ===================================================
void dibujarMapa(Mapa *mapa, double tiempo) {
    prepararFrame(); // mueve cursor a 0,0 para sobrescribir
	int i,j;
    for (i = 0; i < FILAS; i++) {
        // Construimos la fila en un buffer y la imprimimos una vez para eficiencia
        char filaBuf[COLUMNAS + 1];
        for ( j = 0; j < COLUMNAS; j++) {
            TipoCelda celda = mapa->celdas[i][j];
            char simbolo = ' ';
            int color = 7; // blanco por defecto

            switch (celda) {
                case AGUA: {
                    int fase = ((int)(tiempo * 3.0) + (i / 2) + j) % 3;
                    if (fase == 0) simbolo = '~';
                    else if (fase == 1) simbolo = '-';
                    else simbolo = '.';
                    color = 9; // azul claro
                    break;
                }
                case TIERRA: simbolo = '#'; color = 6; break;
                case JUGADOR: simbolo = '@'; color = 15; break;
                case COMIDA: simbolo = 'f'; color = 10; break;
                case MADERA: simbolo = 'T'; color = 2; break;
                case RECURSO: simbolo = '$'; color = 14; break;
                default: simbolo = '?'; color = 12; break;
            }

            // Guardamos símbolo; coloreamos e imprimimos la fila completa después.
            filaBuf[j] = simbolo;

            // Establecemos atributo por carácter: para simplificar, imprimimos por carácter
            // pero sin mover el cursor línea a línea (eficiente suficiente para mapas pequeños).
            setColor(color);
            putchar(simbolo);
        }
        filaBuf[COLUMNAS] = '\0';
        // ya imprimimos la fila con putchar; al final de fila, reseteamos color e imprimimos newline
        setColor(7);
        putchar('\n');
    }

    // Imprimir línea de ayuda en la siguiente fila (si la consola tiene espacio)
    setColor(7);
    printf("Usa WASD o flechas para moverte | Q para salir\n");
}
