#include "navegacion.h"
#include "../mapa/mapa.h"
#include <stdio.h>
#include <math.h>

// ============================================================================
// POSICIONES DE LAS 3 ISLAS EN MAPA GLOBAL (mapaDemo2.bmp)
// ============================================================================
// Estas son coordenadas aproximadas que debes ajustar según tu mapaDemo2.bmp
// Para cada isla almacenamos el centro y la posición de orilla para el barco
// ============================================================================

typedef struct {
  float centroX, centroY;  // Centro de la isla (para detección)
  float orillaX, orillaY;  // Posición de orilla para el barco en mapa global
} PosicionIsla;

// IMPORTANTE: Ajusta estas coordenadas según tu mapaDemo2.bmp
static PosicionIsla gPosicionesIslas[3] = {
  // Isla 1: ajustar según posición real en mapaDemo2.bmp
  {512.0f, 512.0f, 400.0f, 700.0f},
  
  // Isla 2: ajustar según posición real
  {1536.0f, 1024.0f, 1400.0f, 1024.0f},
  
  // Isla 3: ajustar según posición real
  {1024.0f, 1536.0f, 1024.0f, 1400.0f}
};

// ============================================================================
// FUNCIONES AUXILIARES
// ============================================================================

bool barcoContienePunto(Barco* barco, float mundoX, float mundoY) {
  const float BARCO_SIZE = 192.0f;
  
  return (mundoX >= barco->x && mundoX < barco->x + BARCO_SIZE &&
          mundoY >= barco->y && mundoY < barco->y + BARCO_SIZE);
}

void obtenerPosicionBarcoEnGlobal(int islaActual, float *outX, float *outY) {
  if (islaActual < 1 || islaActual > 3) islaActual = 1;
  
  *outX = gPosicionesIslas[islaActual - 1].orillaX;
  *outY = gPosicionesIslas[islaActual - 1].orillaY;
  
  printf("[DEBUG] Barco en isla %d posicionado en global: (%.1f, %.1f)\n", 
         islaActual, *outX, *outY);
}

int detectarIslaPorPosicion(float x, float y) {
  // Detectar en qué isla está el barco según su posición en mapa global
  for (int i = 0; i < 3; i++) {
    float dx = x - gPosicionesIslas[i].centroX;
    float dy = y - gPosicionesIslas[i].centroY;
    float dist = sqrt(dx*dx + dy*dy);
    
    // Si está dentro del radio de 400px de la isla, es esa isla
    if (dist < 400.0f) {
      printf("[DEBUG] Posición (%.1f, %.1f) detectada en isla %d\n", x, y, i + 1);
      return i + 1; // Isla 1, 2, o 3
    }
  }
  
  printf("[DEBUG] Posición (%.1f, %.1f) no pertenece a ninguna isla\n", x, y);
  return 0; // No detectada
}

void desembarcarTropas(Barco* barco, struct Jugador* j) {
  printf("[DEBUG] Desembarcando %d tropas...\n", barco->numTropas);
  
  for (int i = 0; i < barco->numTropas; i++) {
    Unidad* tropa = barco->tropas[i];
    if (tropa) {
      // Colocar tropas en formación 3x2 cerca del barco
      tropa->x = barco->x + (i % 3) * 70.0f;
      tropa->y = barco->y + (i / 3) * 70.0f + 200.0f;
      
      printf("[DEBUG] Tropa %d desembarcada en (%.1f, %.1f)\n", 
             i, tropa->x, tropa->y);
    }
  }
  
  // Vaciar barco
  barco->numTropas = 0;
}

// ============================================================================
// NAVEGACIÓN DEL BARCO
// ============================================================================

void barcoIniciarNavegacion(Barco* barco, float destinoX, float destinoY) {
  barco->destinoX = destinoX;
  barco->destinoY = destinoY;
  barco->navegando = true;
  barco->velocidad = 3.0f; // Píxeles por frame (ajustable)
  
  // Calcular dirección inicial
  float dx = destinoX - barco->x;
  float dy = destinoY - barco->y;
  
  if (fabs(dx) > fabs(dy)) {
    barco->dir = (dx > 0) ? DIR_RIGHT : DIR_LEFT;
  } else {
    barco->dir = (dy > 0) ? DIR_FRONT : DIR_BACK;
  }
  
  printf("[DEBUG] Barco iniciando navegación a (%.1f, %.1f)\n", destinoX, destinoY);
}

void barcoActualizarNavegacion(Barco* barco, struct Jugador* j) {
  if (!barco->navegando) return;
  
  float dx = barco->destinoX - barco->x;
  float dy = barco->destinoY - barco->y;
  float distancia = sqrt(dx*dx + dy*dy);
  
  if (distancia <= barco->velocidad) {
    // ¡LLEGÓ AL DESTINO!
    barco->x = barco->destinoX;
    barco->y = barco->destinoY;
    barco->navegando = false;
    
    printf("[DEBUG] Barco llegó al destino. Detectando isla...\n");
    
    // CRÍTICO: Detectar a qué isla llegó
    int islaDestino = detectarIslaPorPosicion(barco->x, barco->y);
    
    if (islaDestino != j->islaActual && islaDestino > 0) {
      printf("[DEBUG] Cambiando de isla %d a isla %d\n", j->islaActual, islaDestino);
      
      // Cambiar isla activa
      j->islaActual = islaDestino;
      
      // Cambiar mapa de isla
      mapaSeleccionarIsla(islaDestino);
      cargarRecursosGraficos(); // Recargar mapa y árboles
      
      // Posicionar barco en orilla de la nueva isla (vista local)
      float nuevoBarcoX, nuevoBarcoY;
      int nuevoDir;
      mapaDetectarOrilla(&nuevoBarcoX, &nuevoBarcoY, &nuevoDir);
      barco->x = nuevoBarcoX;
      barco->y = nuevoBarcoY;
      barco->dir = (Direccion)nuevoDir;
      
      printf("[DEBUG] Barco reposicionado en isla %d: (%.1f, %.1f)\n",
             islaDestino, barco->x, barco->y);
    }
    
    // Desembarcar tropas
    desembarcarTropas(barco, j);
    
    // Volver a vista local
    j->vistaActual = VISTA_LOCAL;
    
    return;
  }
  
  // Mover hacia el destino
  barco->x += (dx / distancia) * barco->velocidad;
  barco->y += (dy / distancia) * barco->velocidad;
  
  // Actualizar dirección visual
  if (fabs(dx) > fabs(dy)) {
    barco->dir = (dx > 0) ? DIR_RIGHT : DIR_LEFT;
  } else {
    barco->dir = (dy > 0) ? DIR_FRONT : DIR_BACK;
  }
}

void seleccionarIslaDestino(struct Jugador* j, int clickX, int clickY) {
  printf("[DEBUG] Click en vista global: (%d, %d)\n", clickX, clickY);
  
  // Las posiciones de las islas en el mapa global son fijas
  // Por ahora, navegar a una isla diferente de la actual
  
  // Ciclar entre las 3 islas
  int islaObjetivo = (j->islaActual % 3) + 1;
  
  printf("[DEBUG] Navegando desde isla %d hacia isla %d\n", j->islaActual, islaObjetivo);
  
  // Obtener coordenadas de destino en mapa global
  float destinoX = gPosicionesIslas[islaObjetivo - 1].centroX;
  float destinoY = gPosicionesIslas[islaObjetivo - 1].centroY;
  
  barcoIniciarNavegacion(&j->barco, destinoX, destinoY);
}
