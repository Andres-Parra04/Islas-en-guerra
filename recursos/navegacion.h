#ifndef NAVEGACION_H
#define NAVEGACION_H

#include "recursos.h"
#include <windows.h>

// Inicia la navegación del barco hacia coordenadas específicas
void barcoIniciarNavegacion(Barco* barco, float destinoX, float destinoY);

// Actualiza posición del barco durante navegación (llamar en timer)
void barcoActualizarNavegacion(Barco* barco, struct Jugador* j);

// Detecta la isla más cercana al punto clickeado
void seleccionarIslaDestino(struct Jugador* j, int clickX, int clickY);

// Verifica si un punto está dentro del barco (para detección de clicks)
bool barcoContienePunto(Barco* barco, float mundoX, float mundoY);

// NUEVO: Obtiene posición del barco en mapa global según isla actual
void obtenerPosicionBarcoEnGlobal(int islaActual, float *outX, float *outY);

// NUEVO: Detecta en qué isla está una posición del mapa global
int detectarIslaPorPosicion(float x, float y);

// NUEVO: Desembarca las tropas del barco
void desembarcarTropas(Barco* barco, struct Jugador* j);

#endif
