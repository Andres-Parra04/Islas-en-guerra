#include "mapa.h"
#include <stdlib.h>
#include <time.h>
#include <conio.h>

#define NUM_RECURSOS 15
#define NUM_ENEMIGOS 7

/* =============================== */
/* Utilidades de consola           */
/* =============================== */
void ocultarCursor() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hOut, &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(hOut, &cursorInfo);
}

void moverCursor(short x, short y) {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD pos;
    pos.X = x;
    pos.Y = y;
    SetConsoleCursorPosition(hOut, pos);
}

void setColor(int fondo, int texto) {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hOut, fondo * 16 + texto);
}

/* =============================== */
/* Generador de islas fijas        */
/* =============================== */
void crearIsla(char mapa[FILAS][COLUMNAS], int cx, int cy, int rx, int ry) {
    int i, j;
    int dx, dy, dist, deformacion, nx, ny;
    int maxX = rx + 2;
    int maxY = ry + 2;

    for (i = -maxY; i <= maxY; i++) {
        for (j = -maxX; j <= maxX; j++) {
            dx = j;
            dy = i;
            dist = dx * dx + dy * dy;
            deformacion = rand() % 5;
            if (dist <= (rx * ry) + deformacion) {
                nx = cy + i;
                ny = cx + j;
                if (nx >= 0 && nx < FILAS && ny >= 0 && ny < COLUMNAS)
                    mapa[nx][ny] = '.';
            }
        }
    }
}

/* =============================== */
/* Inicializar mapa                */
/* =============================== */
void inicializarMapa(char mapa[FILAS][COLUMNAS]) {
    int i, j, placed, rx, ry, ex, ey;

    srand((unsigned int)time(NULL));

    for (i = 0; i < FILAS; i++) {
        for (j = 0; j < COLUMNAS; j++) {
            mapa[i][j] = '~';
        }
    }

    // Ajustar posiciones de islas para que quepan en 35x50 y se vean equilibradas
    crearIsla(mapa, 7, 7, 6, 5);   // Esquina superior izquierda
    crearIsla(mapa, 42, 7, 7, 5);  // Esquina superior derecha (centro en col 42, radio ajustado)
    crearIsla(mapa, 7, 42, 7, 5);  // Esquina inferior izquierda
    crearIsla(mapa, 42, 42, 6, 5); // Esquina inferior derecha

    placed = 0;
    while (placed < NUM_RECURSOS) {
        rx = rand() % FILAS;
        ry = rand() % COLUMNAS;
        if (mapa[rx][ry] == '.') {
            mapa[rx][ry] = '$';
            placed++;
        }
    }

    placed = 0;
    while (placed < NUM_ENEMIGOS) {
        ex = rand() % FILAS;
        ey = rand() % COLUMNAS;
        if (mapa[ex][ey] == '.') {
            mapa[ex][ey] = 'E';
            placed++;
        }
    }
}

/* =============================== */
/* Mostrar mapa                    */
/* =============================== */
void mostrarMapa(char mapa[FILAS][COLUMNAS]) {
    int i, j;
    ocultarCursor();
    moverCursor(0, 0);

    for (i = 0; i < FILAS; i++) {
        for (j = 0; j < COLUMNAS; j++) {
            char c = mapa[i][j];
            if (c == '~') {
                setColor(1, 9);
                printf("~ ");
            } else if (c == '.') {
                setColor(2, 6);
                printf(". ");
            } else if (c == '$') {
                setColor(2, 14);
                printf("$ ");
            } else if (c == 'E') {
                setColor(4, 12);
                printf("E ");
            } else {
                setColor(0, 15);
                printf("%c ", c);
            }
        }
        printf("\n");
    }
    setColor(0, 15);
}

/* =============================== */
/* Mover jugador                   */
/* =============================== */
void moverJugador(char mapa[FILAS][COLUMNAS], int *x, int *y, char direccion) {
    int nx = *x;
    int ny = *y;
    char destino;
    char msg[60];
    int i;
    char actual;

    if (direccion >= 'a' && direccion <= 'z')
        direccion -= 32;

    if (direccion == 'W') nx--;
    else if (direccion == 'S') nx++;
    else if (direccion == 'A') ny--;
    else if (direccion == 'D') ny++;
    else return;

    if (nx < 0 || nx >= FILAS || ny < 0 || ny >= COLUMNAS) {
        moverCursor(0, FILAS + 1);
        setColor(0, 14);
        printf("No puedes salir del mapa!          ");
        setColor(0, 15);
        return;
    }

    destino = mapa[nx][ny];
    msg[0] = '\0';

    if (destino == '~') {
        moverCursor(0, FILAS + 1);
        setColor(0, 14);
        printf("No puedes nadar!                   ");
        setColor(0, 15);
        return;
    }

    if (destino == '$') {
        sprintf(msg, "Has encontrado un recurso!");
        mapa[nx][ny] = '.';
    } else if (destino == 'E') {
        sprintf(msg, "Has encontrado un enemigo!");
        mapa[nx][ny] = '.';
    }

    moverCursor((short)(*y * 2), (short)(*x));
    actual = mapa[*x][*y];
    if (actual == '~') setColor(1, 9);
    else if (actual == '.') setColor(2, 6);
    else if (actual == '$') setColor(2, 14);
    else if (actual == 'E') setColor(4, 12);
    else setColor(0, 15);
    printf("%c ", actual);

    moverCursor((short)(ny * 2), (short)(nx));
    setColor(0, 10);
    printf("P ");
    setColor(0, 15);

    *x = nx;
    *y = ny;

    moverCursor(0, FILAS + 1);
    for (i = 0; i < 60; i++) printf(" ");
    moverCursor(0, FILAS + 1);

    if (msg[0] != '\0') {
        setColor(0, 11);
        printf("%s", msg);
        setColor(0, 15);
    }
}

/* =============================== */
/* Animar agua con flujo (~ y ' ') */
/* =============================== */
void animarAgua(char mapa[FILAS][COLUMNAS]) {
    int i, j;
    static int frame = 0;
    frame++;

    for (i = 0; i < FILAS; i++) {
        for (j = 0; j < COLUMNAS; j++) {
            if (mapa[i][j] == '~') {
                moverCursor(j * 2, i);
                if ((i + j + frame) % 3 == 0) {
                    setColor(1, 9); printf("~ ");
                } else {
                    setColor(1, 9); printf("  ");
                }
            }
        }
    }
    setColor(0, 15);
}

/* =============================== */
/* Menú inicial                    */
/* =============================== */
void mostrarMenu() {
    int i;
    ocultarCursor();
    system("cls");

    printf("\n\n\n\n");
    printf("                    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    printf("                      ISLAS  EN  GUERRA  \n");
    printf("                    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");

    setColor(0, 14);
    printf("                 Presiona [ENTER] para comenzar\n\n");
    setColor(0, 8);
    printf("                  Desarrollado en C - 100%% CMD Edition\n\n");

    

    setColor(0, 15);
    

    while (1) {
        if (_kbhit() && _getch() == 13) break;
    }

    system("cls");
}

