#ifndef MAPA_H
#define MAPA_H

#include <windows.h>
#include <stdio.h>

#define FILAS 26
#define COLUMNAS 50

void inicializarMapa(char mapa[FILAS][COLUMNAS]);
void mostrarMapa(char mapa[FILAS][COLUMNAS]);
void moverJugador(char mapa[FILAS][COLUMNAS], int *x, int *y, char direccion);
void animarAgua(char mapa[FILAS][COLUMNAS]);
void ocultarCursor();
void moverCursor(short x, short y);
void setColor(int fondo, int texto);
void mostrarMenu();

#endif
