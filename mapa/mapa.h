#ifndef MAPA_H
#define MAPA_H

#include <windows.h>

#define FILAS 40
#define COLUMNAS 80

typedef enum {
    AGUA,
    TIERRA,
    COMIDA,
    MADERA,
    RECURSO,
    JUGADOR
} TipoCelda;

typedef struct {
    TipoCelda celdas[FILAS][COLUMNAS];
    int jugadorX;
    int jugadorY;
    TipoCelda celdaBajoJugador;
} Mapa;

void inicializarMapa(Mapa *mapa);
void moverJugador(Mapa *mapa, char dir);
void dibujarMapa(Mapa *mapa, double tiempo);
void inicializarConsola(int ancho, int alto);
void restaurarCursorVisible();

#endif
